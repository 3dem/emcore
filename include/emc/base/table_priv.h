//
// Created by josem on 4/24/18.
//

#ifndef EM_CORE_METADATA_PRIV_H
#define EM_CORE_METADATA_PRIV_H

namespace emcore
{

    class TableFile::Impl
    {
    public:
        // Store the name of the file that was read/written
        std::string path;

        // Mode in which the file was open
        File::Mode fileMode = File::Mode::READ_ONLY;

        // Keep a file handler to the image file
        FILE* file = nullptr;

        virtual ~Impl() {};

    protected:
        virtual void openFile();
        virtual void closeFile();

        virtual StringVector getTableNames() const = 0;
        // TODO: Document
        virtual void read(const std::string &tableName, Table &table) = 0;
        // TODO: Document
        virtual void write(const std::string &tableName, const Table &table) = 0;

        friend class TableFile;
    }; // class TableFile::Impl

} // namespace emcore

namespace emc = emcore;

// The following macro can be used as a shortcut to register new ImageFile subclasses
#define REGISTER_TABLE_IO(extensions, ioClassName) \
    TableFile::Impl * new___##ioClassName(){ return new ioClassName(); } \
    bool reg___##ioClassName = emc::TableFile::registerImpl(extensions, new___##ioClassName)

#endif //EM_CORE_METADATA_PRIV_H
