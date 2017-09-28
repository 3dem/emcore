
#include "em/image/image_priv.h"
#include "em/image/image_spider_priv.h"
#include <math.h>

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
} ;

/**
 * Inherit properties from base ImageHandler and add information
 * specific for Spider format (e.g, the SpiderHeader struct)
 */
class ImageSpiderHandler: public ImageHandler
{
public:
    SpiderHeader header;
};

std::string ImageSpiderIO::getName() const
{
    return "spider";
}

StringVector ImageSpiderIO::getExtensions() const
{
    return {"spi", "stk", "vol"};
}

ImageSpiderIO::~ImageSpiderIO()
{

}

void ImageSpiderIO::readHeader()
{
    // TODO: Implement it

    auto spiHandler = static_cast<ImageSpiderHandler*>(handler);

    // Try to read the main header from the (already opened) file stream

    if ( fread(&spiHandler->header, SPIDER_HEADER_SIZE, 1, spiHandler->file) < 1 )
        return; //FIXME: Set some error or throw an Exception errCode = -2;

    SpiderHeader &header = spiHandler->header;

    // Determine byte order and swap bytes if from different-endian machine
    if ( (spiHandler->swap = (( fabs(header.nslice) > SWAPTRIG ) ||
                              ( fabs(header.iform) > 1000 )     ||
                              ( fabs(header.nslice) < 1 ))) )
    //    swapPage((char *) header, SPIDERSIZE - 180, DT_Float); //
    // TODO: Determine swap order (little vs big endian)


    //"Invalid Spider file:  %s", filename.c_str()));
    if(header.labbyt != header.labrec*header.lenbyt)
        return; //FIXME: Set some error or throw an Exception errCode = -2;


    // Check dimensions of the data taking into account
    // if it is a 2D or 3D stack
    bool isStack = ( header.istack > 0 );
    ArrayDim &adim = spiHandler->dim; // short-hand alias
    adim.x = header.nsam;
    adim.y = header.nrow;
    adim.z = header.nslice;
    adim.n = (isStack)? (size_t)(header.maxim) : 1;

    std::cout << "DEBUG: Spider Dimensions: " << adim << std::endl;

    spiHandler->type = em::TypeFloat;
    spiHandler->pad = (size_t) header.labbyt;

    // TODO: Store extra information from file header
//    MDMainHeader.setValue(MDL_MIN,(double)header.fmin);
//    MDMainHeader.setValue(MDL_MAX,(double)header.fmax);
//    MDMainHeader.setValue(MDL_AVG,(double)header.av);
//    MDMainHeader.setValue(MDL_STDDEV,(double)header.sig);
//    MDMainHeader.setValue(MDL_SAMPLINGRATE_X,(double)header.scale);
//    MDMainHeader.setValue(MDL_SAMPLINGRATE_Y,(double)header.scale);
//    MDMainHeader.setValue(MDL_SAMPLINGRATE_Z,(double)header.scale);


//    if (_xDim < 1 || _yDim < 1 || _zDim < 1 || _nDim < 1)
//        REPORT_ERROR(ERR_IO_NOTFILE,formatString("Invalid Spider file:  %s", filename.c_str()));
//
//    replaceNsize = _nDim;
//
//    /************
//     * BELLOW HERE DO NOT USE HEADER BUT LOCAL VARIABLES
//     */
//
//    // Map the parameters, REad the whole object (-1) or a slide
//    // Only handle stacks of images not of volumes
//    if(!isStack)
//        _nDimSet = 1;
//    else
//        _nDimSet = (select_img == ALL_IMAGES) ? _nDim : 1;
//
//    setDimensions(_xDim, _yDim, _zDim, _nDimSet);
//
//    //image is in stack? and set right initial and final image
//    size_t header_size = offset;
//
//    if ( isStack)
//    {
//        if ( select_img > _nDim )
//            REPORT_ERROR(ERR_INDEX_OUTOFBOUNDS, formatString("readSpider (%s): Image number %lu exceeds stack size %lu" ,filename.c_str(),select_img, _nDim));
//        offset += offset;
//    }
//
//    if (dataMode == HEADER || (dataMode == _HEADER_ALL && _nDimSet > 1)) // Stop reading if not necessary
//    {
//        delete header;
//        return 0;
//    }
//
//    size_t datasize_n  = _xDim*_yDim*_zDim;
//    size_t image_size  = header_size + datasize_n*sizeof(float);
//    size_t pad         = (size_t) header.labbyt;
//    size_t   imgStart = IMG_INDEX(select_img);
//    size_t   imgEnd = (select_img != ALL_IMAGES) ? imgStart + 1 : _nDim;
//    size_t   img_seek = header_size + imgStart * image_size;
//
//    MD.clear();
//    MD.resize(imgEnd - imgStart,MDL::emptyHeader);
//    double daux;
//
//
//    for (size_t n = 0, i = imgStart; i < imgEnd; ++i, ++n, img_seek += image_size )
//    {
//        if (fseek( fimg, img_seek, SEEK_SET ) != 0)//fseek return 0 on success
//            REPORT_ERROR(ERR_IO, formatString("rwSPIDER: error seeking %lu for read image %lu", img_seek, i));
//
//        // std::cerr << formatString("DEBUG_JM: rwSPIDER: seeking %lu for read image %lu", img_seek, i) <<std::endl;
//
//        if(isStack)
//        {
//            if ( fread( header, SPIDERSIZE, 1, fimg ) != 1 )
//                REPORT_ERROR(ERR_IO_NOREAD, formatString("rwSPIDER: cannot read Spider image %lu header", i));
//            if ( swap )
//                swapPage((char *) header, SPIDERSIZE - 180, DT_Float);
//        }
//        if (dataMode == _HEADER_ALL || dataMode == _DATA_ALL)
//        {
//            daux = (double)header.xoff;
//            MD[n].setValue(MDL_SHIFT_X, daux);
//            daux = (double)header.yoff;
//            MD[n].setValue(MDL_SHIFT_Y, daux);
//            daux = (double)header.zoff;
//            MD[n].setValue(MDL_SHIFT_Z, daux);
//            daux = (double)header.phi;
//            MD[n].setValue(MDL_ANGLE_ROT, daux);
//            daux = (double)header.theta;
//            MD[n].setValue(MDL_ANGLE_TILT, daux);
//            daux = (double)header.gamma;
//            MD[n].setValue(MDL_ANGLE_PSI, daux);
//            daux = (double)header.weight;
//            MD[n].setValue(MDL_WEIGHT, daux);
//            bool baux = (header.flip == 1);
//            MD[n].setValue(MDL_FLIP, baux);
//            daux = (double) header.scale;
//            if (daux==0.)
//                daux=1.0;
//            MD[n].setValue(MDL_SCALE, daux);
//        }
//    }
//

}

size_t ImageSpiderIO::getHeaderSize() const
{
    return SPIDER_HEADER_SIZE;
}

em::ImageIO* ImageSpiderIO::create() const
{
    return new ImageSpiderIO();
}

ImageHandler* ImageSpiderIO::createHandler()
{
    return new ImageSpiderHandler;
} // createHandler


REGISTER_IMAGE_IO(ImageSpiderIO);
