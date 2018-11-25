#include "stdafx.h"
#include "gep/memory/allocators.h"
#include "gep/memory/memtools.h"


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


void gep::PoolAllocator::addTop(size_t index)
{
	m_freePtrs[m_numOnStack] = index;
	m_numOnStack++;
}

size_t gep::PoolAllocator::pop()
{
	m_numOnStack--;
	return m_freePtrs[m_numOnStack];
}

void* gep::PoolAllocator::allocateMemory(size_t size)
{
	
	if (size > m_chunkSize || m_numOnStack == 0)
		return nullptr; //and if there are no free indices

	char* ptr = &m_allocation[pop()*m_chunkSize];
	m_numUsedChunks++;

	return ptr;
}

void gep::PoolAllocator::freeMemory(void* mem)
{
	int stackIndex = 0;
	size_t bytes = 0;
	//iterator over pointers
	for(char* It = m_allocation; It < m_allocation+m_reservedBytes; It+=m_chunkSize) {
		if(It == mem)
		{
			stackIndex = bytes / m_chunkSize;
			addTop(stackIndex);
			m_numChunksFreed++;
			break;
		}
		bytes += m_chunkSize;
	}
}

size_t gep::PoolAllocator::getNumAllocations() const
{
    return m_numUsedChunks;
}

size_t gep::PoolAllocator::getNumFrees() const
{
    return m_numChunksFreed;
}

size_t gep::PoolAllocator::getNumBytesReserved() const
{
	return m_chunkSize * m_maxNumChunks + getFreeListSize();
}

size_t gep::PoolAllocator::getNumBytesUsed() const
{
    return m_chunkSize * (m_maxNumChunks - m_numOnStack);
}

gep::IAllocator* gep::PoolAllocator::getParentAllocator() const
{
    return parent;
}

gep::PoolAllocator::PoolAllocator(size_t chunkSize, size_t numChunks, IAllocator* pParentAllocator):
	m_maxNumChunks(numChunks),
	m_chunkSize(memtools::AlignedSize(chunkSize)),
	m_reservedBytes(memtools::AlignedSize(chunkSize) * numChunks + sizeof(int)*numChunks)
{
	if (pParentAllocator == nullptr)
		parent = &StdAllocator::globalInstance();
	else
		parent = pParentAllocator;
	m_allocation = static_cast<char*>( parent->allocateMemory(chunkSize*numChunks));
	
	//initializing free pointer indexes
	m_freePtrs = static_cast<size_t*>( parent->allocateMemory(sizeof(size_t) * m_maxNumChunks));
	for(unsigned int i = 0; i < m_maxNumChunks; i++)
		addTop(i);
}

gep::PoolAllocator::~PoolAllocator()
{
	parent->freeMemory(m_allocation);
	parent->freeMemory(m_freePtrs);
}

size_t gep::PoolAllocator::getFreeListSize() const
{
	return sizeof(size_t)*m_maxNumChunks;
}

////		STACK ALLOCATOR			////
void* gep::StackAllocator::allocateMemory(size_t size)
{
	size_t alignedSize = memtools::AlignedSize(size);
	if (m_numBytesInUse + alignedSize > m_reservedBytes) {
		//g_logMessage("Not enough memory in stack left");
		return nullptr;
	}

	char* returnptr;
	if (isFront) {
		returnptr = m_pEnd;
		m_pLastelement = m_pAllocation + m_numBytesInUse;
		m_pEnd = m_pAllocation + m_numBytesInUse + alignedSize;
	} else {
		returnptr = m_pEnd;
		m_pLastelement = m_pAllocation - m_numBytesInUse;
		m_pEnd = m_pAllocation - m_numBytesInUse - alignedSize;
	}

	m_numUsedChunks++;
	m_numBytesInUse += alignedSize;

	byteSizes.append(alignedSize);

    return returnptr;
}

void gep::StackAllocator::freeMemory(void* mem)
{
	if (mem != m_pLastelement)
		return;
	size_t toFree = byteSizes.lastElement();
	byteSizes.removeLastElement();

	if(isFront){
		m_pEnd -= toFree;
		m_pLastelement -= toFree;
	} else {
		m_pEnd += toFree;
		m_pLastelement += toFree;
	}
	
	m_numChunksFreed++;
	m_numBytesInUse -= toFree;
}

void gep::StackAllocator::freeToMarker(void* mem)
{
	size_t index = 0;
	if (isFront) {
		//find mem
		for (char* it = m_pEnd; it >= m_pAllocation; it -= byteSizes[index], ++index) {
			if(it == mem){
				//get amount of bytes to free
				size_t toFree = 0;
				for (size_t i = 0; i < index; i++) {
					toFree += byteSizes[i];
					byteSizes.removeLastElement();
				}

				//set pointers
				m_pEnd = m_pAllocation + m_numBytesInUse - toFree;
				m_pLastelement = m_pAllocation + m_numBytesInUse - toFree - byteSizes.lastElement(); // minus last bytesize;


				m_numChunksFreed += index;
				m_numBytesInUse -= toFree;
				break;
			}
		}
	} else {
		for (char* it = m_pEnd; it <= m_pAllocation; it += byteSizes[index], ++index){
			if (it == mem){
				//get amount of bytes to free
				size_t toFree = 0;
				for (size_t i = 0; i < index; i++) {
					toFree += byteSizes[i];
					byteSizes.removeLastElement();
				}

				//set pointers
				m_pEnd = m_pAllocation - m_numBytesInUse + toFree;
				m_pLastelement = m_pAllocation - m_numBytesInUse + toFree + byteSizes.lastElement(); // minus last bytesize;


				m_numChunksFreed += index;
				m_numBytesInUse -= toFree;
				break;
			}
		}
	}
}

