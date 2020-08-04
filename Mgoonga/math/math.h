#pragma once

#ifdef DLLDIR_EX
#define DLL_MATH __declspec(dllexport)
#else
#define DLL_MATH __declspec(dllimport)
#endif

#pragma warning( disable : 4251)