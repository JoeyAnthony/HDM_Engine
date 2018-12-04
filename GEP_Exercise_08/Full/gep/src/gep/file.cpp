#include "stdafx.h"
#include "gep/file.h"

bool gep::fileExists(const char* name)
{
    DWORD attributes = GetFileAttributesA(name);
    return (attributes != 0xFFFFFFFF &&
            !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool gep::directoryExists( const char* pathToDir )
{
    DWORD attributes = GetFileAttributesA( pathToDir );
    return ( attributes != INVALID_FILE_ATTRIBUTES &&
        ( attributes & FILE_ATTRIBUTE_DIRECTORY ) );
}

void gep::createDirectory( const char* pathToDir )
{
    if( !directoryExists(pathToDir) )
    {
        CreateDirectoryA( pathToDir, nullptr );
    }
}
