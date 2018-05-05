#include <iomanip>
#include <functional>

#include "em/base/error.h"
#include "em/image/image.h"
#include "em/image/image_priv.h"

using namespace em;


struct DmTag
{
    enum Class {DIR, SINGLE, GROUP, ARRAY, GROUP_ARRAY};

    size_t nodeId;
    const DmTag *parent;
    int tagType;
    short int dataType;
    Class tagClass;
    std::string tagName;

    size_t size;
    std::vector<Object> values;
    std::vector<DmTag*> childs;

    DmTag(size_t nodeId, const DmTag *parent): nodeId(nodeId), parent(parent) {}

    // Get the inner child iterating through the input tagName list
    const DmTag* getChild(std::initializer_list<std::string> list) const
    {
        auto child = this;

        for (auto& tagName: list)
        {
            child = child->getChild(tagName);
            if (child == nullptr)
                break;
        }
        return child;
    }

    // Return the first child which tagName is equal to the provided param
    const DmTag* getChild(const std::string& childTagName) const
    {
        for (auto child: childs)
            if (child->tagName == childTagName)
                return child;
        return nullptr;
    }

    void print(std::ostream &ostream, std::string space)
    {
        std::string tTypeStr = "DIR";

        if (tagClass == SINGLE)
            tTypeStr = "SINGLE";
        else if (tagClass == GROUP)
            tTypeStr = "GROUP";
        else if (tagClass == ARRAY)
            tTypeStr = "ARRAY";
        else if (tagClass == GROUP_ARRAY)
            tTypeStr = "GROUP_ARRAY";

        ostream << std::setw(6) << parent->nodeId
                << std::setw(6) << nodeId
                << std::setw(15) << tTypeStr
                << space << "˪-- " << tagName;

        if (tagClass == DIR)
        {
            ostream  << std::endl;

            auto childSpace = space + "    ";
            for (auto child: this->childs)
                child->print(ostream, childSpace);

        }
        else
        {
            ostream << " - Datatype: " << this->dataType;

            if (values.size() > 0)
                ostream << " -  Value: " << values[0] << std::endl;
        }
    }

    ~DmTag()
    {
        for (auto child: childs)
            delete child;
    }
}; //DmTag


struct DmHeader
{
    double      CalibrationOffsetX;  //CalibrationOffsetX
    double      pixelWidth;          //CalibrationDeltaX
    int         CalibrationElementX;    //CalibrationElementX
    double      CalibrationOffsetY;   //CalibrationOffsetY
    double      pixelHeight;           //CalibrationDeltaY
    int         CalibrationElementY;    //CalibrationElementY
    short int   dataType;     //DataType
    int         nx;            //ArraySizeX
    int         ny;           //ArraySizeY
    int         nIm;           //Number of images
    short int   dataTypeSize;
    size_t      headerSize;
    bool        flip;
}; //DmHeader


/**
 * Function pointer to be used to read size values that, depending of DM version
 * may be addressed by 4 or 8 bytes. Data will always be stored in a size_t type
 * @param data Pointer to data
 * @param count Number of data elements
 * @param file File handler
 * @param swap Boolean to either swap or not the data array
 * @return biven by fread
 */
std::function< size_t(size_t*, size_t, FILE*, bool) > freadSwapLong;

/**
 * Inherit properties from base ImageIOImpl and add information
 * specific for DM3/4 formats
 */
class ImageIODm: public em::ImageIO::Impl
{
public:
    // File information attributes
    int version;
    size_t rootlen;
    int byteOrder;
    char sorted;
    char open;
    size_t nTags;
    DmTag *rootTag;
    DmTag *dataTypeTag;

    int nIm;
    bool swap;
    bool isLE;
    DmHeader header;

    // Other variables used during the reading of tags
    size_t nodeCounter;

    // Read a single object from file
    void freadObject(Object &object)
    {
        ImageIO::fread(file, object.getData(), 1, object.getType().getSize(),
                       swap);
    }

