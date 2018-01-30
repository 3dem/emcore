
#include "em/base/error.h"
#include "em/image/image.h"
#include "em/image/image_priv.h"

using namespace em;


#define MRC_HEADER_SIZE    1024 // Minimum size of the MRC header (when nsymbt = 0)


struct MrcHeader
{
    // File header for MRC data
    // (following format MRC2014, http://www.ccpem.ac.uk/mrc_format/mrc2014.php)

    // Variable              Word   Bytes   Description
    int nx;              //  1      1-4     number of columns in 3D data array (fast axis, x dimension)
    int ny;              //  2      5-8     number of rows in 3D data array (medium axis, y dimension)
    int nz;              //  3	    9-12	number of sections in 3D data array (slow axis, z*n dimension)
    int mode;            //  4      13-16   0  8-bit signed integer (range -127 to 127)
                         //                 1  16-bit signed integer
                         //                 2  32-bit signed real (float)
                         //                 3  transform: complex 16-bit integers
                         //                 4  transform: complex 32-bit reals
                         //                 6  16-bit unsigned integer
    int nxstart;         //  5      17-20   location of first column in unit cell
    int nystart;         //  6      21-24   location of first row in unit cell
    int nzstart;         //  7      25-28   location of first section in unit cell
    int mx;              //  8      29-32   sampling along X axis of unit cell
    int my;              //  9      33-36   sampling along Y axis of unit cell
    int mz;              //  10     37-40   sampling along Z axis of unit cell (z dimension in EM)
                         //                 MRC2014: For EM, where there is no unit cell, MZ represents the number of
                         //                 sections in a single volume. For a volume stack, NZ/MZ will be the
                         //                 number of volumes in the stack. For images, MZ = 1.
    float cella[3];      // 11-13   41-52   cell dimensions in Angstroms
    float cellb[3];      // 14-16   53-64   cell angles in degrees
    int mapc;            // 17      65-68	axis corresp to cols (1,2,3 for X,Y,Z)
    int mapr;            // 18      69-72	axis corresp to rows (1,2,3 for X,Y,Z)
    int maps;            // 19      73-76	axis corresp to sections (1,2,3 for X,Y,Z)

    float dmin;          // 20      77-80   minimum density value
    float dmax;          // 21      81-84   maximum density value
    float dmean;         // 22      85-88   mean density value
                         //                 MRC2014: Density statistics may not be kept up-to-date for image/volume stacks,
                         //                 since it is expensive to recalculate these every time a new image/volume
                         //                 is added/deleted. It has been proposed the following convention:
                         //                 DMAX < DMIN, DMEAN < (smaller of DMIN and DMAX),
                         //                 RMS < 0 each indicate that the quantity in question is not well determined.
    int ispg;            // 23      89-92   space group number
                         //                 MRC2014: 0=Image/stack,1=Volume,401=volumes stack
    int nsymbt;          // 24      93-96   size of extended header (which follows main header) in bytes
    float extra1[2];     // 25-26   97-104  extra space (first two words)
    char exttyp[4];      // 27     105-108  code for the type of extended header
                         //                 MRC2014: Agreed values are:
                         //                          CCP4  Format from CCP4 suite
                         //                          MRC0  MRC format
                         //                          SERI  SerialEM
                         //                          AGAR  Agard
                         //                          FEI1  FEI software, e.g. EPU and Xplore3D, Amira, Avizo
    int nversion;        // 28     109-112  version of the MRC format
                         //                 MRC2014: The version of the MRC format that the file adheres to, specified
                         //                 as a 32-bit integer and calculated as:
                         //                 Year * 10 + version within the year (base 0)
                         //                 For the current format change, the value would be 20140
    float extra2[21];    // 29-49  113-196  extra space
    float origin[3];     // 50-52  197-208  phase origin (pixels) or origin of subvolume (A)
    char map[4];         // 53     209-212  character string 'MAP ' to identify file type
    char machst[4];      // 54     213-216  machine stamp encoding byte ordering of data
                         //                 MRC2014: Bytes 213 and 214 contain 4 `nibbles' (half-bytes) indicating the
                         //                 representation of float, complex, integer and character datatypes.
                         //                 Bytes 215 and 216 are unused. The CCP4 library contains a general
                         //                 representation of datatypes, but in practice it is safe to use
                         //                 0x44 0x44 0x00 0x00 for little endian machines, and
                         //                 0x11 0x11 0x00 0x00 for big endian machines.
                         //                 The CCP4 library uses this information to automatically byte-swap data
                         //                 if appropriate, when tranferring data files between machines.
    float rms;           // 55     217-220  rms deviation of map from mean density
    int nlabl;           // 56     221-224  number of labels being used
    char labels[800];    // 57-256 225-1024 10 80-character labels
}; // MrcHeader


