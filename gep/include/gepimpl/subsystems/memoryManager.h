#pragma once
#include "gep/interfaces/memorymanager.h"

namespace gep
{
    class MemoryManager : public IMemoryManager
    {
		// Inherited via IMemoryManager
		virtual void initialize() override;
		virtual void destroy() override;
		virtual void registerAllocator(const char * name, IAllocatorStatistics * pAllocator) override;
		virtual void deregisterAllocator(IAllocatorStatistics * pAllocator) override;
	};
}
