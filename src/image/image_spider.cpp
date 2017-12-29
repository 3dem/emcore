
#include <math.h>
#include "em/base/error.h"
#include "em/image/image.h"
#include "em/image/image_priv.h"

using namespace em;


#define SPIDER_HEADER_SIZE 1024 // Minimum size of the SPIDER header (variable)

/** Swapping trigger.
 * Threshold file z size above which bytes are swapped.
 */
#define SWAPTRIG     16776960

struct SpiderHeader
{                    // file header for SPIDER data
    float nslice;    //  0      slices in volume (image = 1)
    float nrow;      //  1      rows per slice
    float irec;      //  2      # records in file (unused)
    float nhistrec;  //  3      (obsolete)
    float iform;     //  4      file type specifier
    float imami;     //  5      max/min flag (=1 if calculated)
    float fmax;      //  6      maximum
    float fmin;      //  7      minimum
    float av;        //  8      average
    float sig;       //  9      standard deviation (=-1 if not calculated)
    float ihist;     // 10      (obsolete)
    float nsam;      // 11      pixels per row
    float labrec;    // 12      # records in header
    float iangle;    // 13      flag: tilt angles filled
    float phi;       // 14      tilt angles
    float theta;     // 15
    float gamma;     // 16      (=psi)
    float xoff;      // 17      translation
    float yoff;      // 18
    float zoff;      // 19
    float scale;     // 20      scaling
    float labbyt;    // 21      # bytes in header
    float lenbyt;    // 22      record length in bytes (row length)
    float istack;    // 23      indicates stack of images
    float inuse;     // 24      indicates this image in stack is used (not used)
    float maxim;     // 25      max image in stack used
    float imgnum;    // 26      number of current image
    float unused[2]; // 27-28     (unused)
    float kangle;    // 29      flag: additional angles set
    float phi1;      // 30      additional angles
    float theta1;    // 31
    float psi1;      // 32
    float phi2;      // 33
    float theta2;    // 34
    float psi2;      // 35

    double fGeo_matrix[3][3]; // x9 = 72 bytes: Geometric info
    float fAngle1; // angle info

    float fr1;
    float fr2; // lift up cosine mask parameters

    /** Fraga 23/05/97  For Radon transforms **/
    float RTflag; // 1=RT, 2=FFT(RT)
    float Astart;
    float Aend;
    float Ainc;
    float Rsigma; // 4*7 = 28 bytes
    float Tstart;
    float Tend;
    float Tinc; // 4*3 = 12, 12+28 = 40B

    /** Sjors Scheres 17/12/04 **/
    float weight; // For Maximum-Likelihood refinement
    float flip;   // 0=no flipping operation (false), 1=flipping (true)

    char fNada2[576]; // empty 700-76-40=624-40-8= 576 bytes

    char cdat[12];   // 211-213   creation date
    char ctim[8];  // 214-215   creation time
    char ctit[160];  // 216-255   title
}; // struct SpiderHeader


class ImageIOSpider: public em::ImageIO::Impl
{
private:
    SpiderHeader header;
    bool isStack;

public:

    virtual void readHeader() override
    {
        // Try to read the main header from the (already opened) file stream

        if ( fread(&header, SPIDER_HEADER_SIZE, 1, file) < 1 )
            THROW_SYS_ERROR(std::string("Error reading SPIDER header in file: ") + path);

        // Determine byte order and swap bytes if from different-endian machine
        if ( (swap = (( fabs(header.nslice) > SWAPTRIG ) ||
                      ( fabs(header.iform) > 1000 )     ||
                      ( fabs(header.nslice) < 1 ))) )
            //    swapPage((char *) header, SPIDERSIZE - 180, DT_Float); //
            // TODO: Determine swap order (little vs big endian)


        //"Invalid Spider file:  %s", filename.c_str()));
        if(header.labbyt != header.labrec*header.lenbyt)
            THROW_ERROR(std::string("Invalid SPIDER file: ") + path);

        // Check dimensions of the data taking into account
        // if it is a 2D or 3D stack
        isStack = ( header.istack > 0 );
        dim.x = header.nsam;
        dim.y = header.nrow;
        dim.z = header.nslice;
        dim.n = (isStack)? (size_t)(header.maxim) : 1;

        std::cout << "DEBUG: Spider Dimensions: " << dim << std::endl;

        type = em::TypeFloat;
        pad = (size_t) header.labbyt;

        // TODO: Store extra information from file header
    } // function readHeader

