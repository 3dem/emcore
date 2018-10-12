#include <iomanip>

#include "em/base/error.h"
#include "em/os/filesystem.h"
#include "em/base/image.h"
#include "em/base/image_priv.h"


using namespace em;


#define IMAGIC_HEADER_SIZE 1024 // Size of the IMAGIC header for each image

struct ImagicHeader
{
    // File header for Imagic data
    // (https://www.imagescience.de/formats.html)

    // Variable              Word     Bytes      Description
    int imn;             //  1        1-4        Image location number (1,2,3,...)
    int ifol;            //  2        5-8        Number of all 1D/2D images/sections following (0,1...).
    int ierror;          //  3	      9-12       Error code for this image during IMAGIC run
    int nblocks;         //  4        13-16      Number of header blocks (each block containing 256 REAL/INTEGER values).
                         //                      For a usual header: NBLOCKS = 1
    int nmonth;          //  5        17-20      Creation month
    int nday;            //  6        21-24      Creation day
    int nyear;           //  7        25-28      Creation year
    int nhour;           //  8        29-32      Creation hour
    int nminut;          //  9        33-36      Creation minute
    int nsec;            //  10       37-40      Creation second
    int rsize;           //  11       41-44      Image size in BYTES as seen from the program
    int izold;           //  12       45-48      Top left Z coordinate before THREED-CUT
    int ixlp;            //  13       49-52      Number of lines per image (for 1D data IXLP=1)
    int iylp;            //  14       53-56      Number of pixels per line
    char type[4];        //  15       57-60      4 coded characters determining the image type:
                         //                      REAL : Each image pixel is represented by a 32-bit real/float number
                         //                      LONG : Each image pixel is represented by a 32-bit (signed) integer number
                         //                      INTG : Each image pixel is represented by a 16-bit (signed) integer number
                         //                      PACK : Each image pixel is represented by one (unsigned) byte number
                         //                      COMP : Each complex image pixel is represented by two REAL values
    int ixold;           //  16       61-64      Top left X coordinate before CUT-IMAGE
    int iyold;           //  17       65-68      Top left Y coordinate before CUT-IMAGE
    float avdens;        //  18       69-72      Average density in image
    float sigma;         //  19       73-76      Sigma of image densities
    float user1;         //  20       77-80      At user's own disposal
    float user2;         //  21       81-84      At user's own disposal
    float densmax;       //  22       85-88      Highest density in image
    float densmin;       //  23       89-92      Minimal density in image
    int complex;         //  24       93-96      Label indicating that data is always complex
    float defocus1;      //  25       97-100     Defocus value 1 (Angstrom)
    float defocus2;      //  26       101-104    Defocus value 2 (Angstrom)
    float defangle;      //  27       105-108    Defocus angle (degree)
    float sinostrt_wavemax;//28       109-112    Start angle if image is a sinogram. Maximal wave number if image is a spectra
    float sinoend_wavstep;// 29       113-116    End angle if image is a sinogram. Step size of wave numbers if image is a spectra
    char name[80];       //  30-49    117-196    Coded NAME/TITLE of the image (80 characters)
    float ccc3d;         //  50       197-200    3D simularity criteria
    int ref3d;           //  51       200-203    3D membership
    int mident;          //  52       204-207    Micrograph identification number
    float ezshift;       //  53       208-211    Equivalent shift in Z direction
    float ealpha;        //  54       212-215    Equivalent Euler angle alpha (degree)
    float ebeta;         //  55       216-219    Equivalent Euler angle beta (degree)
    float egamma;        //  56       220-223    Equivalent Euler angle gamma (degree)
    int ref3dold;        //  57       224-227    Old 3D membership before last ANG-RECONST/ANCHORSET (EACH_TO_BEST)
    int active;          //  58       228-231    Image is active or not (EXLUSIVE-COPY and HEADERS):
                         //                      ACTIVE > 0 : image is set active
                         //                      ACTIVE < 0 : image is set inactive
                         //                      ACTIVE = 0 : not set (treated as being set active)
    int nalisum;         //  59       232-235    Number of images summed (used in SUM-ALIGNED-IMAGES)
    int pgroup;          //  60       236-239    Point-group symmetry in international notation (622, for example)
    int izlp;            //  61       240-243    Number of 2D planes in 3D data (for 1D/2D: IZLP1=1)
    int i4lp;            //  62       244-247    Number of "objects" in file:
                         //                      1D (IXLP=1): number of 1D spectra
                         //                      2D (IZLP=1): number of 2D images
                         //                      3D (IZLP>1): number of 3D volumes
    int i5lp;            //  63       248-251    No comments: (https://www.imagescience.de/formats.html)
    int i6lp;            //  64       252-255    No comments: (https://www.imagescience.de/formats.html)
    float alpha;         //  65       256-259    Euler angle alpha (3D and angular reconstitution) (degree)
    float beta;          //  66       260-263    Euler angle beta (3D and angular reconstitution) (degree)
    float gamma;         //  67       264-267    Euler angle gamma (3D and angular reconstitution) (degree)
    int imavers;         //  68       268-271    IMAGIC version, which created the file (yyyymmdd)
    int real_type;       //  69       272-275    Floating point type / machine stamp:
                         //                      16777216: VAX/VMS
                         //                      33686018: Linux, Unix, Mac OSX, MS Windows, OSF, ULTRIX
                         //                      67372036: SiliconGraphics, SUN, HP, IBM
    int locmicro;        //  70       276-279    Location of the picked particle in the original micrograph
    int ixmicro;         //  71       280-283    Central X coordinate of the picked particle in the original micrograph
    int iymicro;         //  72       284-287    Central Y coordinate of the picked particle in the original micrograph
    int locorig;         //  73       288-291    Location of the picked particle in the "original" file
                         //                      (usually the file created by PICK-PARTICLE or CUT-IMAGE/APERIODIC)
    int numframe;        //  74       292-295    If file contains a movie or the like: number of frames in each movie
                         //                      If file does not contain a movie or the like: 0
    int pupsize;         //  75       296-299    Typical upper size of particle(s) in the image (in pixels)
    int plowsize;        //  76       300-303    Typical lower size of particle(s) in the image (in pixels)
    int pickedgx;        //  77       304-307    Minimal X distance from edge (PICK-PARTICLES)
    int pickedgy;        //  78       308-311    Minimal Y distance from edge (PICK-PARTICLES)
    char buffctrl[60];   //  79-93    312-371    Old I/O parameters, which are no more used
    int readim;          //  94       372-375    I/O parameter during IMAGIC run: Image is read in from file or not
                         //                      (No meaning in a closed file)
    int writeim;         //  95       376-379    I/O buffering parameter during IMAGIC run: Image is dumped to file or not
                         //                      (No meaning in a closed file)
    int num;             //  96       380-383    I/O parameter during IMAGIC run: file number of image file
                         //                      (No meaning in a closed file)
    int numhead;         //  97       384-387    I/O parameter during IMAGIC run: file number of header file
                         //                      (No meaning in a closed file)
    int lbuf;            //  98       388-391    I/O parameter during IMAGIC run: old READIM/WRITIM parameter
                         //                      0: buffer whole image -1: read/write line by line
                         //                      (No meaning in a closed file)
    int readonly;        //  99       392-395    I/O parameter during IMAGIC run: open file action mode
                         //                      1: open read only 0: open read/write (No meaning in a closed file)
    float angle;         //  100      396-399    Last rotation angle (degree)
    float voltage;       //  101      400-403    Acceleration voltage (Volt)
    float sphaber;       //  102      404-407    Spherical aberration (mm)
    float focdist;       //  103      408-411    Focal distance of objective (mm)
    float ccc;           //  104      412-415    Cross correlation peak height
    float errar;         //  105      416-419    Error in angular reconstitution
                         //                      if -1.0: the file is a special file (FABOSA)
    float err3d;         //  106      420-423    Error in 3D reconstruction
    int ref;             //  107      424-427    (Multi-) reference number
    float classno;       //  108      428-431    Class number in MSA classification
    float locold;        //  109      432-435    Location number before CUT-IMAGE or before copying the output images in ANG-REC/NEW
    float repqual;       //  110      436-439    Representation quality. Used in MSA-RUN and MSA (eigen) filtering
    float zshift;        //  111      440-443    Last shift in Z direction
    float xshift;        //  112      444-447    Last shift in X direction
    float yshift;        //  113      448-451    Last shift in Y direction
    float numcls;        //  114      452-455    Number of members in the class specified in CLASSNO, if this image
                         //                      represents a class average (class-sum image)
    float ovqual;        //  115      456-459    Overall quality of the class in CLASSNO
    float eangle;        //  116      460-463    Equivalent angle (degree)
    float exshift;       //  117      464-467    Equivalent shift in X direction
    float eyshift;       //  118      468-471    Equivalent shift in Y direction
    float cmtotvar;      //  119      472-475    Total variance in data matrix relative to centre of mass (MSA calculations)
    float informat;      //  120      476-479    Gauss norm / real*FT space information of the data set
    int numeigen;        //  121      480-483    Number of eigenvalues/eigenimages in MSA
    int niactive;        //  122      484-487    Number of active images in MSA calculations
    float pixsize;       //  123      488-491    Pixel/Voxel size (Angstrom). If DAT1(105) = -1.0 (FABOSA): mm per pixel
    float aperture;      //  124      492-495    Aperture in the back focal plane of the objective lens (in micrometer)
    float ctf;           //  125      496-499    CTF quality
    float alpha2;        //  126      500-503    Euler angle alpha (from projection matching) (degree)
                         //                      NOTE: Specific FABOSA variable if DAT1(105) = -1.0
    float beta2;         //  127      504-507    Euler angle beta (from projection matching) (degree)
                         //                      NOTE: Specific FABOSA variable if DAT1(105) = -1.0
    float gamma2;        //  128      508-511    Euler angle gamma (from projection matching) (degree)
                         //                      NOTE: Specific FABOSA variable if DAT1(105) = -1.0
    float nmetric;       //  129      512-515    Metric used in MSA calculations
    float actmsa;        //  130      516-519    A flag indicating whether the "image" is/was active or not during MSA calculations
    float coosmsa[69];   //  131-199  520-795    MSA coordinates of the "image" along factorial axes (1 through 69).
                         //                      If more than 69 eigenimages were used all MSA coordinates are stored
                         //                      in an additional header block.
                         //                      cossmsa[] is Eigenvalue if the "image" represents an eigenimage.
    char history[228];   //  200-256  796-1024   Coded history of image (228 characters)


}; // ImagicHeader


