#pragma once
/**
 * Holds all definitions neccessary for the Game Engine Programming DLL Module
 */

// prevent min & max macros
#define NOMINMAX

#ifdef GEP_EXPORTS
	#define GEP_API _declspec(dllexport)	
#else
	#define GEP_API _declspec(dllimport)
#endif

