// unittests.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//

#include "stdafx.h"
#include "gep/unittest/UnittestManager.h"

// implement new/delete
#include "gep/memory/newdelete.inl"

int main(int argc, const char* argv[])
{
    bool doDebugBreaks = false;
    bool pause = true;
    for(int i=1; i<argc; i++)
    {
        if(!strcmp(argv[i], "-debugbreak"))
            doDebugBreaks = true;
        else if(!strcmp(argv[i], "-nopause"))
            pause = false;
        else
        {
            printf("Unkown command line option %s\n", argv[i]);
            return -1;
        }
    }
    if(argc > 1 && !strcmp(argv[1], "-debugbreak"))
    {
        doDebugBreaks = true;
    }
    gep::UnittestManager::instance().setDoDebugBreaks(doDebugBreaks);
    int numFailedTests = gep::UnittestManager::instance().runAllTests();
    gep::UnittestManager::instance().destoryGlobalInstance();

	gep::destroy(); //Shutdown gep

    if(pause)
        system("pause");
    return numFailedTests;
}