/**
 * Inherit properties from base ImageIO::Impl and add information
 * specific for IMAGIC format (e.g, the ImagicHeader struct)
 */
class ImageIOImagic: public em::ImageIO::Impl
{
public:
    ImagicHeader header; // main header
    FILE * headerFile = nullptr;  // Keep an extra file handler for the header
    std::string headerPath;  // Keep the path of the header file

    virtual void readHeader() override
    {
        // Try to read the main header from the (already opened) file stream
        if (fread(&header, IMAGIC_HEADER_SIZE, 1, headerFile) < 1)
            THROW_SYS_ERROR(std::string("Error reading IMAGIC header in file: ") + path);

        dim.x = header.ixlp;
        dim.y = header.iylp;
        dim.z = 1;
        dim.n = header.ifol + 1;

        int mode = -1;
        if (strstr(header.type, "PACK"))
            mode = 0;
        else if (strstr(header.type,"INTG"))
            mode = 1;
        else if (strstr(header.type,"REAL"))
            mode = 3;
        else if (strstr(header.type,"COMP"))
            mode = 4;
        else if (strstr(header.type,"LONG"))
            mode = 2;

        type = getTypeFromMode(mode);
        ASSERT_ERROR(type.isNull(), "Unknown IMAGIC type mode.");
    } // function readHeader

