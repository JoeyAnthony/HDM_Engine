#include "stdafx.h"
#include "gep/utils.h"

std::string gep::format(const char* fmt, ...)
{
    va_list argptr;
    va_start(argptr, fmt);
    SCOPE_EXIT{ va_end(argptr); });
    return vformat(fmt, argptr);
}

std::string gep::vformat(GEP_PRINTF_FORMAT_STRING const char* fmt, va_list argptr)
{
    static const size_t maxAllocationSize(1024 * 1024 * 10); // 10 MB
    static const size_t bufferSize(1024);

    char buffer[bufferSize];

    int result = vsnprintf(buffer, bufferSize, fmt, argptr);

    if(result < 0)
    {
        for(size_t allocationSize = 2 * bufferSize;
            allocationSize < maxAllocationSize;
            allocationSize *= 2)
        {
            char* largeBuffer = new char[allocationSize];
            SCOPE_EXIT{ delete[] largeBuffer; });

            result = vsnprintf(largeBuffer, allocationSize, fmt, argptr);
            if(result >= 0)
            {
                return std::string(largeBuffer);
            }
        }
        GEP_ASSERT(false, "The string you want to format is too large! "
                   "It did not even fit into %u characters.",
                   maxAllocationSize);
    }

    return std::string(buffer);
}

std::wstring gep::convertToWideString(const std::string& toConvert)
{
    // Implementation taken from http://stackoverflow.com/a/18597384/1282221
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(toConvert);
}

namespace
{
    /// \note Can only be used with types that do not require ctor/dtor calls for now.
    template<typename T, size_t N>
    class FixedStack
    {
    public:
        enum { NumElements = N };
        enum { NumBytes = NumElements * sizeof(T) };
        typedef T ElementType;

    public:
        FixedStack() : m_current(m_data)
        {
        }

    public:
        void push(T element)
        {
            *m_current = std::move(element);
            ++m_current;
        }

        void pop() { --m_current; }

        T& peek() { return *m_current; }
        const T& peek() const { return *m_current; }

    public:
              T* begin()       { return m_data; }
        const T* begin() const { return m_data; }
              T* end()         { return m_current; }
        const T* end() const   { return m_current; }

        size_t length() const { return m_current - m_data; }
        size_t capacity() const { return NumElements; }
        size_t emptySpace() const { return capacity() - length(); }

        bool isEmpty() const { return length() == 0; }

    private:
        T m_data[NumElements];
        T* m_current;
    };

    struct StringView
    {
        char* data;
        size_t length;

        char peek(gep::int32 position = 0) const { return *(data + position); }
        bool isAt(char c) const
        {
            return !isAtEnd() && peek() == c;
        }
        bool isAt(gep::ArrayPtr<const char> string) const
        {
            if (isAtEnd() || string.length() > length)
                return false;
            return gep::areEqual(data, string.getPtr(), string.length());
        }
        bool isAtAny(gep::ArrayPtr<const char> string) const
        {
            if (isAtEnd())
                return false;
            return gep::contains(string, peek());
        }
        bool isAtEnd() const { return length == 0; }
        bool canAdvance(gep::int32 amount = 1) const { return (gep::int64)length - amount >= 0; }
        /// \return \c false if we cannot advance.
        bool advance(gep::int32 amount = 1)
        {
            if (!canAdvance(amount))
                return false;

            data += amount;
            length -= amount;
            return true;
        }
    };
}

static auto g_pathSeparators = gep::makeArrayPtr("/\\");

static bool isPathSeparator(char c)
{
    return gep::contains(g_pathSeparators, c);
}

