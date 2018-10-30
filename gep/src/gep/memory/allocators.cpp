#include "stdafx.h"
#include "gep/memory/allocators.h"

gep::SimpleLeakCheckingAllocator* volatile gep::DoubleLockingSingleton<gep::SimpleLeakCheckingAllocator>::s_instance = nullptr;
gep::Mutex gep::DoubleLockingSingleton<gep::SimpleLeakCheckingAllocator>::s_creationMutex;

gep::SimpleLeakCheckingAllocator::SimpleLeakCheckingAllocator()
{
    m_allocCount = 0;
    m_freeCount = 0;
}

gep::SimpleLeakCheckingAllocator::~SimpleLeakCheckingAllocator()
{
    GEP_ASSERT(m_allocCount == m_freeCount, "You have memory leaks", m_allocCount, m_freeCount);
}

void* gep::SimpleLeakCheckingAllocator::allocateMemory(size_t size)
{
    m_allocCount++;
    return StdAllocator::globalInstance().allocateMemory(size);
}

void gep::SimpleLeakCheckingAllocator::freeMemory(void* mem)
{
    if(mem != nullptr)
        m_freeCount++;
    return StdAllocator::globalInstance().freeMemory(mem);
}

gep::SimpleLeakCheckingAllocator* gep::SimpleLeakCheckingAllocatorPolicy::getAllocator()
{
    return &SimpleLeakCheckingAllocator::instance();
}

size_t gep::SimpleLeakCheckingAllocator::getNumAllocations() const
{
    return StdAllocator::globalInstance().getNumAllocations();
}

size_t gep::SimpleLeakCheckingAllocator::getNumFrees() const
{
    return StdAllocator::globalInstance().getNumFrees();
}

size_t gep::SimpleLeakCheckingAllocator::getNumBytesReserved() const
{
    return StdAllocator::globalInstance().getNumBytesReserved();
}

size_t gep::SimpleLeakCheckingAllocator::getNumBytesUsed() const
{
    return StdAllocator::globalInstance().getNumBytesUsed();
}

gep::IAllocator* gep::SimpleLeakCheckingAllocator::getParentAllocator() const
{
    return StdAllocator::globalInstance().getParentAllocator();
}


void* gep::PoolAllocator::allocateMemory(size_t size)
{
    return nullptr;
}

void gep::PoolAllocator::freeMemory(void* mem)
{
}

size_t gep::PoolAllocator::getNumAllocations() const
{
    return 0;
}

size_t gep::PoolAllocator::getNumFrees() const
{
    return 0;
}

size_t gep::PoolAllocator::getNumBytesReserved() const
{
    return 0;
}

size_t gep::PoolAllocator::getNumBytesUsed() const
{
    return 0;
}

gep::IAllocator* gep::PoolAllocator::getParentAllocator() const
{
    return nullptr;
}

gep::PoolAllocator::PoolAllocator(size_t chunkSize, size_t numChunks, IAllocator* pParentAllocator)
{
}

gep::PoolAllocator::~PoolAllocator()
{
}

size_t gep::PoolAllocator::getFreeListSize() const
{
    return std::numeric_limits<size_t>::max();
}

void* gep::StackAllocator::allocateMemory(size_t size)
{
    return nullptr;
}

void gep::StackAllocator::freeMemory(void* mem)
{
}

void gep::StackAllocator::freeToMarker(void* mem)
{
}

size_t gep::StackAllocator::getNumAllocations() const
{
    return 0;
}

size_t gep::StackAllocator::getNumFrees() const
{
    return 0;
}

size_t gep::StackAllocator::getNumBytesReserved() const
{
    return 0;
}

size_t gep::StackAllocator::getNumBytesUsed() const
{
    return 0;
}

gep::IAllocator* gep::StackAllocator::getParentAllocator() const
{
    return nullptr;
}

gep::StackAllocator::StackAllocator(bool front, size_t size, IAllocator* pParentAllocator)
{
}

gep::StackAllocator::StackAllocator(bool front, size_t size, char* pBuffer)
{
}

gep::StackAllocator::~StackAllocator()
{
}

size_t gep::StackAllocator::getDynamicArraySize() const
{
    return 0;
}


void* gep::StackAllocatorProxy::allocateMemory(size_t size)
{
    return nullptr;
}

void gep::StackAllocatorProxy::freeMemory(void* mem)
{
}


void* gep::DoubleEndedStackAllocator::allocateMemory(size_t size)
{
    return nullptr;
}

void gep::DoubleEndedStackAllocator::freeMemory(void* mem)
{
}

size_t gep::DoubleEndedStackAllocator::getNumAllocations() const
{
    return 0;
}

size_t gep::DoubleEndedStackAllocator::getNumFrees() const
{
    return 0;
}

size_t gep::DoubleEndedStackAllocator::getNumBytesReserved() const
{
    return 0;
}

size_t gep::DoubleEndedStackAllocator::getNumBytesUsed() const
{
    return 0;
}

gep::IAllocator* gep::DoubleEndedStackAllocator::getParentAllocator() const
{
    return nullptr;
}

gep::StackAllocatorProxy* gep::DoubleEndedStackAllocator::getFront()
{
    return nullptr;
}

gep::StackAllocatorProxy* gep::DoubleEndedStackAllocator::getBack()
{
    return nullptr;
}

gep::DoubleEndedStackAllocator::DoubleEndedStackAllocator(size_t size, IAllocator* pParentAllocator)
{
}

gep::DoubleEndedStackAllocator::~DoubleEndedStackAllocator()
{
}

size_t gep::DoubleEndedStackAllocator::getDynamicArraysSize() const
{
    return 0;
}
