#pragma once

#ifdef DLLDIR_EX
#define DLL_OPENGL_ASSETS __declspec(dllexport)
#else
#define DLL_OPENGL_ASSETS __declspec(dllimport)
#endif

#pragma warning( disable : 4251)
