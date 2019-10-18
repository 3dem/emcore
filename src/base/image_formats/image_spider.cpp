
#include <math.h>
#include "emc/base/error.h"
#include "emc/base/image.h"
#include "emc/base/image_priv.h"

using namespace emcore;


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


class SpiderImageFile: public ImageFile::Impl
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

        // TODO: Determine swap order (little vs big endian)
        // Determine byte order and swap bytes if from different-endian machine
//        if ( (swap = (( fabs(header.nslice) > SWAPTRIG ) ||
//                      ( fabs(header.iform) > 1000 )     ||
//                      ( fabs(header.nslice) < 1 ))) )
        //    swapPage((char *) header, SPIDERSIZE - 180, DT_Float); //


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

        type = typeFloat;
        pad = (size_t) header.labbyt;

        // TODO: Store extra information from file header
    } // function readHeader

    virtual void writeHeader() override
    {
        size_t datasize = dim.getItemSize() * type.getSize();

        // Filling the main header
        float  lenbyt = type.getSize() * dim.x;  // Record length (in bytes)
        float  labrec = floor(SPIDER_HEADER_SIZE / lenbyt); // # header records
        if ( fmod(SPIDER_HEADER_SIZE,lenbyt) != 0 )
            labrec++;
        float  labbyt = labrec * lenbyt;  // Size of header in bytes
        pad = (size_t) labbyt;

        // Map the parameters
        header.lenbyt = lenbyt;     // Record length (in bytes)
        header.labrec = labrec;     // # header records
        header.labbyt = labbyt;      // Size of header in bytes

        header.irec   = labrec + floor(datasize/lenbyt + 0.999999); // Total # records
        header.nsam   = dim.x;
        header.nrow   = dim.y;
        header.nslice = dim.z;
        isStack = dim.n > 1;

        header.imami = 0; //never trust max/min

        if (isStack)
        {
            header.istack = 2;
            header.inuse =  1;
            header.maxim = dim.n;
        }

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

        fwrite(&header, pad, 1, file);

    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
        //return SPIDER_HEADER_SIZE;
        return pad;
    }

    virtual size_t getPadSize() const override
    {
        return dim.n > 1 ? pad : 0;
    }

    virtual const IntTypeMap & getTypeMap() const override
    {
        static const IntTypeMap tm = {{0, typeFloat}};
        return tm;
    } // function getTypeMap

    virtual void toStream(std::ostream &ostream, int verbosity) const override
    {
        if (verbosity > 1)
        {
            ostream << "--- SPIDER File Header ---" << std::endl;
            std::cout << std::setw(7) << "istack: " << header.istack << std::endl;
            std::cout << std::setw(7) << "nsam (x): " << header.nsam << std::endl;
            std::cout << std::setw(7) << "nrow (y): " << header.nrow << std::endl;
            std::cout << std::setw(7) << "maxim (n): " << header.maxim << std::endl;
            std::cout << std::setw(7) << "labrec (header records): " << header.labrec << std::endl;
            std::cout << std::setw(7) << "lenbyt (record bytes): " << header.lenbyt << std::endl;
            std::cout << std::setw(7) << "labbyt (header bytes): " << header.labbyt << std::endl;
        }
    } // function toStream

    // Re-implmente this function to update the header record maxim
    // when the file is expanded with more images
    virtual void expand() override
    {
        if (isStack)
        {
            // Now update the number of images in a set and the header
            header.maxim = dim.n;

            if (fseek(file, 0, SEEK_SET) != 0)
                THROW_SYS_ERROR(std::string("Could not 'fseek' in file: ") + path);

            // Write header again
            fwrite(&header, pad, 1, file);
        }
        ImageFile::Impl::expand();
    } // function expand

    // Overwrite this function to validate that index > 1 can
    // only be written when the opened file is an stack
    virtual void writeImageData(const size_t index, const Image &image) override 
    {
        ASSERT_ERROR(index > 1 && !isStack,
                     "More than one image can only be written in stack files.");
        ImageFile::Impl::writeImageData(index, image);
    } // function writeImageData

}; // class SpiderImageFile


StringVector spiExts = {"spider", "spi", "stk", "vol"};

REGISTER_IMAGE_IO(spiExts, SpiderImageFile);