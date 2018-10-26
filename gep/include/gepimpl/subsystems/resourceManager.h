#pragma once
#include "gep/interfaces/resourcemanager.h"

namespace gep
{

    class ResourceManager : public IResourceManager
    {
		// Inherited via IResourceManager
		virtual void initialize() override;
		virtual void destroy() override;
		virtual void deleteResource(IResource * pResource) override;
		virtual void registerResourceType(IResource * pDummy) override;
		virtual void registerResourceType(const char * resourceType) override;
		virtual void finalizeResourcesWithFlags(uint32 flags) override;
	};

#define g_resourceManager (*static_cast<ResourceManager*>(g_globalManager.getResourceManager()))

}
