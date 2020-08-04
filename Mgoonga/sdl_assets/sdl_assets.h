#pragma once

#ifdef DLLDIR_EX
#define DLL_SDL_ASSETS __declspec(dllexport)
#else
#define DLL_SDL_ASSETS __declspec(dllimport)
#endif

#pragma warning( disable : 4251)
