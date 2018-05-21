//
// Created by josem on 4/24/18.
//

#ifndef EM_CORE_METADATA_PRIV_H
#define EM_CORE_METADATA_PRIV_H

namespace em
{

    class TableIO::Impl
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
        virtual void read(const std::string &tableName, Table &table) = 0;


        friend class TableIO;
    }; // class TableIO::Impl

} // namespace em


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_TABLE_IO(extensions, ioClassName) \
    TableIO::Impl * new___##ioClassName(){ return new ioClassName(); } \
    bool reg___##ioClassName = em::TableIO::registerImpl(extensions, new___##ioClassName)

#endif //EM_CORE_METADATA_PRIV_H
