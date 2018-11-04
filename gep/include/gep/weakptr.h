#pragma once

#include "gep/gepmodule.h"
#include "gep/memory/allocator.h"
#include "gep/exit.h"
#include <limits>
#include "gep/threading/mutex.h"

namespace gep
{
#define MAX_WEAK_REFEENCES 512

	/// \brief union for referencing objects in weak pointers
	union WeakRefIndex {
		struct {
			unsigned int index : 24;
			unsigned int hash : 8;
		};
		unsigned int both;
		bool indexCompare(unsigned int other) { return index == other ? true : false; }
		bool hashCompare(unsigned int other) { return hash == other ? true : false; }
	};

    /// \brief base class for all weak referenced objects
    template <class T>
    class WeakReferenced
	{
		WeakRefIndex m_ptrData;
	public:
		static WeakReferenced* globalHandleTable;

		WeakReferenced()
		{
			memset(globalHandleTable, 0, sizeof(WeakReferenced*)*MAX_WEAK_REFEENCES);

			for (int tableindex = 0; tableindex < MAX_WEAK_REFEENCES; tableindex++) {
				if (WeakReferenced::globalHandleTable[tableindex] == nullptr){
					//WeakReferenced::globalHandleTable[tableindex] = this;
					break;
				}
				m_ptrData.both = 1;
			}
		}

        virtual ~WeakReferenced()
        {
        }

        /// \brief gets the weak ref index for debugging purposes
        inline uint32 getWeakRefIndex()
        {
            return 0;
        }

        /// \brief all weak references of this and another weak referenced object
        void swapPlaces(WeakReferenced<T>& other)
        {
        }
    };



    // this macro should define all static members neccessary for WeakReferenced
    #define DefineWeakRefStaticMembers(T)

    template <class T>
    struct WeakPtr
    {
    protected:

    public:
        /// \brief default constructor
        inline WeakPtr()
        {
        }

        /// \brief constructor from an object
        inline WeakPtr(T* ptr)
        {
        }

        /// \brief returns the pointer to the object, might be null
        inline T* get()
        {
            return nullptr;
        }

        /// \brief returns the pointer to the object, might be null
        inline const T* get() const
        {
            return nullptr;
        }

        WeakPtr<T>& operator = (T* ptr)
        {
            return *this;
        }

        /// creates a new weak reference
        void setWithNewIndex(T* ptr)
        {
        }

        /// \brief invalidates a weak reference which was previously created with setWithNewIndex
        ///   and replaces it with the given object
        void invalidateAndReplace(T* ptr)
        {
        }

        /// \brief gets the weak ref index for debugging purposes
        uint32 getWeakRefIndex()
        {
        }
		
    };



}
