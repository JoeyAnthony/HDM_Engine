#pragma once

#include "gep/threading/mutex.h"
#include <vector>


namespace gep
{
    /// \brief generic allocator interface
    class IAllocator
    {
    public:
        virtual void* allocateMemory(size_t size) = 0;
        virtual void freeMemory(void* mem) = 0;
    };

    /// \brief interface for a allocator which keeps statistics
    class IAllocatorStatistics : public IAllocator
    {
    public:
        /// \brief returns the number of allocations this allocator did so far
        virtual size_t getNumAllocations() const = 0;

        /// \brief returns the number of free operations this allocator did so far
        virtual size_t getNumFrees() const = 0;

        /// \brief returns the amount of memory that is reserved by this allocator
        virtual size_t getNumBytesReserved() const = 0;

        /// \brief returns the amount of memory that is actually used by alive allocations
        virtual size_t getNumBytesUsed() const = 0;

        /// \brief returns the allocator this allocator takes the memory from,
        ///  nullptr if it does not obtain the memory from another allocator
        virtual IAllocator* getParentAllocator() const = 0;
    };

    /// \brief standard allocator
    class GEP_API StdAllocator : public IAllocatorStatistics
    {
    private:
        static volatile StdAllocator* s_globalInstance;

        static Mutex s_creationMutex;

        size_t m_numAllocations;
        size_t m_numFrees;
        size_t m_bytesAllocated;
        size_t m_peakBytesAllocated;

        Mutex m_allocationLock;

        StdAllocator(){}
        ~StdAllocator(){}
    public:
        // IAllocator interface
        virtual void* allocateMemory(size_t size) override;
        virtual void freeMemory(void* mem) override;

        // IAllocatorStatistics Interface
        virtual size_t getNumAllocations() const override;
        virtual size_t getNumFrees() const override;
        virtual size_t getNumBytesReserved() const override;
        virtual size_t getNumBytesUsed() const override;
        virtual IAllocator* getParentAllocator() const override;

        /// \brief returns the only instance of this class
        static StdAllocator& globalInstance(); //not using DoubleLockingSingelton because of cyclic dependency
        static void destroyInstance();
    };

    /// \brief standard allocation policy
    struct StdAllocatorPolicy
    {
        GEP_API static IAllocatorStatistics* getAllocator();
    };
}

#define g_stdAllocator (::gep::StdAllocator::globalInstance())

#include "gep/ArrayPtr.h"

namespace gep
{
    template <class T, class A>
    inline void deleteHelper(T* ptr, A& allocator)
    {
        ptr->~T();
        allocator.freeMemory(ptr);
    }

    template <class T, class A>
    inline void deleteHelper(T* ptr, A* allocator)
    {
        ptr->~T();
        allocator->freeMemory(ptr);
    }


    template <class T, class A, bool isRefCounted>
    struct NewHelper
    {
        static void* newHelper(A* pAllocator)
        {
            GEP_ASSERT(pAllocator != nullptr);
            return pAllocator->allocateMemory(sizeof(T));
        }

        static void* newHelper(A& pAllocator)
        {
            return pAllocator.allocateMemory(sizeof(T));
        }

    };

}

#define GEP_NEW(allocator, T) new (gep::NewHelper<T, typename std::remove_reference<typename std::remove_pointer<decltype(allocator)>::type>::type, false>::newHelper(allocator)) T
#define GEP_NEW_ARRAY(allocator, T, length) gep::NewHelper<T, typename std::remove_reference<typename std::remove_pointer<decltype(allocator)>::type>::type, false>::newArray(allocator, length)
#define GEP_DELETE(allocator, ptr) { gep::deleteHelper(ptr, allocator); ptr = nullptr; }
namespace gep
{
    template<typename T>
    inline void deleteAndNull(T*& ptr){ delete ptr; ptr = nullptr; }
}
#define DELETE_AND_NULL(ptr) ::gep::deleteAndNull((ptr));
