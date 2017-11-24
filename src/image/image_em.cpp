
#include "em/base/error.h"
#include "em/image/image.h"
#include "em/image/image_priv.h"

using namespace em;

#define EM_HEADER_SIZE    1024 // Minimum size of the EM header (when nsymbt = 0)

struct EmHeader
{
    char machine;
    /**  Machine:    Value:
          OS-9         0
          VAX          1
          Convex       2
          SGI          3
          Mac          5
          PC           6 */
    char gPurpose;  //> General purpose. On OS-9 system: 0 old version 1 is new version
    char unused;   //> Not used in standard EM-format, if this byte is 1 the header is abandoned.
    char datatype;
    /** Data TypeCoding:
     *  Image Type:  No. of Bytes:   Value:
          byte            1           1
          short           2           2
          long int        4           4
          float           4           5
          complex         8           8
          double          8           9 */
    //>  Three long integers (3x4 bytes) are image size in x, y, z Dimension
    int xdim;
    int ydim;
    int zdim;
    char comment[80]; //> 80 Characters as comment
    long int params[40]; //> 40 long integers (4 x 40 bytes) are user defined parameters
    /**  The parameters are coded as follwing:
    No.  |  Name  |  Value  |  Factor  |  Comment
    1       U        Volt      1000       accelerating voltage
    2       COE      ???m        1000       Cs of objective lense
    3       APE      mrad      1000       aperture
    4       VE       x         1          end magnification
    5       VN       -         1000       postmagnification of CCD
    6       ET       s         1000       exposure time in seconds
    7       XG       -         1          pixelsize in object-plane
    8       DG       nm        1000       EM-Code:
    EM420=1;
    CM12=2;
    CM200=3;
    CM120/Biofilter=4;
    CM300=5;
    CM300/Tecnai=6;
    extern=0;
    9       APD      nm        1000       photometer aperture
    10      L        nm        1000       phys_pixel_size * nr_of_pixels
    11      DF       Angstr.   1          defocus, underfocus is neg.
    12      FA       Angstr.   1          astigmatism
    13      PHI      deg/1000  1000       angle of astigmatism
    14      DR       Angstr.   1          drift in Angstr.
    15      DELT     deg/1000  1000       direction of drift
    16      DDF      Angstr.   1          focusincr. for focus-series
    17      X0       -         1          obsolete
    18      Y0       -         1          obsolete
    19      KW       deg/1000  1000       tiltangle
    20      KR       deg/1000  1000       axis perpend. to tiltaxis
    21      -        Angstr.   1
    22      SC       ASCII     1
    23      -        -         -
    24      -        pixel     1          internal: subframe X0
    25      -        pixel     1          internal: subframe Y0
    26      -        Angstr.   1000       internal: resolution
    27      -        -         -          internal: density
    28      -        -         -          internal: contrast
    29      -        -         -          internal: unknown
    30      SP       -         1000       mass centre X
    31      SP       -         1000       mass centre Y
    32      SP       -         1000       mass centre Z
    33      H        -         1000       height
    34      -        -         1000       internal: unknown
    35      D1       -         1000       width 'Dreistrahlbereich'
    36      D2       -         1000       width 'Achrom. Ring'
    37      -        -         1          internal: lambda
    38      -        -         1          internal: delta theta
    39      -        -         1          internal: unknown
    40      -        -         1          internal: unknown   */
    char userdata[256]; //>  256 Byte with userdata, i.e. the username
}; // EmHeader


/**
 * Inherit properties from base ImageIOImpl and add information
 * specific for EM format (e.g, the EmHeader struct)
 */
class ImageIOEm: public em::ImageIOImpl
{
public:
    EmHeader header;

    virtual void readHeader() override
    {
        std::cout << "DEBUG:ImageIOEm:readHeader: file " << path << std::endl;

        // Try to read the main header from the (already opened) file stream
        if ( fread(&header, EM_HEADER_SIZE, 1, file) < 1 )
            THROW_SYS_ERROR(std::string("Error reading EM header in file: ") + path);

        // endian: If machine is SGI, OS-9 or MAC: Big Endian, otherwise Litle Endian
        // Check Machine endianess
        bool isLE = IsLittleEndian();

        if (header.machine == 0 || header.machine == 3 || header.machine == 5)
            swap = isLE;
        else if (header.machine == 1 || header.machine == 2 || header.machine == 4 || header.machine == 6)
            swap = !isLE;
        else
            THROW_SYS_ERROR("ImageIOEm::readHEader: Unknown source machine to "
                                    "determine Endianness");

        //if (swap)
        //    swapPage((char *) &header, EMHEADERSIZE - 256, DT_UInt); // EMHEADERSIZE - 256 is to exclude userdata from swapping


        // Check dimensions of the data taking into account
        // if it is a 2D or 3D stack
        dim.x = (size_t) header.xdim;
        dim.y = (size_t) header.ydim;
        dim.z = (size_t) header.zdim;

        type = getTypeFromMode((char) header.datatype);
        ASSERT_ERROR(type == nullptr, "Unknown EM type mode.");

    } // function readHeader

    virtual void writeHeader() override
    {
        memset(&header, 0, EM_HEADER_SIZE);

        // FIXME: Implement more general mechanism of Type matching
        if (type == em::TypeDouble || type == em::TypeFloat ||
            type == em::TypeInt32 || em::TypeUInt32)
            header.mode = 2;
        else if (type == em::TypeInt16)
            header.mode = 1;
        else if (type == em::TypeUInt16)
            header.mode = 6;
        else if (type == em::TypeInt8 || type == em::TypeUInt8)
            header.mode = 0;
            // TODO: Implement complex float and double
        else
            THROW_ERROR("Unsupported type for EM format. ");

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

        std::cout << "header size: " << sizeof(EmHeader)
                  << "EM_HEADER_SIZE: " << EM_HEADER_SIZE
                  << std::endl;
        fwrite(&header, EM_HEADER_SIZE, 1, file);

        // FIXME: consider swap
        // if ( swapWrite )
        //   swapPage((char *) header, EMSIZE - 800, DT_Float);

    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
        return EM_HEADER_SIZE;
    } // function getHeaderSize

    virtual const TypeMap & getTypeMap() const override
    {
        static const TypeMap tm = {{1, TypeInt8},
                                   {2, TypeInt16},
                                   {4, TypeInt32},
                                   {5, TypeFloat},
                // TODO:         //{8, TypeComplex},
                                   {9, TypeDouble}};
        return tm;
    } // function getTypeMap

}; // class ImageIOEm

StringVector emExts = {"em"};

REGISTER_IMAGE_IO(emExts, ImageIOEm);
