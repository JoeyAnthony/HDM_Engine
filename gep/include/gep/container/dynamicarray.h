#pragma once
#include <iostream>
#include "gep/globalManager.h"
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
		size_t m_reserveNumElements;
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
			init();
        }

        /// \brief copy constructor
        DynamicArrayImpl(const DynamicArrayImpl<T>& other)
        {
			copy(other);
        }

        /// \brief move constructor
        DynamicArrayImpl(DynamicArrayImpl<T>&& other)
		{
			move(other);
		}

        /// \brief constructor with inital data
        DynamicArrayImpl(IAllocator* pAllocator, const ArrayPtr<T>& data) : DynamicArrayImpl(pAllocator)
        {
			if (data.length() > m_count)
				resize(data.length() * 2 - data.length() / 2);
			memcpy(m_pMemPtr, data.getPtr(), data.length()*sizeof(T));
			m_count = data.length();
        }

        /// \brief destructor
        ~DynamicArrayImpl()
        {
			clear(true);
			//m_pArrayAllocator = nullptr;
        }

        /// \brief copy assignment
        DynamicArrayImpl<T>& operator = (const DynamicArrayImpl<T>& rh)
        {
			//free memory, then copy
			clear(true);
			copy(rh);
            return *(this);
        }

        /// \brief move assignment
        DynamicArrayImpl<T>& operator = (DynamicArrayImpl<T>&& rh)
        {
			//free memory, then move
			clear(true);
			move(rh);
            return *(this);
        }


        /// \brief [] operator
        T& operator[] (size_t index)
        {
            return *(m_pMemPtr+index);
        }

        /// \brief [] operator const
        const T& operator[] (size_t index) const
        {
			return *(m_pMemPtr + index);
        }


        /// \brief reserves at least the given number of elements
        void reserve(size_t numElements)
        {
			int s = sizeof(T)*numElements;
        }

        /// \brief resizes the array to the given number of elements
        void resize(size_t numElements)
        {
			size_t newNumOfElements = numElements;
			if (numElements < m_reserveNumElements)
				newNumOfElements = m_reserveNumElements;

			T* oldalloc = m_pMemPtr;
			T* newalloc =  static_cast<T*>( m_pArrayAllocator->allocateMemory(newNumOfElements * sizeof(T)));

			if (newNumOfElements < m_count) {
				//TODO num of elements are smaller than original
				//g_logMessage("Num of elements are smaller than original");
				GEP_DEBUG_BREAK;
			}
			memcpy(newalloc, oldalloc, sizeof(T)*m_count);
			m_pMemPtr = newalloc;
			m_pArrayAllocator->freeMemory(oldalloc);

			m_maxElements = newNumOfElements;
			m_pBegin = m_pMemPtr;
			setEndPtr();
        }

		/// \brief destroys all elements in the array and sets its length to 0
        void clear(bool destruct=false)
        {
			if (m_pMemPtr == nullptr)
			{
				printf("null \n");
			}
			if (m_pMemPtr == nullptr)
				return;
			m_pArrayAllocator->freeMemory(m_pMemPtr);
			m_pMemPtr = nullptr;
			m_maxElements = 0;
			m_reserveNumElements = 0;
			m_count = 0;
			m_pBegin = nullptr;
			m_pEnd = nullptr;
			//if destruct is true it means the array will be uninitialized and not ready for use
			if (!destruct)
				init();
        }

        /// \brief appends a element to the end of the array
        void append(const T& el)
        {
			if(m_pEnd == (m_pMemPtr + m_maxElements))
			{
				resize(m_count * 2);
			}

			new (m_pEnd) T(el);
			m_count++;
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
			if (m_count < m_count + array.length())
				resize(m_count + array.length());

			memcpy(m_pEnd, array.getPtr(), sizeof(T) * array.length());
			m_count+=array.length();
			setEndPtr();
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
            return ArrayPtr<T>(m_pMemPtr, m_count);
        }

        const ArrayPtr<T> toArray() const
        {
            return ArrayPtr<T>();
        }

        /// \brief returns the length of the dynamic array
        size_t length() const
        {
            return m_count;
        }

        /// \brief returns the reserved number of elements
        size_t reserved() const
        {
            return m_reserveNumElements;
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
		/// \brief Initializes all values, m_pAllocator cannot be null
		void init(int maxElements = INITIAL_MAX_ELEMENT_SIZE)
		{
			m_maxElements = maxElements;

			m_pMemPtr = static_cast<T*>(m_pArrayAllocator->allocateMemory(sizeof(T) * m_maxElements));

			m_reserveNumElements = 0;
			m_count = 0;
			m_pBegin = setEndPtr();
		}

		/// \brief copies other dynamic array in this array
		void copy(const DynamicArrayImpl<T>& other)
		{
			m_pArrayAllocator = other.m_pArrayAllocator;
			init(other.m_count);
			memcpy(m_pMemPtr, other.m_pMemPtr, other.m_count * sizeof(T));
			m_reserveNumElements = other.m_reserveNumElements;
			//init resets count
			m_count = other.m_count;
		}

		/// \brief moves other dynamic array in this array
		void move(DynamicArrayImpl<T>& other)
		{
			m_pArrayAllocator = other.m_pArrayAllocator;
			m_pMemPtr = other.m_pMemPtr;
			m_maxElements = other.m_maxElements;
			m_reserveNumElements = other.m_reserveNumElements;
			m_count = other.m_count;
			m_pBegin = other.m_pBegin;
			m_pEnd = other.m_pEnd;

			//////
			//other.m_pArrayAllocator = nullptr;
			other.m_pMemPtr = nullptr;
			other.m_maxElements = 0;
			other.m_reserveNumElements = 0;
			other.m_count = 0;
			other.m_pBegin = nullptr;
			other.m_pEnd = nullptr;
		}

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
