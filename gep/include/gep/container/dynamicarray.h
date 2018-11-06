#pragma once
#include "gep/globalManager.h"

//#include "gep/memory/allocator.h"

#define INITIAL_MAX_ELEMENT_SIZE 8

namespace gep
{
    class IAllocator;

    /// \brief a resizeable array
    template <class T>
    struct DynamicArrayImpl
    {
    private:
        // add members here
		//Allocstuff
		//pointer to the first element
		T* m_pMemPtr;
		//byte size of the array
		size_t m_maxElements;
		///////////
		IAllocator* m_pArrayAllocator;
		size_t m_reserveNum;
		unsigned int m_count;
		T* m_pBegin;
		T* m_pEnd;

    public:
        /// \brief constructor
        ///
        /// \param allocator
        ///   the allocator to be used. May not be null
        DynamicArrayImpl(IAllocator* pAllocator)
        {
			m_pArrayAllocator = pAllocator;
			m_maxElements = sizeof(T) * INITIAL_MAX_ELEMENT_SIZE;
			m_pMemPtr = memtools::memoryalloc<T>(m_maxElements);
			m_reserveNum = 0;
			m_count = 0;
			m_pBegin = setEndPtr();
        }

        /// \brief copy constructor
        DynamicArrayImpl(const DynamicArrayImpl<T>& other)
        {
        }

        /// \brief move constructor
        DynamicArrayImpl(DynamicArrayImpl<T>&& other)
        {
        }

        /// \brief constructor with inital data
        DynamicArrayImpl(IAllocator* pAllocator, const ArrayPtr<T>& data)
        {
        }

        /// \brief destructor
        ~DynamicArrayImpl()
        {
        }

        /// \brief copy assignment
        DynamicArrayImpl<T>& operator = (const DynamicArrayImpl<T>& rh)
        {
            GEP_ASSERT(0, "not implemented");
            return *((DynamicArrayImpl<T>*)nullptr);
        }

        /// \brief move assignment
        DynamicArrayImpl<T>& operator = (DynamicArrayImpl<T>&& rh)
        {
            GEP_ASSERT(0, "not implemented");
            return *((DynamicArrayImpl<T>*)nullptr);
        }

        /// \brief [] operator
        T& operator[] (size_t index)
        {
            GEP_ASSERT(0, "not implemented");
            return *((T*)nullptr);
        }

        /// \brief [] operator const
        const T& operator[] (size_t index) const
        {
            GEP_ASSERT(0, "not implemented");
            return *((const T*)nullptr);
        }

        /// \brief reserves at least the given number of elements
        void reserve(size_t numElements)
        {
			int s = sizeof(T)*numElements;
			//TODO resize immediately or when resize happens?
        }

        /// \brief resizes the array to the given number of elements
        void resize(size_t numElements)
        {
			int allocsize = sizeof(T)*numElements;
			if (allocsize < m_reserveNum)
				allocsize = m_reserveNum;

			T* oldalloc = m_pMemPtr;
			T* newalloc = memtools::memoryalloc<T>(numElements * sizeof(T));

			if (numElements < (sizeof(T)*m_count / sizeof(T))) {
				//TODO num of elements are smaller than original
				//g_logMessage("Num of elements are smaller than original");
				GEP_DEBUG_BREAK;
			}
			memmove(newalloc, oldalloc, sizeof(T)*m_count);
			m_pMemPtr = newalloc;
			//memtools::deleteMemory(oldalloc);
			free(oldalloc);

			m_pBegin = m_pMemPtr;
			setEndPtr();
        }

		/// \brief destroys all elements in the array and sets its length to 0
        void clear()
        {
        }

        /// \brief appends a element to the end of the array
        void append(const T& el)
        {
			if(m_pEnd == (m_pMemPtr + m_maxElements + sizeof(T)))
			{
				resize(m_count * 2);
			}

			m_count++;
			m_pEnd = new T(el);
			setEndPtr();
        }

        /// \brief removes the element at the given index shifting all elements behind it one index forth
        void removeAtIndex(size_t index)
        {
        }