    DmTag* readTag(const DmTag* parent)
    {
        /* Header Tag ============================================================== */

        // Generate an incremental and unique node id from nodeCounter
        DmTag* pTag = new DmTag(++nodeCounter, parent);
        auto &tag = *pTag; // shortcut name

        unsigned char cTag;
        unsigned short int ltName;

        fread(&cTag, 1, 1, file); // Identification tag: 20 = tag dir,  21 = tag
        ImageIO::fread(file, &ltName, 1, 2, isLE); // Length of the tag name

        tag.tagType = int(cTag);

        if (ltName > 0)
        {
            tag.tagName.resize(ltName);
            fread(&tag.tagName[0], ltName, 1, file); // Tag name
        }

        if (version == 4)
        {
            /*total bytes in tag/tag directory including all sub-directories
             * (new for DM4). Actually, we don't use it*/
            fseek(file, 8, SEEK_CUR);
        }

        /* Reading tags ======================================================*/
        if (tag.tagType == 20)  // Tag directory
        {
            tag.tagClass = DmTag::DIR;

            // We skip the following parameters, actually we don't use it.
            // 1 = sorted (normally = 1)
            //  0 = closed, 1 = open (normally = 0)
            fseek(file, 2, SEEK_CUR);

            //  number of tags in tag directory
            size_t tagSize;
            freadSwapLong(&tagSize, 1, file, isLE);
            tag.size = tagSize;

            for (size_t i = 0; i < tagSize; ++i)
                tag.childs.push_back(readTag(pTag));
        }
        else if (tag.tagType == 21)    // Tag
        {
            // We skip the %%%% symbols
            fseek(file, 4, SEEK_CUR);

            // Size of info array
            size_t ninfo;
            freadSwapLong(&ninfo, 1, file, isLE);
            // Reading of Info
            size_t info[ninfo];
            freadSwapLong(info, ninfo, file, isLE);

            /* Tag classification  ===========================================*/

            if (ninfo == 1)   // Single entry tag
            {
                tag.tagClass = DmTag::SINGLE;
                tag.dataType = info[0];
                tag.size = 1;
                // Store a single value of an Array, of a single element ;)
                tag.values.emplace_back(Object(getTypeFromMode(tag.dataType)));
                freadObject(tag.values[0]);

                // We store the special tag which we assume contains the image
                // information, i.e, the one that has 'Datatype' as name and
                // is not 23, which seems to be the thumbnail image
                if (tag.tagName == "DataType" && (int)tag.values[0] != 23)
                    dataTypeTag = pTag;
            }
            else if(ninfo == 3 && info[0]==20)   // Tag array
            {
                /*ninfo = 3
                info(0) = 20
                info(1) = number type for all values
                info(2) = info(ninfo) = size of array*/
                tag.tagClass = DmTag::ARRAY;
                tag.dataType = info[1];
                tag.size = info[2];

                // We store the image position in file to be read properly
                tag.values.emplace_back(Object(typeSizeT));
                size_t  cpos  = ftell(file);
                tag.values[0] = cpos;

                // We jump the image bytes
                fseek(file, tag.size*getTypeFromMode(tag.dataType).getSize(),
                      SEEK_CUR);
            }
            else if (info[0]==20 && info[1] == 15)    // Tag Group array
            {
                /*ninfo = size of array
                         info(0) = 20 (array)
                         info(1) = 15 (group)
                         info(2) = 0 (always 0)
                         info(3) = number of elements in group
                         info(2*i+  3) = number type for value i
                         info(ninfo) = size of info array*/
                tag.tagClass = DmTag::GROUP_ARRAY;
                size_t nGroups = info[3];
                tag.size = info[ninfo-1];

                // We do not store de group arrays. They are entangled and we
                // don't know how useful they are at this moment. We store only
                // the different datatypes in one array per element in the group
                for (size_t n = 0; n < nGroups; ++n)
                    tag.values.emplace_back(Object(getTypeFromMode(info[5+2*n])));

                size_t nBytes=0;
                for (size_t n = 0; n < nGroups; ++n)
                    nBytes += tag.values[n].getType().getSize();

                // Jump the array values
                fseek(file, tag.size*nBytes , SEEK_CUR);
            }
            else if (info[0] == 15)    // Tag Group  (struct)
            {
                /*ninfo = size of info array
                    info(0) = 15 (Group)
                    info(1) = length of groupname? (always = 0)
                    info(2) = ngroup, number of elements in group
                    info(2*i+1) = length of fieldname? (always = 0)
                    info(2*i+2) = tag data type for value i */
                tag.tagClass = DmTag::GROUP;
                tag.size = info[2];

                for (size_t n = 0; n < tag.size; ++n)
                {
                    tag.values.emplace_back(Object(getTypeFromMode(info[4+2*n])));
                    freadObject(tag.values[n]);
                }
            }
        }
        return pTag;
    }

