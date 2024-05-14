#pragma once

#ifdef DLLDIR_EX
#define DLL_OPENGL_ASSETS __declspec(dllexport)
#else
#define DLL_OPENGL_ASSETS __declspec(dllimport)
#endif

#pragma warning( disable : 4251)

#include <glm\glm\glm.hpp>
#include <vector>

struct bloomMip
{
  glm::vec2 size;
  glm::ivec2 intSize;
  unsigned int texture_id;
};

struct TessellationRenderingInfo
{
	std::vector<float> base_start_heights;
	std::vector<float> texture_scale;
	float min_height = 0.0f;
	float max_height = 1.0f;
	float height_scale = 1.0f;
};

