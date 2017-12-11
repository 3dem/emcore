#include <functional>
#include "em/base/error.h"
#include "em/image/image.h"
#include "em/image/image_priv.h"

using namespace em;


struct DmTag
{
    size_t nodeId;
    size_t parentId;
    int tagType;
    std::string tagName;
    std::string tagClass;
    uint64_t size;
    ConstTypePtr type = nullptr;
    Array values;

    DmTag(){};
    DmTag(size_t nodeId, size_t parentId, int tagType,
          const std::string tagName) : nodeId(nodeId), parentId(parentId),
                                        tagType(tagType), tagName(tagName) {}
}; //DmTag

struct DmFileInfo
{
    int version;
    uint64_t rootlen;
    int byteOrder;
    char sorted;
    char open;
    uint64_t nTags;
    std::vector<DmTag> tags;
    int nIm;
    bool isLE;
    bool swap;
}; //DmFileInfo


struct DmHeader
{
    double      CalibrationOffsetX;  //CalibrationOffsetX
    double      pixelWidth;          //CalibrationDeltaX
    int         CalibrationElementX;    //CalibrationElementX
    double      CalibrationOffsetY;   //CalibrationOffsetY
    double      pixelHeight;           //CalibrationDeltaY
    int         CalibrationElementY;    //CalibrationElementY
    short int   dataType;     //DataType
    int         imageWidth;            //ArraySizeX
    int         imageHeight;           //ArraySizeY
    short int   dataTypeSize;
    size_t      headerSize;
    bool        flip;
}; //DmHeader


/**
 * Function pointer to be used to read size values that, depending of DM version
 * may be addressed by 4 or 8 bytes. Data will always be stored in a uint64_t type
 * @param data Pointer to data
 * @param count Number of data elements
 * @param file File handler
 * @param swap Boolean to either swap or not the data array
 * @return biven by fread
 */
std::function< size_t(uint64_t*, size_t, FILE*, bool) > freadSwapLong;
//size_t (*freadSwapLong)(uint64_t*, size_t, FILE*, bool);

/**
 * Inherit properties from base ImageIOImpl and add information
 * specific for DM3/4 formats
 */
class ImageIODm: public em::ImageIOImpl
{
public:

    DmFileInfo fileInfo;
    DmHeader header;
    bool isLE;


    double readTag(size_t parentId, size_t &nodeId)
    {
        /* Header Tag ============================================================== */

//        fileInfo.tags.emplace_back(nodeId, parentId, tagType, stagName);
        fileInfo.tags.emplace_back();
//        fileInfo.tags.push_back(DmTag());
        DmTag &tag = fileInfo.tags.back();

        ++nodeId;
        tag.nodeId = nodeId;
        tag.parentId = parentId;


        unsigned char cTag;
        unsigned short int ltName;

        fread(&cTag, 1, 1, file); // Identification tag: 20 = tag dir,  21 = tag
        freadSwap(&ltName, 1, 2, file, isLE); // Length of the tag name

        int tagType = int(cTag);
        tag.tagType = tagType;

        if (ltName > 0)
        {
            char * tagName =  new char[ltName+1];
            fread(tagName, ltName, 1, file); // Tag name
            tagName[ltName] = '\0';
            tag.tagName = tagName;
            delete [] tagName;
        }

        if (fileInfo.version == 4)
        {
            /*total bytes in tag/tag directory including all sub-directories
             * (new for DM4). Actually, we don't use it*/
            fseek(file, 8, SEEK_CUR);
        }




        /* Reading tags ======================================================*/
        if (tagType == 20)  // Tag directory
        {
            tag.tagClass = "Dir";

            // We skip the following parameters, actually we don't use it.
            // 1 = sorted (normally = 1)
            //  0 = closed, 1 = open (normally = 0)
            fseek(file, 2, SEEK_CUR);

            //  number of tags in tag directory
            freadSwapLong(&tag.size, 1, file, isLE);

            parentId = nodeId;
            for (size_t i = 0; i < tag.size; ++i)
                readTag(parentId, nodeId);

            return 0;
        }
        else if (tagType == 21)    // Tag
        {
            // We skip the %%%% symbols
            fseek(file, 4, SEEK_CUR);

            // Size of info array
            uint64_t ninfo;
            freadSwapLong(&ninfo, 1, file, isLE);
            // Reading of Info
            uint64_t info[ninfo];
            freadSwapLong(info, ninfo, file, isLE);

            /* Tag classification  ===========================================*/

            if (ninfo == 1)   // Single entry tag
            {
                tag.tagClass = "Single";
                tag.type = getTypeFromMode(info[0]);
                tag.size = 0;

                tag.values.resize(ArrayDim(1), tag.type);

                freadArray(tag.values, file, swap);
            }
            else if(ninfo == 3 && info[0]==20)   // Tag array
            {
                /*ninfo = 3
                info(0) = 20
                info(1) = number type for all values
                info(2) = info(ninfo) = size of array*/

                tag.tagClass = "Array";
                tag.type = getTypeFromMode(info[1]);
                tag.size = info[2];

                tag.values.resize(ArrayDim(1), TypeUInt64);

                // We store the image position in file to be read properly
                size_t pos = ftell(file);
                tag.values.copyFrom(&pos, TypeUInt64);

                // We jump the image bytes
                fseek(file, tag.size*tag.type->getSize(), SEEK_CUR);
            }
            else if (info[0]==20 && info[1] == 15)    // Tag Group array
            {
                /*ninfo = size of array
                         info(0) = 20 (array)
                         info(1) = 15 (group)
                         info(2) = 0 (always 0)
                         info(3) = number of values in group
                         info(2*i+  3) = number type for value i
                         info(ninfo) = size of info array*/
                tag.tagClass = "GroupArray";
                tag.size = info[3];

                //For future use we store info array
                tag.values.resize(ArrayDim(tag.size), TypeUInt64);
                tag.values.copyFrom(&info, TypeUInt64);

                size_t nBytes=0;
                for (size_t n = 0; n < tag.size; ++n)
                    nBytes += getTypeFromMode(info[3+2*n])->getSize();

                // Jump the array values
                fseek(file, info[ninfo-1]*nBytes , SEEK_CUR);
            }
            else if (info[0] == 15)    // Tag Group  (struct)
            {
                /*ninfo = size of info array
                    info(0) = 15 (Group)
                    info(1) = length of groupname? (always = 0)
                    info(2) = ngroup, number of elements in group
                    info(2*i+1) = length of fieldname? (always = 0)
                    info(2*i+2) = tag data type for value i */

                tag.tagClass = "Group";
                tag.size = info[2];

                double dValues[tag.size];
                Array aValue;
                ArrayDim adim(1);

                for (size_t n = 0; n < tag.size; ++n)
                {
                    aValue.resize(adim, getTypeFromMode(info[4+2*n]));
                    freadArray(aValue, file, swap);
                    aValue.copyTo(&(dValues[n]), TypeDouble);
                }

                tag.values.resize(ArrayDim(tag.size), TypeDouble);
                tag.values.copyFrom(dValues, TypeDouble);
            }
        }
        return 0;
    }