    virtual void writeHeader() override
    {
        size_t datasize = dim.getItemSize() * type->getSize();

        // Filling the main header
        float  lenbyt = type->getSize()*dim.x;  // Record length (in bytes)
        float  labrec = floor(SPIDER_HEADER_SIZE/lenbyt); // # header records
        if ( fmod(SPIDER_HEADER_SIZE,lenbyt) != 0 )
            labrec++;
        float  labbyt = labrec*lenbyt;   // Size of header in bytes
        size_t offset = (size_t) labbyt;

        // Map the parameters
        header.lenbyt = lenbyt;     // Record length (in bytes)
        header.labrec = labrec;     // # header records
        header.labbyt = labbyt;      // Size of header in bytes

        header.irec   = labrec + floor(datasize/lenbyt + 0.999999); // Total # records
        header.nsam   = dim.x;
        header.nrow   = dim.y;
        header.nslice = dim.z;

//    // If a transform, then the physical storage in x is only half+1
//    size_t xstore  = dim.x;
//    if ( transform == Hermitian )
//    {
//        xstore = (size_t)(dim.x * 0.5 + 1);
//        header.nsam = 2*xstore;
//    }
//
//
//    if ( dim.z < 2 )
//    {
//        // 2D image or 2D Fourier transform
//        header.iform = ( transform == NoTransform ) ? 1 :  -12 + (int)header.nsam%2;
//    }
//    else
//    {
//        // 3D volume or 3D Fourier transform
//        header.iform = ( transform == NoTransform )? 3 : -22 + (int)header.nsam%2;
//    }
//    double aux;
//    bool baux;
//    header.imami = 0;//never trust max/min
//
//
//    if (!MDMainHeader.empty())
//    {
//#define SET_MAIN_HEADER_VALUE(field, label, aux)  MDMainHeader.getValueOrDefault(label, aux, 0.); header.field = (float)aux
//        SET_MAIN_HEADER_VALUE(fmin, MDL_MIN, aux);
//        SET_MAIN_HEADER_VALUE(fmax, MDL_MAX, aux);
//        SET_MAIN_HEADER_VALUE(av, MDL_AVG, aux);
//        SET_MAIN_HEADER_VALUE(sig, MDL_STDDEV, aux);
//    }
//
//
//    if (Ndim == 1 && mode != WRITE_APPEND && !isStack && !MD.empty())
//    {
//        if ((dataMode == _HEADER_ALL || dataMode == _DATA_ALL))
//        {
//#define SET_HEADER_VALUE(field, label, aux)  MD[0].getValueOrDefault((label), (aux), 0.); header.field = (float)(aux)
//            SET_HEADER_VALUE(xoff, MDL_SHIFT_X, aux);
//            SET_HEADER_VALUE(yoff, MDL_SHIFT_Y, aux);
//            SET_HEADER_VALUE(zoff, MDL_SHIFT_Z, aux);
//            SET_HEADER_VALUE(phi, MDL_ANGLE_ROT, aux);
//            SET_HEADER_VALUE(theta, MDL_ANGLE_TILT, aux);
//            SET_HEADER_VALUE(gamma, MDL_ANGLE_PSI, aux);
//            SET_HEADER_VALUE(weight, MDL_WEIGHT, aux);
//            SET_HEADER_VALUE(flip, MDL_FLIP, baux);
//            SET_HEADER_VALUE(scale, MDL_SCALE, aux);
//        }
//        else
//        {
//            header.xoff = header.yoff = header.zoff =\
//                                          header.phi = header.theta = header.gamma = header.weight = 0.;
//            header.scale = 1.;
//        }
//    }


        if (dim.n > 1 )
        {
            header.istack = 2;
            header.inuse =  1;
            header.maxim = dim.n;
        }

        fwrite(&header, offset, 1, file);

    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
        return SPIDER_HEADER_SIZE;
    }

    virtual size_t getPadSize() const override
    {
        return dim.n > 1 ? SPIDER_HEADER_SIZE : 0;
    }

    virtual const TypeMap & getTypeMap() const override
    {
        static const TypeMap tm = {{0, TypeFloat}};
        return tm;
    } // function getTypeMap

}; // class ImageIOSpider


StringVector spiExts = {"spider", "spi", "stk", "vol"};

REGISTER_IMAGE_IO(spiExts, ImageIOSpider);