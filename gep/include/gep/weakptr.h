#pragma once

#include "gep/gepmodule.h"
#include "gep/memory/allocator.h"
#include "gep/exit.h"
#include <limits>
#include "gep/threading/mutex.h"


namespace gep
{


    /// \brief base class for all weak referenced objects
    template <class T>
    class WeakReferenced
    {
    public:
        WeakReferenced()
        {
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