    virtual void readHeader() override
    {
        // Check Machine endianness
        isLE = Type::isLittleEndian();

        ImageIO::fread(file, &version, 1, 4, isLE);

        /* Main difference between v3 and v4 is that lentype is 4 and8 bytes
         * so we select the proper function to store it in a int64_t type.
         * */
        if (version == 3)
        {
            freadSwapLong = static_cast<std::function<size_t(size_t *, size_t,
                                                             FILE *, bool)>>
            ([](size_t *data, size_t count, FILE *file, bool swap) -> size_t
                    {
                        int32_t tmp[count];
                        auto bytes = ImageIO::fread(file, &tmp, count, 4, swap);
                        for (size_t i = 0; i < count; ++i)
                            data[i] = (size_t) (tmp[i]);
                        return bytes;
                    });
        }
        else if (version == 4)
        {
            freadSwapLong = static_cast<std::function<size_t(size_t *, size_t,
                                                             FILE *, bool)>>
            ([](size_t *data, size_t count, FILE *file, bool swap) -> size_t
                    {
                        return ImageIO::fread(file, data, count, 8, swap);
                    });
        }
        else
            THROW_ERROR(std::string("ImageIODm::freadSwapLong: unsupported "
                                            "Digital micrograph version ") +
                        Object(version).toString());


        freadSwapLong(&rootlen, 1, file, isLE);
        ImageIO::fread(file, &byteOrder, 1, 4, isLE);

        // Set swap mode from endiannes and file byteorder
        swap = (isLE ^ byteOrder);

        fread(&sorted, 1, 1, file);
        fread(&open, 1, 1, file);
        freadSwapLong(&nTags, 1, file, isLE);

        nodeCounter = 0;
        rootTag = new DmTag(0, nullptr);


        for (size_t j = 0; j < nTags; ++j)
            rootTag->childs.push_back(readTag(rootTag));

        dim.n = 0;

        int value;
        auto imageData = dataTypeTag->parent;

        std::cout << "ImageData: " << imageData->tagName << std::endl;

        auto child = imageData->getChild("Data");
        header.headerSize = (size_t) child->values[0];
        header.dataType = child->dataType;

        child = imageData->getChild("Dimensions");
        header.nx = child->childs[0]->values[0];
        header.ny = child->childs[1]->values[0];
        header.nIm = (child->childs.size() > 2 ) ?
                       (int)child->childs[2]->values[0] : 1;

        child = imageData->parent->getChild({"ImageTags", "Acquisition",
                                             "Frame", "CCD","Pixel Size (um)"});
        if ( child != nullptr )
        {
            header.pixelHeight = (double)child->values[0]*1e4;
            header.pixelWidth = (double)child->values[1]*1e4;

        }
        else
            header.pixelHeight = header.pixelWidth = 0;

        // Setting the ImageIO header info
        dim.x = header.nx;
        dim.y = header.ny;
        dim.z = 1;
        dim.n = header.nIm;
        type = getTypeFromMode(header.dataType);
    } // function readHeader

    virtual void writeHeader() override
    {
        THROW_SYS_ERROR("ImageIODm::writeHeader: Writing in Digital Micrograph "
                                "format is not supported. If your life depends "
                                "on it, good luck!!");
    } // function writeHeader

    virtual size_t getHeaderSize() const override
    {
        return header.headerSize;
    } // function getHeaderSize

    virtual const IntTypeMap & getTypeMap() const override
    {
        static const IntTypeMap tm = {{2,  typeInt16},
                                   {3,  typeInt32},
                                   {4,  typeUInt16},
                                   {5,  typeUInt32},
                                   {6,  typeFloat},
                                   {7,  typeDouble},
                                   {8,  typeBool},
                                   {9,  typeInt8},
                                   {10, typeUInt8},
                                   {11, typeUInt64},
                                   {12, typeInt64}};
        return tm;
    } // function getTypeMap

    virtual void toStream(std::ostream &ostream, int verbosity) const override
    {
        ostream << "verbosity normal" << std::endl;

        if (verbosity > 1)
        {
            ostream << "--- DM File struct ---" << std::endl;

            for (auto child: rootTag->childs)
                child->print(ostream, "");
        }
    } // function toStream

    virtual ~ImageIODm()
    {
        delete rootTag;
    }

}; // class ImageIOMrc

StringVector dmExts = {"dm3", "dm4"};

REGISTER_IMAGE_IO(dmExts, ImageIODm);