        /// \brief inserts a element at the given index
        void insertAtIndex(size_t index, const T& value)
        {
        }

        /// \brief appends an array
        void append(const ArrayPtr<T>& array)
        {

        }

        /// \brief creates a begin iterator
        T* begin()
        {
            // Tip: pointers are valid iterators
            // a begin iterator points to the first element in the container
            return nullptr;
        }

        /// \brief creates a constant begin iterator
        const T* begin() const
        {
            return nullptr;
        }

        /// \brief creates a end iterator
        T* end()
        {
            // A end iterator points to the element after the last element in the container. (the first non valid element)
            return nullptr;
        }

        /// \brief creates a constant end iterator
        const T* end() const
        {
            return nullptr;
        }

        /// \brief creates a array ptr point to the container data
        ArrayPtr<T> toArray()
        {
            return ArrayPtr<T>();
        }

        const ArrayPtr<T> toArray() const
        {
            return ArrayPtr<T>();
        }

        /// \brief returns the length of the dynamic array
        size_t length() const
        {
            return 0;
        }

        /// \brief returns the reserved number of elements
        size_t reserved() const
        {
            return 0;
        }

        /// \brief removes a element without keeping the order of elements
        void removeAtIndexUnordered(size_t index)
        {
        }

        /// \brief returns the last element in the array
        T& lastElement()
        {
        }

        /// \brief returns the last element in the array
        const T& lastElement() const
        {
        }

        /// \brief removes the last element in the array
        void removeLastElement()
        {
        }

    private:
		/// \brief Sets the new end pointer to m_pEnd and returns it
		T* setEndPtr()
		{
			m_pEnd = m_pMemPtr + m_count;
			return m_pEnd;
		}
    };

    /// \brief DynamicArray indirection to deal with allocator policies and avoid code bloat
    // note: this is already fully implemented, you don't need to change anything here
    template <class T, class AllocatorPolicy = StdAllocatorPolicy>
    struct DynamicArray : public DynamicArrayImpl<T>
    {
    public:
        inline DynamicArray() : DynamicArrayImpl(AllocatorPolicy::getAllocator())
        {
        }

        inline DynamicArray(IAllocator* allocator) : DynamicArrayImpl(allocator)
        {
        }

        inline DynamicArray(const ArrayPtr<T>& data) : DynamicArrayImpl(AllocatorPolicy::getAllocator(), data)
        {
        }

        inline DynamicArray(const DynamicArray<T, AllocatorPolicy>& rh) : DynamicArrayImpl(rh)
        {
        }

        inline DynamicArray(const DynamicArrayImpl<T>& rh) : DynamicArrayImpl(rh)
        {
        }

        inline DynamicArray(DynamicArray<T, AllocatorPolicy>&& rh) : DynamicArrayImpl(std::move(rh))
        {
        }

        inline DynamicArray(DynamicArrayImpl<T>&& rh) : DynamicArrayImpl(std::move(rh))
        {
        }

        inline DynamicArray<T, AllocatorPolicy>& operator = (const DynamicArray<T, AllocatorPolicy>& rh)
        {
            return static_cast<DynamicArray<T, AllocatorPolicy>&>(DynamicArrayImpl<T>::operator=(rh));
        }

        inline DynamicArray<T, AllocatorPolicy>& operator = (const DynamicArrayImpl<T>& rh)
        {
            return static_cast<DynamicArray<T, AllocatorPolicy>&>(DynamicArrayImpl<T>::operator=(rh));
        }

        inline DynamicArray<T, AllocatorPolicy>& operator = (DynamicArray<T, AllocatorPolicy>&& rh)
        {
            return static_cast<DynamicArray<T, AllocatorPolicy>&>(DynamicArrayImpl<T>::operator=(std::move(rh)));
        }

        inline DynamicArray<T, AllocatorPolicy>& operator = (DynamicArrayImpl<T>&& rh)
        {
            return static_cast<DynamicArray<T, AllocatorPolicy>&>(DynamicArrayImpl<T>::operator=(std::move(rh)));
        }
    };
}
