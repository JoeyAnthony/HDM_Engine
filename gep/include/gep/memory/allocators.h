#pragma once
#pragma warning( disable : 4251 )

#include "gep/memory/allocator.h"
#include "gep/singleton.h"
#include "gep/container/dynamicarray.h"

namespace gep
{
    /// \brief a simple leak checking allocator
    class GEP_API SimpleLeakCheckingAllocator : public IAllocatorStatistics,
        public DoubleLockingSingleton<SimpleLeakCheckingAllocator>
    {
        // the singelton template needs to be friend so it can create this class
        friend class DoubleLockingSingleton<SimpleLeakCheckingAllocator>;
    private:
        size_t m_allocCount;
        size_t m_freeCount;

        SimpleLeakCheckingAllocator();
        ~SimpleLeakCheckingAllocator();

    public:
        virtual void* allocateMemory(size_t size) override;
        virtual void freeMemory(void* mem) override;

        // IAllocatorStatistics Interface
        virtual size_t getNumAllocations() const override;
        virtual size_t getNumFrees() const override;
        virtual size_t getNumBytesReserved() const override;
        virtual size_t getNumBytesUsed() const override;
        virtual IAllocator* getParentAllocator() const override;

        inline size_t getAllocCount() const { return m_allocCount; }
        inline size_t getFreeCount() const { return m_freeCount; }
    };

    /// \brief allocator policy for simple leak checking allocator
    struct SimpleLeakCheckingAllocatorPolicy
    {
        GEP_API static SimpleLeakCheckingAllocator* getAllocator();
    };


    /// \brief pool allocator
    // TODO Add locking policy
    class GEP_API PoolAllocator : public IAllocatorStatistics
    {
    private:
		size_t m_numUsedChunks = 0;							//total number of allocations
		size_t m_numChunksFreed = 0;						//total freed chunks
		const size_t m_chunkSize = 0;						//size of the chunks
		const size_t m_reservedBytes = 0;					//total number of bytes this object allocated
		const size_t m_maxNumChunks = 0;					//max number of chunks that can be allocated
   	
		size_t m_numOnStack = 0;							//stack index

        // not accessible
        PoolAllocator(){}
        PoolAllocator(const PoolAllocator& other){}
        PoolAllocator(PoolAllocator&& other){}

		size_t* m_freePtrs;									//stack pointer
		char* m_allocation;									//allocation pointer
		IAllocator* parent;									//parent allocator

    public:
        // IAllocator interface
        virtual void* allocateMemory(size_t size) override;
        virtual void freeMemory(void* mem) override;

        // IAllocatorStatistics Interface
		/// \brief total number of allocations
        virtual size_t getNumAllocations() const override;

		/// \brief total number of freed chunks
        virtual size_t getNumFrees() const override;

		/// \brief maximum number of bytes that can be allocated
        virtual size_t getNumBytesReserved() const override;

		/// \brief number of bytes currently in use
        virtual size_t getNumBytesUsed() const override;

        virtual IAllocator* getParentAllocator() const override;

        PoolAllocator(size_t chunkSize, size_t numChuks, IAllocator* pParentAllocator = nullptr);
        ~PoolAllocator();

        // returns the size of the free list in bytes
        size_t getFreeListSize() const;

	private:
    	// stack methods
		void addTop(size_t index);
		size_t pop();
    };

	class DoubleEndedStackAllocator;
	class StackAllocatorProxy;
	/// \brief stack allocator
    // TODO Add locking policy
    class GEP_API StackAllocator : public IAllocatorStatistics
    {
		friend DoubleEndedStackAllocator;
		friend StackAllocatorProxy;
    private:
		size_t m_numUsedChunks = 0;							//total number of allocations
		size_t m_numChunksFreed = 0;						//total freed chunks
		size_t m_numBytesInUse = 0;
		const size_t m_reservedBytes = 0;					//bytes reserved only by the stack pointer

		DynamicArray<size_t> byteSizes;
		IAllocator* parent;

		char* m_pAllocation;
		char* m_pEnd;										//next empty pointer
		char* m_pLastelement;								//pointer to the last element

		const bool isFront = true;							//remove if unnecessary

        // not accessible
        StackAllocator(){}
        StackAllocator(const StackAllocator& other){}
        StackAllocator(StackAllocator&& other){}

    public:
        // IAllocator interface
        virtual void* allocateMemory(size_t size) override;
        virtual void freeMemory(void* mem) override;

        // frees multiple allocations on the stack at once
        void freeToMarker(void* mem);
        inline void* getMarker()
        {
            return nullptr;
        }

        // IAllocatorStatistics Interface
        virtual size_t getNumAllocations() const override;
        virtual size_t getNumFrees() const override;
        virtual size_t getNumBytesReserved() const override;
        virtual size_t getNumBytesUsed() const override;
        virtual IAllocator* getParentAllocator() const override;

        StackAllocator(bool front, size_t size, IAllocator* pParentAllocator = nullptr);
        StackAllocator(bool front, size_t size, char* pBuffer);
        ~StackAllocator();

        // returns the memory reserved by the internally used dynamic array
        size_t getDynamicArraySize() const;
    };

    /// \brief stack allocator proxy used by double ended stack allocator
    class GEP_API StackAllocatorProxy : IAllocator
    {
		friend DoubleEndedStackAllocator;
    private:
		DoubleEndedStackAllocator* doubleEndedStack;
		StackAllocator m_proxyStack;
    public:
		StackAllocatorProxy();
		~StackAllocatorProxy();

		StackAllocatorProxy(const StackAllocatorProxy& other);
		StackAllocatorProxy(StackAllocatorProxy&& other);
		StackAllocatorProxy(bool front, size_t size, DoubleEndedStackAllocator* deStack, IAllocator* pParentAllocator = nullptr);
        virtual void* allocateMemory(size_t size) override;
        virtual void freeMemory(void* mem) override;
    };

    /// \brief double ended stack allocator
    class GEP_API DoubleEndedStackAllocator : public IAllocatorStatistics
    {
    private:
		StackAllocatorProxy m_frontStack;
		StackAllocatorProxy m_backStack;

        // not accessible
        DoubleEndedStackAllocator(){} 
        
		DoubleEndedStackAllocator(const DoubleEndedStackAllocator& other){};
		DoubleEndedStackAllocator(DoubleEndedStackAllocator&& other){};

    public:
        // IAllocator interface
        virtual void* allocateMemory(size_t size) override;
        virtual void freeMemory(void* mem) override;

		bool isOverlapping(size_t size);

        // IAllocatorStatistics Interface
        virtual size_t getNumAllocations() const override;
        virtual size_t getNumFrees() const override;
        virtual size_t getNumBytesReserved() const override;
        virtual size_t getNumBytesUsed() const override;
        virtual IAllocator* getParentAllocator() const override;

        // stack allocator proxies
        StackAllocatorProxy* getFront();
        StackAllocatorProxy* getBack();

        DoubleEndedStackAllocator(size_t size, IAllocator* pParentALlocator = nullptr);
        ~DoubleEndedStackAllocator();

        // returns the memory reserved by the internally used dynamic arrays of both stack allocators
        size_t getDynamicArraysSize() const;
    };
}
#define g_simpleLeakCheckingAllocator gep::SimpleLeakCheckingAllocator::instance()
