#pragma once

#include "gep/interfaces/memoryManager.h"
#include "gep/container/DynamicArray.h"
#include "gep/traits.h"

namespace gep
{
    class MemoryManager
        : public IMemoryManager
    {
    private:
        struct AllocatorInfo
        {
            std::string name;
            IAllocatorStatistics* pAllocator;

            // default ctor
            AllocatorInfo() {}

            // ctor
            AllocatorInfo(const char* name, IAllocatorStatistics* pAllocator) :
                name(name),
                pAllocator(pAllocator)
            {}

            // move constructor
            AllocatorInfo(AllocatorInfo&& rh)
            {
                name = std::move(rh.name);
                pAllocator = rh.pAllocator;
            }

			// FIX needed to prevent C2280 when used as T in DynamicArray
			AllocatorInfo(const AllocatorInfo&) = default;
			AllocatorInfo& operator=(const AllocatorInfo&) = default;
		};
        static_assert(isPod<AllocatorInfo>::value == false, "AllocatorInfo should be non-pod");

        DynamicArray<AllocatorInfo> m_allocators;

    public:
        // ISubsystem interface
        virtual void initialize() override;
        virtual void destroy() override;
        virtual void update(float elapsedTime) override;

        // IMemoryManager interface
        virtual void registerAllocator(const char* name, IAllocatorStatistics* pAllocator) override;
        virtual void deregisterAllocator(IAllocatorStatistics* pAllocator) override;
    };
}