    virtual void writeHeader() override
    {
        // FIXME: Review again the specification, I think it does support volumes
        if (dim.z > 1)
             THROW_SYS_ERROR(std::string("Error writing header. IMAGIC format does not support volumes. File: ") + path);

        auto mode = getModeFromType(type);
        char dtype[4];

        switch(mode){
        case 0:
            memcpy(dtype, "PACK", 4);
        break;
        case 1:
            memcpy(dtype, "INTG", 4);
        break;
        case 2:
            memcpy(dtype, "LONG", 4);
        break;
        case 3:
            memcpy(dtype, "REAL", 4);
        break;
        case 4:
            memcpy(dtype, "COMP", 4);
        break;
        default:
            THROW_SYS_ERROR("Unsupported data type for IMAGIC format.");
        }

        header.imn = 1;
        header.ifol = dim.n - 1;
        header.nblocks = dim.n;
        header.iylp = dim.x;
        header.ixlp = dim.y;
        strncpy(header.type, dtype, 4);

        //TODO[pedrohv]: Implements others. See: "The values that must be set are shown with a blue background"
        //                                  https://www.imagescience.de/formats.html

        //TODO: If this code is repeated in other places, consider a Function/Class
        // that can be placed in timer.h in our base module
        time_t timer;
        time ( &timer );
        tm* t = localtime(&timer);
        header.nmonth = t->tm_mon + 1;
        header.nday = t->tm_mday;
        header.nyear = t->tm_year;
        header.nhour = t->tm_hour;
        header.nminut = t->tm_min;
        header.nsec = t->tm_sec;
        header.izlp = 1;
        //header.imavers = ????
        //FIXME[pedrohv]: Implements all headers data
        fwrite(&header, IMAGIC_HEADER_SIZE, 1, headerFile);
    } // function writeHeader

