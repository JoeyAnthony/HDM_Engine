#pragma once


namespace gep
{
    GEP_API unsigned int hashOf( const void* buf, size_t len, unsigned int seed = 0 );

    struct StdHashPolicy
    {
        template <class T>
        static unsigned int hash(const T& el)
        {
            return hashOf(&el, sizeof(T));
        }

        template<class T>
        static bool equals(const T& lhs, const T& rhs)
        {
            return lhs == rhs;
        }
    };

    struct StringHashPolicy
    {
        static unsigned int hash(const char* str)
        {
            return hashOf(str, strlen(str));
        }

        static unsigned int hash(const std::string& str)
        {
            return hashOf(str.c_str(), str.length());
        }

        static bool equals(const char* lhs, const char* rhs)
        {
            return strcmp(lhs, rhs) == 0;
        }

        static bool equals(const std::string& lhs, const std::string& rhs)
        {
            return lhs == rhs;
        }
    };

    struct HashMethodPolicy
    {
        template <class T>
        static unsigned int hash(const T& el)
        {
            return el.hash();
        }

        template <class T>
        static unsigned int equals(const T& lhs, const T& rhs)
        {
            return lhs == rhs;
        }
    };

    struct HashMethodPointerPolicy
    {
        template <class T>
        static unsigned hash(const T* el)
        {
            return el->hash();
        }

        template <class T>
        static unsigned equals(const T* lhs, const T* rhs)
        {
            return (*lhs) == (*rhs);
        }
    };

    struct PointerHashPolicy
    {
        inline static unsigned int hash(const void* ptr)
        {
            // We can savely devide the pointer by the architectures default alignment because almost all pointers will be aligned
            // Pointers that are not aligned will cause a hash collision
            #ifdef _M_X64 //x64
            return (reinterpret_cast<unsigned int>(ptr) / 8) % 0xFFFFFFFF;
            #else //X86
            return reinterpret_cast<unsigned int>(ptr) / 4;
            #endif
        }

        static bool equals(const void* lhs, const void* rhs)
        {
            return lhs == rhs;
        }
    };

    struct DontHashPolicy
    {
        template <typename T>
        inline static unsigned int hash(T value)
        {
            return value;
        }

        template<class T>
        static bool equals(const T& lhs, const T& rhs)
        {
            return lhs == rhs;
        }
    };

    template <class K, class V, class HashPolicy>
    class HashmapImpl
    {
      public:
        struct Pair {
            K key;
            V value;
        };

      private:

      public:

        struct Iterator
        {
        private:

        public:
            Iterator(size_t index, HashmapImpl<K, V, HashPolicy>* pBackptr)
            {}

            inline bool operator == (const Iterator& rh) const
            {
                return false;
            }
            inline bool operator != (const Iterator& rh) const
            {
                return true;
            }
            Iterator& operator++()
            {
                return *this;
            }
            Pair* operator->() const
            {
                return nullptr;
            }
            Pair& operator*() const
            {
                return *((Pair*)nullptr);
            }
        };

        struct KeyIterator
        {
        private:
        public:
            KeyIterator(Iterator it)
            {}

            inline bool operator != (const KeyIterator& rh) const
            {
                return true;
            }
            inline bool operator == (const KeyIterator& rh) const
            {
                return false;
            }
            KeyIterator& operator++()
            {
                return *this;
            }
            K* operator->() const
            {
                return nullptr;
            }
            K& operator*() const
            {
                return *((K*)nullptr);
            }
        };

        struct ValueIterator
        {
        private:
        public:
            ValueIterator(Iterator it)
            {}

            inline bool operator != (const ValueIterator& rh) const
            {
                return true;
            }
            inline bool operator == (const ValueIterator& rh) const
            {
                return false;
            }
            ValueIterator& operator++()
            {
                return *this;
            }
            V* operator->() const
            {
                return nullptr;
            }
            V& operator*() const
            {
                return *((V*)nullptr);
            }
        };

        struct KeyRange
        {
        private:
            KeyIterator m_begin, m_end;
        public:
            KeyRange(KeyIterator begin, KeyIterator end) : m_begin(begin), m_end(end) {}
            KeyIterator begin() { return m_begin; }
            KeyIterator end() { return m_end; }
        };

        struct ValueRange
        {
        private:
            ValueIterator m_begin, m_end;
        public:
            ValueRange(ValueIterator begin, ValueIterator end) : m_begin(begin), m_end(end) {}
            ValueIterator begin() { return m_begin; }
            ValueIterator end() { return m_end; }
        };

        /// \brief constructor
        HashmapImpl(IAllocator* allocator)
        {
        }

        /// \brief copy constructor
        HashmapImpl(const HashmapImpl<K, V, HashPolicy>& other)
        {
        }

