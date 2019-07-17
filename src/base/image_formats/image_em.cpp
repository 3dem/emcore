
#include "em/base/error.h"
#include "em/base/image.h"
#include "em/base/image_priv.h"

using namespace em;

#define EM_HEADER_SIZE    512 // Minimum size of the EM header (when nsymbt = 0)

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
 * Inherit properties from base ImageFile::Impl and add information
 * specific for EM format (e.g, the EmHeader struct)
 */
class EmImageFile: public em::ImageFile::Impl
{
public:
    EmHeader header;

    virtual void readHeader() override
    {
        std::cout << "DEBUG:EmImageFile:readHeader: file " << path << std::endl;

        // Try to read the main header from the (already opened) file stream
        if (fread(&header, EM_HEADER_SIZE, 1, file) < 1)
            THROW_SYS_ERROR(
                    std::string("Error reading EM header in file: ") + path);

        bool isLE = Type::isLittleEndian();

        // If machine is SGI, OS-9 or MAC: Big Endian, otherwise Little Endian
        // Check Machine endianess
        if (header.machine == 0 || header.machine == 3 || header.machine == 5)
            swap = isLE;
        else if (header.machine == 1 || header.machine == 2 ||
                 header.machine == 4 || header.machine == 6)
            swap = !isLE;
        else
            THROW_SYS_ERROR("EmImageFile::readHEader: Unknown source machine to "
                                    "determine Endianness");

        if (swap)
        {
            Type::swapBytes((char *) &(this->header.xdim), 3, typeUInt32.getSize());
            Type::swapBytes((char *) &(this->header.params), 40, typeUInt32.getSize());
        }

        // Check dimensions of the data taking into account
        // if it is a 2D or 3D stack
        dim.x = (size_t) header.xdim;
        dim.y = (size_t) header.ydim;
        dim.z = (size_t) header.zdim;

        type = getTypeFromMode((char) header.datatype);
        ASSERT_ERROR(type.isNull(), "Unknown EM type mode.");

    } // function readHeader

    virtual void writeHeader() override
    {
        // Set everything with zeros
        memset(&header, 0, EM_HEADER_SIZE);
        header.datatype = (char) getModeFromType(type);

        header.xdim = (int) dim.x;
        header.ydim = (int) dim.y;
        header.zdim = (int) dim.z;

        //FIXME: Do we need to check about endianess and machine?

        //FIXME: Use proper machine code, now hard-coded PC
        header.machine = (char) 6;

        fwrite(&header, EM_HEADER_SIZE, 1, file);

    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
        return EM_HEADER_SIZE;
    } // function getHeaderSize

    virtual const IntTypeMap & getTypeMap() const override
    {
        static const IntTypeMap tm = {
               {1, typeInt8},
               {2, typeInt16},
               {4, typeInt32},
               {5, typeFloat},
// TODO:         //{8, &TypeComplex},
               {9, typeDouble}};
        return tm;
    } // function getTypeMap

}; // class EmImageFile

StringVector emExts = {"em"};

REGISTER_IMAGE_IO(emExts, EmImageFile);