    virtual const IntTypeMap & getTypeMap() const override
    {
        static const IntTypeMap tm = {
               {0, typeUInt8},
               {1, typeInt16},
               {2, typeInt32},
               {3, typeFloat}
        };
        // TODO:
        // 4: Complex float
        return tm;
    } // function getTypeMap

    virtual void toStream(std::ostream &ostream, int verbosity) const override
    {

    } // function toStream

    /** Open the file for IMAGIC format. The path and mode
     * should be set before calling this function.
     */
    void openFile() override
    {
        auto ext = Path::getExtension(path);

        if (ext != "hed" && ext != "img")
            THROW_ERROR(std::string("Invalid IMAGIC extension: ") + ext);

        auto base = Path::removeExtension(path);
        // Keep path always for image file (.img)
        // and headerPath for the header file (.hed)
        path = base + ".img";
        headerPath = base + ".hed";

        file = fopen(path.c_str(), getModeString());

        if (file == nullptr)
            THROW_SYS_ERROR(std::string("Error opening file: ") + path);

        headerFile = fopen(headerPath.c_str(), getModeString());

        if (headerFile == nullptr)
            THROW_SYS_ERROR(std::string("Error opening file ") + headerPath);
    } // function openFile

    void closeFile()
    {
        Impl::closeFile();
        if (headerFile != nullptr)
        {
            fclose(headerFile);
            headerFile = nullptr;
        }
    }

    void expandFile()
    {
        // Compute the size of one item, taking into account its x, y, z dimensions
        // and the size of the type that will be used
        size_t itemSize = getImageSize();

        // Compute the total size of the file taking into account the general header
        // size and the size of all items (including extra padding per item)
        size_t fileSize = itemSize * dim.n;

        File::resize(file, fileSize);
        fflush(file);

        fileSize = IMAGIC_HEADER_SIZE * dim.n;
        File::resize(headerFile, fileSize);
        fflush(headerFile);
    } // function expandFile
}; // class ImageIOImagic

StringVector imagicExts = {"hed", "img"};

REGISTER_IMAGE_IO(imagicExts, ImageIOImagic);