        /// \brief move constructor
        HashmapImpl(HashmapImpl<K, V, HashPolicy>&& other)
        {
        }

        ~HashmapImpl()
        {
        }

        /// \brief assignment operator
        HashmapImpl<K, V, HashPolicy>& operator = (const HashmapImpl<K, V, HashPolicy>& rh)
        {
            return *this;
        }

        /// \brief move assignment operator
        HashmapImpl<K, V, HashPolicy>& operator = (HashmapImpl<K, V, HashPolicy>&& rh)
        {
            return *this;
        }

        /// \brief [] operator
        V& operator[](const K& key)
        {
          return *((V*)nullptr);
        }

        /// \brief const version of operator []
        const V& operator[](const K& key) const
        {

            GEP_ASSERT(0,"not found");
            throw std::exception("key not found");
        }

        /// \brief checks if a element does exist within the HashmapImpl
        bool exists(const K& key) const
        {
            return false;
        }

        /// \brief tries to retrieve a element from the HashmapImpl. On success outValue will be filled and SUCCESS will be returned, otherwise it will return FAILURE
        Result tryGet(const K& key, V& outValue) const
        {
            return FAILURE;
        }

        /// \brief removes a entry from the HashmapImpl
        Result remove(const K& key)
        {
            return FAILURE;
        }

        /// \brief removes all entries from the HashmapImpl
        void clear()
        {
        }

        /// \brief returns a begin iterator
        inline Iterator begin()
        {
            return Iterator(0, nullptr);
        }

        /// \brief returns a end iterator
        inline Iterator end()
        {
            return Iterator(0, nullptr);
        }

        /// \brief returns a range for iterating the keys
        inline KeyRange keys()
        {
            return KeyRange(KeyIterator(begin()), KeyIterator(end()));
        }

        /// \brief returns a range for iterating the values
        inline ValueRange values()
        {
            return ValueRange(ValueIterator(begin()), ValueIterator(end()));
        }

        /// \brief returns how many elements are inside the HashmapImpl
        inline size_t count() const
        {
            return 0;
        }


        // For testing
        bool isPseudoFull()
        {
            // TODO Return false if any element is marked as free
            return true;
        }
    };

    /// \brief Hashmap indirection to deal with allocator policies and avoid code bloat
    // note: this is already fully implemented, you don't need to change anything here
    template <class K, class V, class HashPolicy = StdHashPolicy, class AllocatorPolicy = StdAllocatorPolicy>
    struct Hashmap : public HashmapImpl<K, V, HashPolicy>
    {
    public:
        inline Hashmap() : HashmapImpl(AllocatorPolicy::getAllocator())
        {
        }

        inline Hashmap(IAllocator* allocator) : HashmapImpl(allocator)
        {
        }

        inline Hashmap(const Hashmap<K, V, HashPolicy, AllocatorPolicy>& rh) : HashmapImpl(rh)
        {
        }

        inline Hashmap(const HashmapImpl<K, V, HashPolicy>& rh) : HashmapImpl(rh)
        {
        }

        inline Hashmap(Hashmap<K, V, HashPolicy, AllocatorPolicy>&& rh) : HashmapImpl(std::move(rh))
        {
        }

        inline Hashmap(HashmapImpl<K, V, HashPolicy>&& rh) : HashmapImpl(std::move(rh))
        {
        }

        inline Hashmap<K, V, HashPolicy, AllocatorPolicy>& operator = (const Hashmap<K, V, HashPolicy, AllocatorPolicy>& rh)
        {
            return static_cast<Hashmap<K, V, HashPolicy, AllocatorPolicy>&>(HashmapImpl<K, V, HashPolicy>::operator=(rh));
        }

        inline Hashmap<K, V, HashPolicy, AllocatorPolicy>& operator = (const HashmapImpl<K, V, HashPolicy>& rh)
        {
            return static_cast<Hashmap<K, V, HashPolicy, AllocatorPolicy>&>(HashmapImpl<K, V, HashPolicy>::operator=(rh));
        }

        inline Hashmap<K, V, HashPolicy, AllocatorPolicy>& operator = (Hashmap<K, V, HashPolicy, AllocatorPolicy>&& rh)
        {
            return static_cast<Hashmap<K, V, HashPolicy, AllocatorPolicy>&>(HashmapImpl<K, V, HashPolicy>::operator=(std::move(rh)));
        }

        inline Hashmap<K, V, HashPolicy, AllocatorPolicy>& operator = (HashmapImpl<K, V, HashPolicy>&& rh)
        {
            return static_cast<Hashmap<K, V, HashPolicy, AllocatorPolicy>&>(HashmapImpl<K, V, HashPolicy>::operator=(std::move(rh)));
        }
    };
}