size_t gep::StackAllocator::getNumAllocations() const
{
    return m_numUsedChunks;
}

size_t gep::StackAllocator::getNumFrees() const
{
    return m_numChunksFreed;
}

size_t gep::StackAllocator::getNumBytesReserved() const
{
    return m_reservedBytes + byteSizes.reserved();
}

size_t gep::StackAllocator::getNumBytesUsed() const
{
	if(!isFront)
		return 1 * (m_pAllocation - m_pEnd);
    return 1 * (m_pEnd - m_pAllocation);
}

gep::IAllocator* gep::StackAllocator::getParentAllocator() const
{
    return parent;
}

gep::StackAllocator::StackAllocator(bool front, size_t size, IAllocator* pParentAllocator) :
	m_reservedBytes(memtools::AlignedSize(size)),
	isFront(front)
{
	if (pParentAllocator == nullptr)
		parent = &StdAllocator::globalInstance();
	else
		parent = pParentAllocator;

	m_pAllocation = static_cast<char*>( parent->allocateMemory(m_reservedBytes));
	byteSizes = DynamicArrayImpl<size_t>(parent);

	if (!isFront)
		m_pAllocation = m_pAllocation + m_reservedBytes;

	m_pEnd = m_pAllocation;
	m_pLastelement = m_pAllocation;
}

gep::StackAllocator::StackAllocator(bool front, size_t size, char* pBuffer) :
	m_reservedBytes(memtools::AlignedSize(size)),
	parent(nullptr),
	isFront(front)
{
	m_pAllocation = pBuffer;
	if (!isFront)
		m_pAllocation = m_pAllocation + m_reservedBytes;

	m_pEnd = m_pAllocation;
	m_pLastelement = m_pAllocation;
	parent = &StdAllocator::globalInstance();
	byteSizes = DynamicArrayImpl<size_t>(parent);
}

gep::StackAllocator::~StackAllocator()
{
	if (parent != nullptr) {
		if (isFront)
			parent->freeMemory(m_pAllocation);
		else
			parent->freeMemory(m_pAllocation - m_reservedBytes);
	}
	byteSizes.~DynamicArray();
}

size_t gep::StackAllocator::getDynamicArraySize() const
{
	return byteSizes.reserved();
}


gep::StackAllocatorProxy::StackAllocatorProxy(bool front, size_t size, IAllocator* pParentAllocator) :
	m_proxyStack(front, memtools::AlignedSize(size), pParentAllocator != nullptr? pParentAllocator : &StdAllocator::globalInstance())
{

}

void* gep::StackAllocatorProxy::allocateMemory(size_t size)
{
	return m_proxyStack.allocateMemory(memtools::AlignedSize(size));
}

void gep::StackAllocatorProxy::freeMemory(void* mem)
{
	m_proxyStack.freeMemory(mem);
}


void* gep::DoubleEndedStackAllocator::allocateMemory(size_t size)
{
    return m_frontStack.allocateMemory(memtools::AlignedSize(size));
}

void gep::DoubleEndedStackAllocator::freeMemory(void* mem)
{
	m_frontStack.freeMemory(mem);
}

size_t gep::DoubleEndedStackAllocator::getNumAllocations() const
{
    return  m_frontStack.m_proxyStack.getNumAllocations() + m_backStack.m_proxyStack.getNumAllocations();
}

size_t gep::DoubleEndedStackAllocator::getNumFrees() const
{
    return m_frontStack.m_proxyStack.getNumFrees() + m_backStack.m_proxyStack.getNumFrees();
}

size_t gep::DoubleEndedStackAllocator::getNumBytesReserved() const
{
    return m_frontStack.m_proxyStack.getNumBytesReserved() + m_backStack.m_proxyStack.getNumBytesReserved();
}

size_t gep::DoubleEndedStackAllocator::getNumBytesUsed() const
{
    return m_frontStack.m_proxyStack.getNumBytesUsed() + m_backStack.m_proxyStack.getNumBytesUsed();
}

gep::IAllocator* gep::DoubleEndedStackAllocator::getParentAllocator() const
{
    return m_frontStack.m_proxyStack.parent;
}

gep::StackAllocatorProxy* gep::DoubleEndedStackAllocator::getFront()
{
    return &m_frontStack;
}

gep::StackAllocatorProxy* gep::DoubleEndedStackAllocator::getBack()
{
    return &m_backStack;
}

gep::DoubleEndedStackAllocator::DoubleEndedStackAllocator(size_t size, IAllocator* pParentAllocator) :
	m_frontStack(true, size, pParentAllocator),
	m_backStack(false, size, pParentAllocator)
{

}

gep::DoubleEndedStackAllocator::~DoubleEndedStackAllocator()
{
	m_frontStack.m_proxyStack.~StackAllocator();
	m_backStack.m_proxyStack.~StackAllocator();
}

size_t gep::DoubleEndedStackAllocator::getDynamicArraysSize() const
{
    return m_frontStack.m_proxyStack.getDynamicArraySize() + m_backStack.m_proxyStack.getDynamicArraySize();
}
