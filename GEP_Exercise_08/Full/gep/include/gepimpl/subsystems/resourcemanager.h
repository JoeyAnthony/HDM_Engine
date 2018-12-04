#pragma once

#include "gep/interfaces/resourceManager.h"
#include "gep/container/hashmap.h"
#include "gep/container/DynamicArray.h"
#include "gep/directory.h"

namespace gep
{

    class ResourceManager
        : public IResourceManager
    {
    private:
        struct ReloadInfo
        {
            ResourcePtr<IResource> pResource;
            IResourceLoader* pLoader;
            uint32 updateNum;
        };


        Hashmap<std::string, IResource*, StringHashPolicy> m_resourceDummies;
        Hashmap<std::string, ReloadInfo, StringHashPolicy> m_fileChangedListener;
        Hashmap<IResourceLoader*, bool, PointerHashPolicy> m_failedInitialLoad;
        Hashmap<std::string, ResourcePtr<IResource>, StringHashPolicy> m_loadedResources;
        Mutex m_fileChangedLock;
        DynamicArray<IResource*> m_newResources;
        DirectoryWatcher m_dataDirWatcher;
        float m_timeSinceLastCheck;
        uint32 m_updateNum;

        void removeFromNewList(IResource* pResource);


    protected:
        virtual ResourcePtr<IResource> doLoadResource(IResourceLoader& loader) override;

    public:
        ResourceManager();

        // ISubsystem interface
        virtual void initialize() override;
        virtual void destroy() override;
        virtual void update(float elapsedTime) override;


        // IResourceManager interface
        virtual void deleteResource(IResource* pResource) override;
        virtual void registerResourceType(IResource* pDummy) override;
		virtual void registerResourceType(const char* resourceType) override;
        virtual void finalizeResourcesWithFlags(uint32 flags) override;
        virtual void registerLoaderForReload(const std::string& filename, IResourceLoader* pLoader, ResourcePtr<IResource> pResource) override;
        virtual void deregisterLoaderForReload(const std::string& filename, IResourceLoader* pLoader) override;
        virtual void reloadResource(ResourcePtr<IResource> pResource);



    };

#define g_resourceManager (*static_cast<ResourceManager*>(g_globalManager.getResourceManager()))

}
