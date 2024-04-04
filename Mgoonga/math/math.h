#pragma once
#include "stdafx.h"

#ifdef DLLDIR_EX
#define DLL_MATH __declspec(dllexport)
#else
#define DLL_MATH __declspec(dllimport)
#endif

#pragma warning( disable : 4251)
#pragma warning( disable : 4251) // vector & unique_ptr have to be exported or not used
#pragma warning( disable : 4273) // inconsistent dll-linkage