/**
 * Inherit properties from base ImageIO::Impl and add information
 * specific for MRC format (e.g, the MrcHeader struct)
 */
class ImageIOMrc: public em::ImageIO::Impl
{
public:
    MrcHeader header;

    virtual void readHeader() override
    {
        std::cout << "DEBUG:ImageIOMrc:readHeader: file " << path << std::endl;

        // Try to read the main header from the (already opened) file stream
        if ( fread(&header, MRC_HEADER_SIZE, 1, file) < 1 )
            THROW_SYS_ERROR(std::string("Error reading MRC header in file: ") + path);

        bool isImgStack = (header.ispg == 0 and header.nx > 1);
        bool isVolStack = (header.ispg == 401);

        // Check dimensions of the data taking into account
        // if it is a 2D or 3D stack
        dim.x = (size_t) header.nx;
        dim.y = (size_t) header.ny;

        if (isImgStack)
        {
            dim.z = 1;
            dim.n = header.nz;
        }
        else if (isVolStack)
        {
            dim.z = header.mz;
            dim.n = header.nz / header.mz;
        }
        //if (isImgStack) TODO: default option?
        else
        {
            dim.z = header.mz;
            dim.n = header.nz / header.mz;
        }

        type = getTypeFromMode(header.mode);
        ASSERT_ERROR(type.isNull(), "Unknown MRC type mode.");

        // TODO: Check special cases where image is a transform
        // TODO: Determine swap order (little vs big endian)

    } // function readHeader

    virtual void writeHeader() override
    {
        memset(&header, 0, MRC_HEADER_SIZE);

        // FIXME: Implement more general mechanism of Type matching
        if (type == em::TypeDouble || type == em::TypeFloat ||
            type == em::TypeInt32 || type == em::TypeUInt32)
            header.mode = 2;
        else if (type == em::TypeInt16)
            header.mode = 1;
        else if (type == em::TypeUInt16)
            header.mode = 6;
        else if (type == em::TypeInt8 || type == em::TypeUInt8)
            header.mode = 0;
            // TODO: Implement complex float and double
        else
            THROW_ERROR("Unsupported type for MRC format. ");

        // Map the parameters
        strncpy(header.map, "MAP ", 4);

        // Set the machine Endianess
        auto machine_stamp = static_cast<char *>(header.machst);
        if (isLittleEndian())
        {
            machine_stamp[0] = 68;
            machine_stamp[1] = 65;
        }
        else
        {
            machine_stamp[0] = machine_stamp[1] = 17;
        }

        // Set Image dimensions and Pixel size
        header.mx = header.nx = dim.x;
        header.my = header.ny = dim.y;
        header.nz = dim.z * dim.n;
        header.mz = dim.z;

        float pixelSize = 1; // FIXME

        header.cella[0] = dim.x * pixelSize;
        header.cella[1] = dim.y * pixelSize;
        header.cella[2] = dim.z * pixelSize;

        // FIXME: consider complex transforms
        //    if ( transform == CentHerm )
        //        header.nx = Xdim/2 + 1;        // If a transform, physical storage is nx/2 + 1

        // Cells angles in degrees, only used in Crystallography
        header.cellb[0] = header.cellb[1] = header.cellb[2] = 90.; // FIXME Why 90?

        header.mapc = 1;
        header.mapr = 2;
        header.maps = 3;

        // For the moment, set that Image statistics are not well determined
        header.rms = -1;

        // Set if volume or not, if stack or not
        // ispg = 0 if image or stack, 1 if volume, 401 if volume stack
        header.ispg = (dim.z == 1) ? 0 : (dim.n == 1) ?  1 : 401;
        header.nsymbt = 0;
        header.nlabl = 10; // FIXME: or zero?

        std::cout << "header size: " << sizeof(MrcHeader)
                  << "MRC_HEADER_SIZE: " << MRC_HEADER_SIZE
                  << std::endl;
        fwrite(&header, MRC_HEADER_SIZE, 1, file);

        // FIXME: consider swap
        // if ( swapWrite )
        //   swapPage((char *) header, MRCSIZE - 800, DT_Float);

    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
        return MRC_HEADER_SIZE;
    } // function getHeaderSize

    virtual const TypeMap & getTypeMap() const override
    {
        static const TypeMap tm = {{0, &TypeInt8}, {1, &TypeInt16},
                                   {2, &TypeFloat}, {6, &TypeUInt16}};
        // TODO:
        // 3: Complex short
        // 4: Complex float
        return tm;
    } // function getTypeMap

}; // class ImageIOMrc

StringVector mrcExts = {"mrc", "mrcs"};

REGISTER_IMAGE_IO(mrcExts, ImageIOMrc);

