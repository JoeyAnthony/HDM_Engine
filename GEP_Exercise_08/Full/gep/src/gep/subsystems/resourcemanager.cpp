#include "stdafx.h"
#include "gepimpl/subsystems/resourceManager.h"
#include "gep/exception.h"
#include "gep/utils.h"
#include "gep/globalManager.h"
#include "gep/interfaces/logging.h"
#include "gep/interfaces/updateFramework.h"
#include "gep/file.h"
#include <algorithm>

DefineWeakRefStaticMembersExport(gep::IResource)

gep::ResourceManager::ResourceManager()
  : m_dataDirWatcher("data", DirectoryWatcher::WatchSubdirs::yes, DirectoryWatcher::Watch::writes),
    m_timeSinceLastCheck(0.1f),
    m_updateNum(0)
{
}

void gep::ResourceManager::initialize()
{
}

void gep::ResourceManager::destroy()
{
    IResource** e = end();
    for(IResource** it = begin(); it < e; ++it)
    {
        auto ptr = *it;
        if(ptr != nullptr)
        {
            IResource* pResource = ptr;
            if(pResource != nullptr)
            {
                deleteResource(pResource);
            }
        }
    }
    for(auto pLoader : m_failedInitialLoad.keys())
    {
        pLoader->release();
    }
    for(IResource* pDummy : m_resourceDummies.values())
    {
        if(pDummy != nullptr)
        {
            IResourceLoader* pLoader = pDummy->getLoader();
            pDummy->unload();
            pLoader->deleteResource(pDummy);
            pLoader->release();
        }
    }
    m_failedInitialLoad.clear();
    m_resourceDummies.clear();
    m_fileChangedListener.clear();
    m_loadedResources.clear();
}

void gep::ResourceManager::update(float elapsedTime)
{
    m_timeSinceLastCheck -= elapsedTime;
    if(m_timeSinceLastCheck <= 0.0f)
    {
        m_updateNum++;
        ScopedLock<Mutex> lock(m_fileChangedLock);
        m_dataDirWatcher.enumerateChanges([=](const char* filename, DirectoryWatcher::Action::Enum action)
        {
            if(action != DirectoryWatcher::Action::modified)
                return;
            std::string path = "data\\" + std::string(filename);
            ReloadInfo info;
            if(m_fileChangedListener.tryGet(path, info))
            {
                // Some modified events come in twice. Make sure to not handle them twice
                if(info.updateNum == m_updateNum)
                    return;
                g_globalManager.getLogging()->logMessage("Reloading '%s' resource from file '%s'.", info.pLoader->getResourceType(), filename);
                m_fileChangedListener[path].updateNum = m_updateNum;
                // the modified file might still be be open for writing, wait until its possible to read it
                Sleep(10);
                while(true)
                {
                    RawFile file(path.c_str(), "r");
                    if(file.isOpen())
                        break;
                    Sleep(100);
                }
                try {
                    IResource* pDummyResource = nullptr;
                    IResource* pResourceToReload = info.pResource;
                    if(!m_resourceDummies.tryGet(std::string(info.pLoader->getResourceType()), pDummyResource))
                    {
                        GEP_ASSERT(false, "resource type not registered yet");
                    }
                    IResource* pNewResource = info.pLoader->loadResource((pDummyResource == pResourceToReload) ? nullptr : pResourceToReload);
                    if(pNewResource != nullptr)
                    {
                        if(m_failedInitialLoad.exists(info.pLoader))
                            m_failedInitialLoad.remove(info.pLoader);
                        if(pResourceToReload != nullptr && pResourceToReload != pDummyResource)
                        {
                            // swap if it did not reload inplace
                            if(pResourceToReload != pNewResource)
                            {
                                pNewResource->swapPlaces(*pResourceToReload);
                                removeFromNewList(pResourceToReload);
                                info.pLoader->deleteResource(pResourceToReload);
                            }
                        }
                        else
                        {
                            //replace the dummy resource
                            invalidateAndReplace(info.pResource, pNewResource);
                        }

                        if(pNewResource->getFinalizeOptions() > 0)
                        {
                            m_newResources.append(pNewResource);
                        }
                        pNewResource->setLoader(info.pLoader);
                    }
                    else
                    {
                        g_globalManager.getLogging()->logError("Failed to reload '%s' resource from file '%s'. Loader returned null.", info.pLoader->getResourceType(), filename);
                    }
                }
                catch(LoadingError& ex)
                {
                    g_globalManager.getLogging()->logError("Failed to reload '%s' resource from file '%s' error:\n%s", info.pLoader->getResourceType(), filename, ex.what());
                }
            }
        });
        m_timeSinceLastCheck = 0.1f;
    }
}


