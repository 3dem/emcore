//
// Created by josem on 10/3/17.
//

#ifndef EM_CORE_REGISTRY_H_H
#define EM_CORE_REGISTRY_H_H

#include <map>
#include "em/base/string.h"
#include "em/base/error.h"


namespace emcore
{
    // TODO: Document this class and its rationale

    /** Registry to specific implementation classes.
     */
    template <class T>
    class ImplRegistry
    {
    public:
        using BuilderFuncPtr = T* (*)();
        using BuilderMap = std::map<std::string, BuilderFuncPtr>;

        /**
         * Register an ImageFile class to be available for reading/writing images.
         * The class will be accessible via the ImageFile name and the extensions
         * defined by the class.
         * @param imgio Input pointer to the ImageFile subclass that will be
         * registred.
         * @return Return True if the new ImageFile was successfully registered.
         */
        bool registerImpl(const StringVector &extOrNames,
                          BuilderFuncPtr newImplBuilder)
        {
            for (auto &extOrName: extOrNames)
                registryMap[extOrName] = newImplBuilder;

            uniqueMap[extOrNames[0]] = newImplBuilder;
            return true;
        }

        /**
         * Check if some ImageFile is registered for a given name or extension.
         *
         * @param extOrName Input string representing either the ImageFile name
         * or one of the extensions registered.
         * @return Return True if there is any ImageFile registered.
         */
        bool hasImpl(const std::string &extOrName)
        {
            return registryMap.find(extOrName) != registryMap.end();
        }

        /**
         * Check if some ImageFile is registered for a given name or extension.
         *
         * @param extOrName Input string representing either the ImageFile name
         * or one of the extensions registered.
         * @return Return True if there is any ImageFile registered.
         */
        BuilderFuncPtr getImplBuilder(const std::string &extOrName)
        {
            if (!hasImpl(extOrName))
                return nullptr;

            return registryMap[extOrName];
        }

        /**
         * Return a pointer to a new instance of the Implementation
         * @param extOrName
         * @return
         */
        T* buildImpl(const std::string &extOrName)
        {
            auto implBuilder = getImplBuilder(extOrName);
            ASSERT_ERROR(implBuilder == nullptr,
                         std::string("Can not find implementation for ")
                         + extOrName);
            return implBuilder();
        }

        const BuilderMap& getMap() { return registryMap; }
        const BuilderMap& getUniqueMap() { return uniqueMap; }

    private:
         BuilderMap registryMap;
         BuilderMap  uniqueMap;  // only store once per implementation alias

    }; // class ImplRegistry

} // namespace emcore

#endif //EM_CORE_REGISTRY_H_H
