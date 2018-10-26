#include "stdafx.h"
#include "gep/memory/allocator.h"
#include "gep/threading/mutex.h"
#include "gep/exit.h"


volatile gep::StdAllocator* gep::StdAllocator::s_globalInstance = nullptr;

gep::Mutex gep::StdAllocator::s_creationMutex;

void* gep::StdAllocator::allocateMemory(size_t size)
{
    ScopedLock<Mutex> lock(m_allocationLock);
    m_bytesAllocated += size;
    if(m_bytesAllocated > m_peakBytesAllocated)
        m_peakBytesAllocated = m_bytesAllocated;
    m_numAllocations++;
    return malloc(size);
}

void gep::StdAllocator::freeMemory(void* mem)
{
    ScopedLock<Mutex> lock(m_allocationLock);
    if(mem != nullptr)
    {
        ++m_numFrees;
        m_bytesAllocated -= _msize(mem);
        free(mem);
    }
}

size_t gep::StdAllocator::getNumAllocations() const
{
    return m_numAllocations;
}

size_t gep::StdAllocator::getNumFrees() const
{
    return m_numFrees;
}

size_t gep::StdAllocator::getNumBytesReserved() const
{
    return m_peakBytesAllocated;
}

size_t gep::StdAllocator::getNumBytesUsed() const
{
    return m_bytesAllocated;
}

gep::IAllocator* gep::StdAllocator::getParentAllocator() const
{
    return nullptr;
}

gep::StdAllocator& gep::StdAllocator::globalInstance()
{
    // double locking pattern
    // this is NOT an anti pattern if you know what the pitfalls are
    if(s_globalInstance == nullptr)
    {
        ScopedLock<Mutex> lock(s_creationMutex);
        if(s_globalInstance == nullptr)
        {
            static char allocatorInstanceMemory[sizeof(StdAllocator)];
            StdAllocator* stdAllocator = new(allocatorInstanceMemory) StdAllocator();

            s_globalInstance = stdAllocator;

            auto result = gep::atexit(&destroyInstance);
            GEP_ASSERT(result == SUCCESS, "registering exit function failed");
        }
    }
    GEP_ASSERT(s_globalInstance != nullptr);
    return (StdAllocator&)(*s_globalInstance);
}

void gep::StdAllocator::destroyInstance()
{
    ScopedLock<Mutex> lock(s_creationMutex);
    if(s_globalInstance != nullptr)
    {
        auto temp = s_globalInstance;
        s_globalInstance = nullptr;
        temp->~StdAllocator();
    }
}
gep::IAllocatorStatistics* gep::StdAllocatorPolicy::getAllocator()
{
    return &StdAllocator::globalInstance();
}