gep::ResourcePtr<gep::IResource> gep::ResourceManager::doLoadResource(IResourceLoader& loader)
{
    // This call makes sure that all equivalent file paths will actually look the same.
    // E.g. "./data/models/something.FBX" referes to the same file as "data\models\toilet\something.FBX".
    // Note: We preserve leading slashes because resources like FMOD sounds use them as ID.
    normalizePath(loader.m_resourceId, NormalizationOptions::PreserveLeadingSlash);
    ResourcePtr<IResource> alreadyLoaded;
    if(m_loadedResources.tryGet(loader.m_resourceId, alreadyLoaded))
    {
        g_globalManager.getLogging()->logMessage("Reusing already loaded resource '%s'", loader.m_resourceId.c_str());
        return alreadyLoaded;
    }
    IResourceLoader* pLoader = loader.moveToHeap();
    GEP_ASSERT(pLoader != nullptr);
        try {
            IResource* pResult = pLoader->loadResource(nullptr);
            if(pResult != nullptr)
            {
                if(pResult->getFinalizeOptions() > 0)
                {
                    m_newResources.append(pResult);
                }
                pResult->setLoader(pLoader);

                auto result = makeResourcePtr(pResult, false);
                pLoader->postLoad(result);
                m_loadedResources[pLoader->getResourceId()] = result;
                GEP_ASSERT(result != nullptr);
                return result;
            }
        }
        catch(LoadingError& ex)
        {
            g_globalManager.getLogging()->logError("%s", ex.what());
        }
    IResource* pResult = nullptr;
    m_failedInitialLoad[pLoader] = true;
    m_resourceDummies.tryGet(std::string(pLoader->getResourceType()), pResult);
    GEP_ASSERT(pResult != nullptr, "Unkown resource type", pLoader->getResourceType());
    auto result = makeResourcePtr(pResult, true);
    m_loadedResources[pLoader->getResourceId()] = result;
    pLoader->postLoad(result);
    GEP_ASSERT(result != nullptr);
    return result;
}

void gep::ResourceManager::removeFromNewList(IResource* pResource)
{
    for(size_t i=0; i < m_newResources.length(); i++)
    {
        if(m_newResources[i] == pResource)
        {
            m_newResources.removeAtIndexUnordered(i);
            break;
        }
    }
}

void gep::ResourceManager::reloadResource(ResourcePtr<IResource> ptr)
{
    // not implemented yet
}

void gep::ResourceManager::deleteResource(IResource* pResource)
{
    if(pResource == nullptr)
        return;
    auto pLoader = pResource->getLoader();
    if(m_loadedResources.exists(pLoader->m_resourceId))
        m_loadedResources.remove(pLoader->m_resourceId);
    IResource* pDummyResource = nullptr;
    if(!m_resourceDummies.tryGet(pLoader->getResourceType(), pDummyResource))
    {
        GEP_ASSERT(false, "resource type not registered yet");
    }
    if(pResource != pDummyResource)
    {
        removeFromNewList(pResource);
        pResource->unload();
        pLoader->deleteResource(pResource);
        pLoader->release();
    }
}

void gep::ResourceManager::registerResourceType(IResource* pDummy)
{
    const char* name = pDummy->getLoader()->getResourceType();
    GEP_ASSERT(!m_resourceDummies.exists(std::string(name)), "resource type already exists");
    m_resourceDummies[std::string(name)] = pDummy;
}

void gep::ResourceManager::registerResourceType(const char* resourceType)
{
    const char* name = resourceType;
    GEP_ASSERT(!m_resourceDummies.exists(std::string(name)), "resource type already exists");
    m_resourceDummies[std::string(name)] = nullptr;
}

void gep::ResourceManager::finalizeResourcesWithFlags(uint32 flags)
{
    for(size_t i=0; i < m_newResources.length(); )
    {
        IResource* pResource = m_newResources[i];
        uint32 resourceFlags = pResource->getFinalizeOptions();
        if((resourceFlags & ResourceFinalize::NotYet) == 0 && (resourceFlags & flags) > 0)
        {
            pResource->finalize();
            m_newResources.removeAtIndexUnordered(i); // this removes a element => we don't need to increment the index
        }
        else
        {
            i++;
        }
    }
}

void gep::ResourceManager::registerLoaderForReload(const std::string& filename, IResourceLoader* pLoader, ResourcePtr<IResource> pResource)
{
    std::string fixedpath(filename);
    std::replace(fixedpath.begin(), fixedpath.end(), '/', '\\');
    {
      size_t pos = 0;
      while((pos = fixedpath.find("\\\\", pos)) != std::string::npos)
      {
         fixedpath.replace(pos, 2, "\\");
         pos += 1;
      }
    }
    if(fixedpath.substr(0, 2) == ".\\")
      fixedpath = fixedpath.substr(2);
    ScopedLock<Mutex> lock(m_fileChangedLock);
    ReloadInfo& info = m_fileChangedListener[fixedpath];
    info.pLoader = pLoader;
    info.pResource = pResource;
    info.updateNum = m_updateNum;
}

void gep::ResourceManager::deregisterLoaderForReload(const std::string& filename, IResourceLoader* pLoader)
{
    std::string fixedpath(filename);
    std::replace(fixedpath.begin(), fixedpath.end(), '/', '\\');
    ScopedLock<Mutex> lock(m_fileChangedLock);
    ReloadInfo info;
    if(m_fileChangedListener.tryGet(fixedpath, info))
    {
        if(info.pLoader == pLoader)
            m_fileChangedListener.remove(fixedpath);
    }
}



