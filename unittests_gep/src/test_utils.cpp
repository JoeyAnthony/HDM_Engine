#include "stdafx.h"

#include <gep/utils.h>

using namespace gep;
using std::begin;
using std::end;

GEP_UNITTEST_GROUP(Utils)
GEP_UNITTEST_TEST(Utils, findLast)
{
    int32 is[5];
    int32* isEnd = is + GEP_ARRAY_SIZE(is);

    is[0] = 0;
    is[1] = 3;
    is[2] = 3;
    is[3] = 3;
    is[4] = 4;

    // Find something thats actually in there multiple times.
    // Expect the last occurence to be found.
    {
        auto result = findLast(is, isEnd, 3);
        GEP_ASSERT(result == is + 3,
                   "Invalid result.");
    }

    // Just for kicks, try with `begin()` and `end()`.
    GEP_ASSERT(findLast(is, isEnd, 3) == findLast(begin(is), end(is), 3),
               "Invalid result.");

    // Should yield a nullptr because 42 is not in the range
    {
        auto result = findLast(is, isEnd, 42);
        GEP_ASSERT(result == nullptr,
                   "Invalid result.");
    }

    // Should yield a nullptr because `first` and `last` are the same.
    {
        auto result = findLast(is, is, 0);
        GEP_ASSERT(result == nullptr,
                   "Invalid result.");
    }
}

GEP_UNITTEST_TEST(Utils, areEqual)
{
    std::string hello1  = "Hello";
    std::string hello2  = "Hello";
    std::string world   = "World";
    std::string good    = "Good";
    std::string goodbye = "Good bye";

    GEP_ASSERT(areEqual(hello1.c_str(), hello2.c_str()));
    GEP_ASSERT(areEqual(hello1.c_str(), hello2.c_str(), 5));
    GEP_ASSERT(areEqual(hello1.c_str(), hello2.c_str(), 3));

    GEP_ASSERT(!areEqual(hello1.c_str(), world.c_str()));

    GEP_ASSERT(!areEqual(good.c_str(), goodbye.c_str()));
    GEP_ASSERT(areEqual(good.c_str(), goodbye.c_str(), good.size()));

    // Corner case?
    {
        std::string a = "aaa";
        std::string b = "aab";
        GEP_ASSERT(!areEqual(a.c_str(), b.c_str(), 3));
    }
}

GEP_UNITTEST_TEST(Utils, normalizePath)
{
    std::string path;

    // Check extreme cases
    {
        // Empty string.
        normalizePath(path);
        GEP_ASSERT(path.empty());

        // Single dot should be preserved.
        path = ".";
        GEP_ASSERT(path == ".");

        // Dot-slash should be preserved.
        path = "./";
        GEP_ASSERT(path == "./");

        // Two dots should be preserved.
        path = "..";
        GEP_ASSERT(path == "..");

        // Two dots and then a slash should be preserved.
        path = "../";
        GEP_ASSERT(path == "../");
    }

    // Convert backslashes to forward slashes
    {
        path = "foo\\bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");
    }

    // Remove duplicate slashes
    {
        path = "foo//bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");

        path = "foo//\\bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");

        path = "foo//\\//bar//\\\\//baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar/baz");
    }

    // With leading ./ which must be discarded
    {
        path = "./foo/bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");

        path = "./foo//bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");

        path = "./foo\\bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");

        path = "./foo\\\\bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz");

        path = "./foo\\\\//bar.baz//\\hello//world.exe";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar.baz/hello/world.exe");
    }

    // With leading ../ which must be preserved
    {
        path = "../foo/bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "../foo/bar.baz");

        path = "../foo//bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "../foo/bar.baz");

        path = "../foo\\bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "../foo/bar.baz");

        path = "../foo\\\\bar.baz";
        normalizePath(path);
        GEP_ASSERT(path == "../foo/bar.baz");

        path = "../foo\\\\//bar.baz//\\hello//world.exe";
        normalizePath(path);
        GEP_ASSERT(path == "../foo/bar.baz/hello/world.exe");
    }

    // Trailing slashes; Only one should be preserved at most.
    {
        path = "foo/bar/";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar/");

        path = "foo//////bar//";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar/");

        path = "foo\\//bar///";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar/");

        path = "foo\\//bar////";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar/");
    }

    // ./ inbetween
    {
        path = "foo/./bar";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar");

        path = "foo//.\\//bar";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar");

        path = "foo//.\\//bar//";
        normalizePath(path);
        GEP_ASSERT(path == "foo/bar/");
    }

    // ../ inbetween
    {
        path = "foo/../bar";
        normalizePath(path);
        GEP_ASSERT(path == "bar");

        path = "foo//..\\//bar";
        normalizePath(path);
        GEP_ASSERT(path == "bar");

        path = "foo//..\\//bar//";
        normalizePath(path);
        GEP_ASSERT(path == "bar/");

        path = "hello/nope/nope/world/nope/../../../../world";
        normalizePath(path);
        GEP_ASSERT(path == "hello/world");
    }

    // Something more complex
    {
        path = "C:\\Users\\SomeOne\\\\Projects/Game_Engine_Programming//Engine/bin/bin32/../..//data/models/barbarian/Barbarian_Belt_Low_d.dds";
        auto expected = "C:/Users/SomeOne/Projects/Game_Engine_Programming/Engine/data/models/barbarian/Barbarian_Belt_Low_d.dds";
        normalizePath(path);
        GEP_ASSERT(path == expected);
    }

    // Preserve trailing slash option
    {
        path = "/Weapons/Some Sound";
        normalizePath(path, NormalizationOptions::PreserveLeadingSlash);
        GEP_ASSERT(path == "/Weapons/Some Sound");

        // Multiple slashes are still reduced to 1
        path = "//Weapons/Some Sound";
        normalizePath(path, NormalizationOptions::PreserveLeadingSlash);
        GEP_ASSERT(path == "/Weapons/Some Sound");

        // Check if it still works without the option and this string.
        path = "//Weapons/Some Sound";
        normalizePath(path);
        GEP_ASSERT(path == "Weapons/Some Sound");
    }
}
