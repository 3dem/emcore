//
// Created by josem on 10/3/17.
//

#ifndef EM_CORE_REGISTRY_H_H
#define EM_CORE_REGISTRY_H_H

#include <map>


namespace em
{
    // TODO: DOcument this class and its rationale

    /** Registry to specific implemenation classes.
     */
    template <class T>
    class ImplRegistry
    {
    public:
        using BuilderFuncPtr = T* (*)();
        /**
         * Register a ImageIO class to be available for reading/writing images.
         * The class will be accessible via the ImageIO name and the extensions
         * defined by the class.
         * @param imgio Input pointer to the ImageIO subclass that will be
         * registred.
         * @return Return True if the new ImageIO was successfully registered.
         */
        bool registerImpl(const std::string &extOrName,
                          BuilderFuncPtr newImplBuilder)
        {
            registryMap[extOrName] = newImplBuilder;
        }

        /**
         * Check if some ImageIO is registered for a given name or extension.
         *
         * @param extOrName Input string representing either the ImageIO name
         * or one of the extensions registered.
         * @return Return True if there is any ImageIO registered.
         */
        bool hasImpl(const std::string &extOrName)
        {
            auto it = registryMap.find(extOrName);
            return it != registryMap.end();
        }

        /**
         * Check if some ImageIO is registered for a given name or extension.
         *
         * @param extOrName Input string representing either the ImageIO name
         * or one of the extensions registered.
         * @return Return True if there is any ImageIO registered.
         */
        BuilderFuncPtr getImplBuilder(const std::string &extOrName)
        {
            if (!hasImpl(extOrName))
                return nullptr;

            return registryMap[extOrName];
        }

        T* buildImpl(const std::string &extOrName)
        {
            auto implBuilder = getImplBuilder(extOrName);
            assert(implBuilder!= nullptr);
            return implBuilder(extOrName)();
        }

    private:
        std::map<std::string, BuilderFuncPtr> registryMap;

    }; // class ImplRegistry

} // namespace em

#endif //EM_CORE_REGISTRY_H_H