size_t gep::normalizePath(char* path, size_t len, uint32 options)
{
    if (len == -1)
        len = strlen(path);

    if (len == 0)
        return 0;

    // Convert all / slashes to \ slashes.
    // This make the rest of this code much easier.
    for (auto& c : makeArrayPtr(path, len))
    {
        if (c == '\\')
            c = '/';
    }

    StringView state;
    state.data = path;
    state.length = len;

    // Skip initial ./ or .\ constructs.
    if (state.isAt(makeArrayPtr("./", 2)))
        state.advance(2);

    FixedStack<StringView, 32> stack;

    {
        // If we are supposed to preserve a leading slash, and we are at one currently,
        // we push it on the stack.
        bool preserveLeadingSlash = (options & NormalizationOptions::PreserveLeadingSlash) != 0;
        if(preserveLeadingSlash && state.isAt('/'))
        {
            StringView leadingSlash = state;
            leadingSlash.length = 1;
            stack.push(leadingSlash);
        }
    }

    // Skip all leading slashes.
    while(state.isAt('/'))
        state.advance();

    while(!state.isAtEnd())
    {
        if (state.isAt(makeArrayPtr("./", 2)))
        {
            state.advance(2);
        }
        else if (state.isAt(makeArrayPtr("../", 3)))
        {
            // If the stack is currently empty, we preserve the ../
            if (stack.isEmpty())
            {
                StringView dotDotSlash = state;
                dotDotSlash.length = 3;

                GEP_ASSERT(stack.emptySpace() > 0, "Stack is full. "
                           "Increase the static capacity or use shorter strings as input!");

                // Append the "../" string.
                stack.push(dotDotSlash);
            }
            else
            {
                // To go one level up, just pop the last element off the stack.
                stack.pop();
            }

            // Skip the "../"
            state.advance(3);
        }
        else
        {
            StringView dir = state;

            // Seek forward until we see a slash.
            while (!state.isAtEnd() && !state.isAt('/'))
                state.advance();

            // Keep one trailing slash
            if (state.isAt('/'))
                state.advance();

            // Length of the sub-string is the distance between the current and the previous state.
            dir.length = state.data - dir.data;

            GEP_ASSERT(stack.emptySpace() > 0, "Stack is full. "
                       "Increase the static capacity or use shorter strings as input!");

            // Append the current sub-string `dir`.
            stack.push(dir);
        }

        // Skip all slashes.
        while (state.isAt('/'))
            state.advance();
    }

    auto result = path;
    size_t newLen = 0;
    for (auto& dir : stack)
    {
        memmove(result, dir.data, dir.length);
        newLen += dir.length;
        result += dir.length;
    }

    GEP_ASSERT(newLen <= len);
    return newLen;
}

bool gep::areEqual(const char* lhs, const char* rhs, size_t count /*= -1*/)
{
    if (count == -1)
        return strcmp(lhs, rhs) == 0;

    while(count > 0)
    {
        if (*lhs != *rhs)
            return false;

        ++lhs;
        ++rhs;
        --count;
    }

    return true;
}

GEP_API gep::ArrayPtr< char > gep::extractDataPath( gep::ArrayPtr< char > path )
{
    const char dataString[] = "/data/";
    ArrayPtr<char> idx = strrstr( path, ArrayPtr< char>( const_cast<char*>( dataString ), GEP_ARRAY_SIZE( dataString ) - 1 ) );
    if (idx.getPtr() != nullptr)
    {
        return ArrayPtr<char>( idx.getPtr() + 1, (path.length() - ( idx.getPtr() - path.getPtr() )) - 1 );
    }
    else
    {
        return ArrayPtr<char>();
    }
}


GEP_API gep::ArrayPtr<char> gep::strrstr( const ArrayPtr< char >& haystack, const ArrayPtr< char >& needle )
{
    if (needle.length() <= haystack.length() && haystack.length() != 0 && needle.length() != 0)
    {
        for( size_t i = haystack.length() - 1; i >= needle.length(); i-- )
        {
            bool match = true;
            for( size_t x = needle.length()-1; x > 0; x-- )
            {
                if (needle[x] != haystack[i-needle.length()+x])
                {
                    match = false;
                    break;
                }
            }
            if( match )
            {
                return haystack( i - needle.length(), i );
            }
        }
    }

    return ArrayPtr<char>();
}
