
#include "em/base/error.h"
#include "em/image/image_priv.h"
#include "em/image/image_mrc_priv.h"



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
 * Inherit properties from base ImageHandler and add information
 * specific for MRC format (e.g, the MrcHeader struct)
 */
class ImageMrcHandler: public em::ImageHandler
{
public:
    MrcHeader header;
};


std::string ImageMrcIO::getName() const
{
    return "mrc";
}

StringVector ImageMrcIO::getExtensions() const
{
    return {"mrc", "mrcs"};
}

void ImageMrcIO::readHeader()
{
    auto mrcHandler = static_cast<ImageMrcHandler*>(handler);

    std::cout << "DEBUG: Reading header...file " << mrcHandler->path << std::endl;

    // Try to read the main header from the (already opened) file stream
    if ( fread(&mrcHandler->header, MRC_HEADER_SIZE, 1, mrcHandler->file) < 1 )
        return; //FIXME: Set some error or throw an Exception errCode = -2;

    MrcHeader &header = mrcHandler->header;

    bool isImgStack = (header.ispg == 0 and header.nx > 1);
    bool isVolStack = (header.ispg == 401);


    // Check dimensions of the data taking into account
    // if it is a 2D or 3D stack
    ArrayDim &adim = mrcHandler->dim; // short-hand alias
    adim.x = header.nx;
    adim.y = header.ny;

    if (isImgStack)
    {
        adim.z = 1;
        adim.n = header.nz;
    }
    else if (isVolStack)
    {
        adim.z = header.mz;
        adim.n = header.nz / header.mz;
    }

    std::cout << "DEBUG: Dimensions: " << adim << std::endl;


    // Check Datatype
    switch (header.mode)
    {
        case 0:
            mrcHandler->type = em::TypeInt8;
            break;
        case 1:
            mrcHandler->type = em::TypeInt16;
            break;
        case 2:
            mrcHandler->type = em::TypeFloat;
            break;
        case 3:
            // FIXME: datatype = DT_CShort;
            mrcHandler->type = nullptr;
            break;
        case 4:
            // FIXME: datatype = DT_CFloat;
            mrcHandler->type = nullptr;
            break;
        case 6:
            mrcHandler->type = em::TypeUInt16;
            break;
        default:
            mrcHandler->type = nullptr;
            // FIXME: errCode = -1;
            break;
    }

    std::cout << "DEBUG: Data Type: " << *mrcHandler->type << std::endl;

    // TODO: Check special cases where image is a transform
    // TODO: Determine swap order (little vs big endian)

    mrcHandler->pad = 0;
}

/** Returns true if machine is little endian else false */
bool isLittleEndian(void)
{
    static const unsigned long ul = 0x00000001;
    return ((int)(*((unsigned char *) &ul)))!=0;
}

void ImageMrcIO::writeHeader()
{
    auto mrcHandler = static_cast<ImageMrcHandler*>(handler);
    auto& header = mrcHandler->header;
    auto& dim = mrcHandler->dim;

    ConstTypePtr type = mrcHandler->type;

    if (type == em::TypeDouble || type == em::TypeFloat || type == em::TypeInt32 || em::TypeUInt32)
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

    fwrite(&header, MRC_HEADER_SIZE, 1, mrcHandler->file);

    // FIXME: consider swap
    // if ( swapWrite )
    //   swapPage((char *) header, MRCSIZE - 800, DT_Float);

} // function writeHeader

size_t ImageMrcIO::getHeaderSize() const
{
    return MRC_HEADER_SIZE;
}

ImageHandler* ImageMrcIO::createHandler()
{
    return new ImageMrcHandler;
} // createHandler

ImageMrcIO::~ImageMrcIO()
{

}

em::ImageIO* ImageMrcIO::create() const
{
    return new ImageMrcIO();
}

REGISTER_IMAGE_IO(ImageMrcIO);

#ifdef NEVER_DEFINED

