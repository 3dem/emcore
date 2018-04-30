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
        std::string path;

        virtual void open(const std::string &path) = 0;
        virtual void close() = 0;
        virtual void read(const std::string &tableName, Table &table) = 0;

        virtual ~Impl() {};
    }; // class TableIO::Impl

} // namespace em


// The following macro can be used as a shortcut to register new ImageIO subclasses
#define REGISTER_TABLE_IO(extensions, ioClassName) \
    TableIO::Impl * new___##ioClassName(){ return new ioClassName(); } \
    bool reg___##ioClassName = em::TableIO::registerImpl(extensions, new___##ioClassName)

#endif //EM_CORE_METADATA_PRIV_H
