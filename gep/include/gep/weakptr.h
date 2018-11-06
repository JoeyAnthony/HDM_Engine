#pragma once

#include "gep/gepmodule.h"
#include "gep/memory/allocator.h"
#include "gep/exit.h"
#include <limits>
#include "gep/threading/mutex.h"

namespace gep
{
#define INITIAL_WEAK_TABLE_LENGTH 8

	/// \brief union for referencing objects in weak pointers
	union WeakRefIndex {
		struct {
			unsigned int index : 24;
			unsigned int hash : 8;
		};
		unsigned int both;
		bool Compare(WeakRefIndex& other) { return both == other.both ? true : false; }
		static WeakRefIndex invalidRef() { return WeakRefIndex{ 0xFFFFFF,0xFF }; }
	};

    /// \brief base class for all weak referenced objects
    template <class T>
    class WeakReferenced
	{
		template <class U> friend struct WeakPtr; //TODO Why class U?
	private:
		static T** m_pGlobalHandleTable;
		static unsigned int m_weakTableLength;
		static unsigned int m_numWeakTableEntries;

		WeakRefIndex m_ptrData;
	public:

		WeakReferenced()
		{
			if (m_weakTableLength == 0) initTable();
			if (m_weakTableLength == m_numWeakTableEntries) enlargeTable();

			//adding reference
			int newIndex = 0;
			m_numWeakTableEntries++;
			for (; newIndex < m_weakTableLength; newIndex++)
			{
				if (m_pGlobalHandleTable[newIndex] == nullptr)
				{
					m_pGlobalHandleTable[newIndex] = static_cast<T*>(this);
					m_ptrData.index = newIndex;
					break;
				}
					
			}
			
		}

        virtual ~WeakReferenced()
        {
			m_numWeakTableEntries--;
			m_pGlobalHandleTable[m_ptrData.index] = nullptr;
        }

        /// \brief gets the weak ref index for debugging purposes
        inline uint32  getWeakRefIndex()
        {
			return m_ptrData.index;
        }

        /// \brief all weak references of this and another weak referenced object
        void swapPlaces(WeakReferenced<T>& other)
        {
        }

		static void enlargeTable()
		{
			//TODO important yo
		}

		static void initTable()
		{
			m_pGlobalHandleTable = new T* [INITIAL_WEAK_TABLE_LENGTH];
			memset(m_pGlobalHandleTable, 0, INITIAL_WEAK_TABLE_LENGTH * sizeof(T));
			m_weakTableLength = INITIAL_WEAK_TABLE_LENGTH;
		}
    };

    // this macro should define all static members neccessary for WeakReferenced
	// other static member initialization
    #define DefineWeakRefStaticMembers(T) \
	T** gep::WeakReferenced<T>::m_pGlobalHandleTable = nullptr; \
	unsigned int gep::WeakReferenced<T>::m_weakTableLength = 0; \
	unsigned int gep::WeakReferenced<T>::m_numWeakTableEntries = 0;

    template <class T>
    struct WeakPtr
    {
    protected:
		WeakRefIndex m_ptrData;
    public:
        /// \brief default constructor
        inline WeakPtr()
        {
			m_ptrData = WeakRefIndex::invalidRef();
        }

        /// \brief constructor from an object
        inline WeakPtr(T* ptr)
        {
			m_ptrData = ptr->m_ptrData;
        }

        /// \brief returns the pointer to the object, might be null
        inline T* get()
        {
			//if (m_ptrData.both == WeakRefIndex::invalidRef().both)
				//return nullptr;

			WeakReferenced<T>* temp = WeakReferenced<T>::m_pGlobalHandleTable[m_ptrData.index];
			if (temp == nullptr)
				return nullptr;
			if (m_ptrData.Compare(temp->m_ptrData))
				return WeakReferenced<T>::m_pGlobalHandleTable[m_ptrData.index];

			return nullptr;
        }

        /// \brief returns the pointer to the object, might be null
        inline const T* get() const
		{
			unsigned int index = m_ptrData.index;

			//WeakReferenced<T>* temp = WeakReferenced<T>::m_pGlobalHandleTable[m_ptrData.index];
			//if (m_ptrData.Compare(temp->m_ptrData))
				return WeakReferenced<T>::m_pGlobalHandleTable[index];

			//return nullptr;
        }

        WeakPtr<T>& operator = (T* ptr)
        {
			if (ptr == nullptr) {
				m_ptrData = WeakRefIndex::invalidRef();
				return *this;
			}

			m_ptrData = ptr->m_ptrData;
            return *this;
        }

        /// creates a new weak reference
        void setWithNewIndex(T* ptr)
        {
			//m_ptrData = ptr.m_ptrData;
        }

        /// \brief invalidates a weak reference which was previously created with setWithNewIndex
        ///   and replaces it with the given object
        void invalidateAndReplace(T* ptr)
        {
        }

        /// \brief gets the weak ref index for debugging purposes
        uint32 getWeakRefIndex()
        {
			return m_ptrData.index;
        }

		inline ~WeakPtr()
		{
			//delete WeakReferenced<T>::m_pGlobalHandleTable[m_ptrData.index];
			m_ptrData = WeakRefIndex::invalidRef();
		}
		
    };



}
