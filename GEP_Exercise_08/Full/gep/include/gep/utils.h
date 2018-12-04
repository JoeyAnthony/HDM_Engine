#pragma once

#include <string>
#include <varargs.h>

namespace gep
{
    GEP_API std::string format(GEP_PRINTF_FORMAT_STRING const char* fmt, ...);
    GEP_API std::string vformat(GEP_PRINTF_FORMAT_STRING const char* fmt, va_list args);

    GEP_API std::wstring convertToWideString(const std::string& toConvert);

    /// \brief Finds the last occurence of \a v in the range.
    /// \return `nullptr` if not found, otherwise the position in the range.
    /// \remark It is the responsibility of the caller
    ///         to make sure \a first <= \a last.
    template<typename It, typename ValueType>
    inline It findLast(It first, It last, const ValueType& v)
    {
        auto current = last;
        while(true)
        {
            if (current == first)
            {
                // Not found.
                return nullptr;
            }

            if (*current == v)
            {
                // Found the value.
                return current;
            }

            --current;
        }
    }

    /// \brief Whether \a collection contains \a target.
    template<typename T, typename U>
    inline bool contains(gep::ArrayPtr<T> collection, const U& target)
    {
        for (const auto& element : collection)
        {
            if (element == target)
                return true;
        }
        return false;
    }

    struct NormalizationOptions
    {
        enum Enum
        {
            None = 0,
            PreserveLeadingSlash = 1 << 0, ///< Preserves one trailing slash

            // Add more using 1 << 1, 1 << 2, 1 << 3, etc...
        };
    };

    /// \brief Finds the last occurence of a string \c needle in the \c haystack if any
    ///
    /// This is the opposite of the std c \c strstr
    ///
    /// \return A pointer to the start index with length of the \c needle if any is found,
    /// otherwise a nullptr with length 0
    GEP_API ArrayPtr<char> strrstr( const ArrayPtr< char >& haystack, const ArrayPtr<char  >& needle );

    /// \brief Extracts the relative data path of a given absolute path if any data path is found
    ///
    /// Does the following:
    /// Searches a path for the last occurrence of /data/ and returns a pointer the starting index of the relative data dir
    /// E.g. C:/gep/data/model/model.fbx -> data/model/model.fbx
    /// \return The substring of the data path if any, otherwise a nullptr
    GEP_API ArrayPtr< char > extractDataPath( ArrayPtr< char > path);

    /// \brief Normalizes the given \a path.
    ///
    /// Does the following:
    ///   * Reduce multiple consecutive slashes 1
    ///   * Convert \ slashes to / slashes
    ///   * Remove leading ./ or .\ constructs.
    ///   * Resolve Hello/../World/file.ext => World/file.ext
    ///   * Resolve Hello/./World/file.ext => Hello/World/file.ext
    /// \note The \a path is not checked against the file system,
    ///       i.e. this function can be called on any string,
    ///       even if the given \a path does not exist on the file system.
    /// \param end
    ///        The end of the path string. Useful when working with substrings.
    ///        If \a -1 (default), i.e. max value for size_t, will assume path is
    ///        a zero-terminated string and use strlen to determine the length.
    /// \return New length of the path
    GEP_API size_t normalizePath(char* path, size_t len = -1, uint32 options = NormalizationOptions::None);

    /// \brief Convenience overload to call \a normalizePath(char*, char*) properly with a \c std::string.
    inline void normalizePath(std::string& path, uint32 options = NormalizationOptions::None)
    {
        auto memory = const_cast<char*>(path.c_str());
        auto newLen = normalizePath(memory, path.size(), options);
        path.assign(memory, newLen);
    }

    GEP_API bool areEqual(const char* lhs, const char* rhs, size_t count = -1);

    template<size_t N>
    inline bool areEqual(const char* lhs, const char (&rhs)[N])
    {
        return areEqual(lhs, rhs, N);
    }
}