    virtual void readHeader() override
    {

        int dummy;

        // Check Machine endianness
        isLE = isLittleEndian();

        freadSwap(&fileInfo.version, 1, 4, file, isLE);

        /* Main difference between v3 and v4 is that lentype is 4 and8 bytes
         * so we select the proper function to store it in a int64_t type.
         * */
        if (fileInfo.version == 3)
        {
            freadSwapLong = static_cast<std::function<size_t(uint64_t *, size_t,
                                                             FILE *, bool)>>
            ([](uint64_t *data, size_t count, FILE *file, bool swap) -> size_t
                    {
                        int32_t tmp[count];
                        freadSwap(&tmp, count, 4, file, swap);
                        for (size_t i = 0; i < count; ++i)
                            data[i] = (uint64_t) (tmp[i]);
                    });
        }
        else if (fileInfo.version == 4)
        {
            freadSwapLong = static_cast<std::function<size_t(uint64_t *, size_t,
                                                             FILE *, bool)>>
            ([](uint64_t *data, size_t count, FILE *file, bool swap) -> size_t
                    {return freadSwap(data, count, 8, file, swap);});
        }
        else
            THROW_ERROR("ImageIODm::freadSwapLong: unsupported Digital "
                     "micrograph version " + fileInfo.version);


        freadSwapLong(&fileInfo.rootlen, 1, file, isLE);
        freadSwap(&fileInfo.byteOrder, 1, 4, file, isLE);

        // Set swap mode from endiannes and file byteorder
        swap = (isLE^fileInfo.byteOrder);

        fread(&fileInfo.sorted, 1, 1, file);
        fread(&fileInfo.open, 1, 1, file);
        freadSwapLong(&fileInfo.nTags, 1, file, isLE);

        size_t nodeID = 0, parentID = 0;

        for (size_t j = 0; j < fileInfo.nTags ; ++j)
            readTag(parentID, nodeID);



    } // function readHeader

    virtual void writeHeader() override
    {
        THROW_SYS_ERROR("ImageIODm::writeHeader: Writing in Digital Micrograph "
                                "format is not supported. If your life depends "
                                "on it, good luck!!");

    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
      //  return MRC_HEADER_SIZE;
    } // function getHeaderSize

    virtual const TypeMap & getTypeMap() const override
    {
        static const TypeMap tm = {{2,  TypeInt16},
                                   {3,  TypeInt32},
                                   {4,  TypeUInt16},
                                   {5,  TypeUInt32},
                                   {6,  TypeFloat},
                                   {7,  TypeDouble},
                                   {8,  TypeBool},
                                   {9,  TypeInt8},
                                   {10, TypeUInt8},
                                   {11, TypeUInt64}};

        return tm;
    } // function getTypeMap


/*    size_t freadSwapLong(uint64_t *data, size_t count, FILE *file,
                         bool swap) const
    {
        switch (fileInfo.version)
        {
            case 3:
            {
                int32_t tmp[count];
                freadSwap(&tmp, count, 4, file, swap);
                for (size_t i = 0; i < count; ++i)
                    data[i] = (uint64_t) (tmp[i]);

                break;
            }
            case 4:
            {
                freadSwap(data, count, 8, file, swap);
                break;
            }
            default:
                THROW_ERROR("ImageIODm::freadSwapLong: unsupported Digital micrograph");
        }

    }*/


}; // class ImageIOMrc

StringVector dmExts = {"dm3", "dm4"};

REGISTER_IMAGE_IO(dmExts, ImageIODm);

