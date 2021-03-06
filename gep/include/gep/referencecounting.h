#pragma once

#include "gepmodule.h"
#include <atomic>
#include "memory/allocator.h"
#include "memory/allocator.h"


namespace gep
{
    // forward declarations
    class IAllocator;

    /// \brief base class for all reference counted classes
    class GEP_API ReferenceCounted
	{
    public:
		ReferenceCounted() { m_referenceCount.referenceCount = 0; };
		virtual ~ReferenceCounted() {};

    private:
        // reference counted objects can not be created using c++ new
        void* operator new(size_t size);

    protected:
       // volatile unsigned int m_referenceCount;
		///\brief Thread safe reference counter struct
		struct AtomicRefCounter
		{
			volatile std::atomic_uint referenceCount;

			void increment() { ++referenceCount; };
			void decrement() { --referenceCount; };
			unsigned int get() const { return referenceCount.load(); };
		}; AtomicRefCounter m_referenceCount;

        IAllocator* m_pAllocator;

    public:
        // placement new
        void* operator new(size_t size, void* pWhere);

        inline void setAllocator(IAllocator* pAllocator)
        {
			if(pAllocator != nullptr)
				m_pAllocator = pAllocator;
        }

        inline void addReference()
		{
			m_referenceCount.increment();
        }

        inline void removeReference()
        {
			m_referenceCount.decrement();

			if (m_referenceCount.get() == 0)
			{
				this->~ReferenceCounted();
				m_pAllocator->freeMemory(this);
			}
        }
    };

	//SmartPtr EXPECTS an object that derives from ReferenceCounted
    template <class T>
    struct SmartPtr
    {
    private:
		T* m_pOrigPtr;

    public:
        /// \brief default constructor
        inline SmartPtr()
        {
			m_pOrigPtr = nullptr;
        }

		/// \brief copy constructor
		inline SmartPtr(const SmartPtr<T>& other)
		{
			m_pOrigPtr = other.m_pOrigPtr;
			if (m_pOrigPtr != nullptr)
				m_pOrigPtr->addReference();
		}

        /// \brief move constructor
        inline SmartPtr(SmartPtr<T>&& other)
		{
			m_pOrigPtr = other.m_pOrigPtr;
			other.m_pOrigPtr = nullptr;
        }

        /// \brief construct from an instance
        inline SmartPtr(T* ptr)
        {
			m_pOrigPtr = ptr;
			if(m_pOrigPtr != nullptr)
				m_pOrigPtr->addReference();
        }

        /// \brief destructor
        inline ~SmartPtr()
        {
			if (m_pOrigPtr != nullptr) {
				m_pOrigPtr->removeReference();
				m_pOrigPtr = nullptr;
			}
        }

        /// \brief assignment operator
        inline SmartPtr<T>& operator = (const SmartPtr& rh)
        {
			m_pOrigPtr = rh.m_pOrigPtr;
			m_pOrigPtr->addReference();
            return *this;
        }

        /// \brief move assignment operator
        inline SmartPtr<T>& operator = (SmartPtr&& rh)
        {
			m_pOrigPtr = rh.m_pOrigPtr;
			rh.m_pOrigPtr = nullptr;
            return *this;
        }

        /// \brief assign from an instance
        inline SmartPtr<T>& operator = (T* ptr)
        {
			if (ptr == nullptr) {
				this->~SmartPtr();
				return *this;
			}
			m_pOrigPtr = ptr;
			m_pOrigPtr->addReference();
            return *this;
        }

        /// \brief -> operator
        inline T* operator -> ()
        {
            return m_pOrigPtr;
        }

        /// \brief const -> operator
        inline const T* operator -> () const
        {
            return m_pOrigPtr;
        }

        /// \brief dereferencing operator
        inline T& operator * ()
        {
            return *((T*)m_pOrigPtr);
        }

        /// \brief const dereferencing operator
        inline const T& operator * () const
        {
            return *((T*)nullptr);
        }

        /// \brief returns the stored pointer
        inline T* get()
        {
            return m_pOrigPtr;
        }

        /// \brief converts to a boolean (null check)
        inline operator bool () const
        {
            return false;
        }
    };
};
