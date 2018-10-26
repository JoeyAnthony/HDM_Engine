#pragma once

#include "gep/interfaces/subsystem.h"

namespace gep
{
    //forward reference
    class IResource;
    class IResourceLoader;
    template <class T>
    struct ResourcePtr;


    class IResource
    {
        friend class IResourceManager;
    public:
    };


    /// \brief class which is responsible for loading resources
    class IResourceLoader
    {
		friend class ResourceManager;
    public:
        virtual ~IResourceLoader() {}
    };


    /// \brief interface for the resource manager
    class IResourceManager : public ISubsystem
    {
    public:
        virtual ~IResourceManager() {}



        /// \brief deletes a resource
        virtual void deleteResource(IResource* pResource) = 0;

        /// \brief registeres a new resource type
        /// \param pDummy
        ///   a pointer to the dummy resource which should be used for this type
        virtual void registerResourceType(IResource* pDummy) = 0;

		/// \brief registeres a new resource type
        /// \param name
        ///   the name of the resource type
		virtual void registerResourceType(const char* resourceType) = 0;

        /// \brief finalizes resources with a given set of flags
        virtual void finalizeResourcesWithFlags(uint32 flags) = 0;

    };
}
