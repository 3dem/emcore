
#include "em/image/image_priv.h"
#include "em/image/image_spider_priv.h"


#define SPIDER_HEADER_SIZE 1024 // Minimum size of the SPIDER header (variable)


struct SPIDERhead
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
} ;


std::string ImageSpiderIO::getName() const
{
    return "spider";
}

StringVector ImageSpiderIO::getExtensions() const
{
    return {"spi", "stk", "vol"};
}

void ImageSpiderIO::read(const size_t index, em::Image &image)
{

}

ImageSpiderIO::~ImageSpiderIO()
{

}

void ImageSpiderIO::readHeader()
{
    // TODO: Implement it
}

size_t ImageSpiderIO::getHeaderSize() const
{
    return SPIDER_HEADER_SIZE;
}

size_t ImageSpiderIO::getPadSize() const
{
    return 0;
}

em::ImageIO* ImageSpiderIO::create() const
{
    return new ImageSpiderIO();
}
