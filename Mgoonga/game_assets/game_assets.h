#pragma once

#ifdef DLLDIR_EX
#define DLL_GAME_ASSETS __declspec(dllexport)
#else
#define DLL_GAME_ASSETS __declspec(dllimport)
#endif

#pragma warning( disable : 4251)