int ImageBase::readMRC(size_t select_img, bool isStack)
{
#undef DEBUG
    //#define DEBUG
#ifdef DEBUG
    printf("DEBUG readMRC: Reading MRC file\n");
#endif

    MRChead* header = new MRChead;

    int errCode = 0;

    if ( fread( header, MRCSIZE, 1, fimg ) < 1 )
        return(-2);

    // Determine byte order and swap bytes if from little-endian machine
    if ( (swap = (( abs( header->mode ) > SWAPTRIG ) || ( abs(header->nz) > SWAPTRIG ))) )
    {
#ifdef DEBUG
        fprintf(stderr, "Warning: Swapping header byte order for 4-byte types\n");
#endif

        swapPage((char *) header, MRCSIZE - 800, DT_Float); // MRCSIZE - 800 is to exclude labels from swapping
    }

    // Convert VAX floating point types if necessary
    // if ( header->amin > header->amax )
    //    REPORT_ERROR(ERR_IMG_NOREAD,"readMRC: amin > max: VAX floating point conversion unsupported");

    size_t _xDim,_yDim,_zDim,_nDim;

    _xDim = header->nx;
    _yDim = header->ny;
    _zDim = header->nz;

    bool isVolStk = (header->ispg > 400);

    /* isStack is already true if file uses our customized "mrcs" extension. In this case
     * we ignore the stack behavior in header. If format is forced through ":" flag suffix,
     * then we also ignore the stack behavior in header */
    if ( !isStack && (isVolStk || !filename.contains(":")))
        isStack = ((header->ispg == 0 || isVolStk ) && (header->nsymbt == 0));

    if(isStack)
    {
        if (isVolStk)
        {
            _nDim = _zDim / header->mz;
            _zDim = header->mz;
        }
        else
        {
            _nDim = _zDim;// When isStack slices in Z are supposed to be a stack of images
            _zDim = 1;
        }

        if ( select_img > _nDim )
            REPORT_ERROR(ERR_INDEX_OUTOFBOUNDS, formatString("readMRC: %s Image number %lu exceeds stack size %lu", this->filename.c_str(), select_img, _nDim));

        if (select_img > ALL_IMAGES)
            _nDim = 1;
    }
    else // If the reading is not like a stack, then the select_img is not taken into account and must be selected the only image
    {
        select_img = ALL_IMAGES;
        _nDim = 1;
    }

    replaceNsize = _nDim;
    setDimensions(_xDim, _yDim, _zDim, _nDim);

    DataType datatype;
    switch ( header->mode )
    {
        case 0:
            datatype = DT_UChar;
            break;
        case 1:
            datatype = DT_Short;
            break;
        case 2:
            datatype = DT_Float;
            break;
        case 3:
            datatype = DT_CShort;
            break;
        case 4:
            datatype = DT_CFloat;
            break;
        case 6:
            datatype = DT_UShort;
            break;
        default:
            datatype = DT_Unknown;
            errCode = -1;
            break;
    }
    offset = MRCSIZE + header->nsymbt;
    size_t datasize_n;
    datasize_n = _xDim*_yDim*_zDim;

    if ( header->mode > 2 && header->mode < 5 )
    {
        transform = CentHerm;
        fseek(fimg, 0, SEEK_END);
        if ( ftell(fimg) > offset + 0.8*datasize_n*gettypesize(datatype) )
            _xDim = (2 * (_xDim - 1));
        if ( header->mx%2 == 1 )
            _xDim += 1;     // Quick fix for odd x-size maps
        setDimensions(_xDim, _yDim, _zDim, _nDim);
    }

    MDMainHeader.setValue(MDL_MIN,(double)header->amin);
    MDMainHeader.setValue(MDL_MAX,(double)header->amax);
    MDMainHeader.setValue(MDL_AVG,(double)header->amean);
    MDMainHeader.setValue(MDL_STDDEV,(double)header->arms);
    MDMainHeader.setValue(MDL_DATATYPE,(int)datatype);

    if ( header->mx && header->a!=0)//ux
        MDMainHeader.setValue(MDL_SAMPLINGRATE_X,(double)header->a/header->mx);
    if ( header->my && header->b!=0)//yx
        MDMainHeader.setValue(MDL_SAMPLINGRATE_Y,(double)header->b/header->my);
    if ( header->mz && header->c!=0)//zx
        MDMainHeader.setValue(MDL_SAMPLINGRATE_Z,(double)header->c/header->mz);

    if (dataMode==HEADER || (dataMode == _HEADER_ALL && _nDim > 1)) // Stop reading if not necessary
    {
        delete header;
        return errCode;
    }

    size_t   imgStart = IMG_INDEX(select_img);
    size_t   imgEnd = (select_img != ALL_IMAGES) ? imgStart + 1 : _nDim;

    MD.clear();
    MD.resize(imgEnd - imgStart,MDL::emptyHeader);

    /* As MRC does not support stacks, we use the geometry stored in the header
    for any image when we simulate the file is a stack.*/
    if (dataMode == _HEADER_ALL || dataMode == _DATA_ALL)
    {
        double aux;
        for ( size_t i = 0; i < imgEnd - imgStart; ++i )
        {
            MD[i].setValue(MDL_SHIFT_X, (double) -header->nxStart);
            MD[i].setValue(MDL_SHIFT_Y, (double) -header->nyStart);
            MD[i].setValue(MDL_SHIFT_Z, (double) -header->nzStart);

            // We include auto detection of MRC2000 or CCP4 style origin based on http://situs.biomachina.org/fmap.pdf
            if (header->xOrigin != 0)
                MD[i].setValue(MDL_ORIGIN_X, -header->xOrigin);
            else if (header->nxStart != 0 && MDMainHeader.getValue(MDL_SAMPLINGRATE_X,aux))
                MD[i].setValue(MDL_ORIGIN_X, -header->nxStart/aux);

            if (header->yOrigin !=0)
                MD[i].setValue(MDL_ORIGIN_Y, -header->yOrigin);
            else if(header->nyStart !=0 && MDMainHeader.getValue(MDL_SAMPLINGRATE_Y,aux))
                MD[i].setValue(MDL_ORIGIN_Y, -header->nyStart/aux);

            if (header->zOrigin != 0)
                MD[i].setValue(MDL_ORIGIN_Z, -header->zOrigin);
            else if(header->nzStart !=0 && MDMainHeader.getValue(MDL_SAMPLINGRATE_Z,aux))
                MD[i].setValue(MDL_ORIGIN_Z, -header->nzStart/aux);
        }
    }
    //#define DEBUG
#ifdef DEBUG
    MDMainHeader.write("/dev/stderr");
    MD.write("/dev/stderr");
#endif

    delete header;

    if ( dataMode < DATA )   // Don't read the individual header and the data if not necessary
        return errCode;

    readData(fimg, select_img, datatype, 0);

    return errCode;
}

#endif
