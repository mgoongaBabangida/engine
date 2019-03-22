#pragma once

// glew.h -- Should be first included header
#include <GL/glew.h>

// glm headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/norm.hpp>

// assimp headers
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// DevIL headers
#include <IL/IL.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

// AL headers
#include <AL/al.h>
#include <AL/alc.h>

// SDL2 headers
#include <SDL.h>
#include <SDL_opengl.h>

// SDL2_image headers
#include <SDL_image.h>

// spdlog headers
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// std library headers
#include <map>
#include <set>
#include <vector>
#include <thread>
#include <future>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <random>
#include <algorithm>
#include <memory>
#include <exception>
#include <type_traits>
#include <functional>

#include <stdio.h>
#include <assert.h>

// windows headers
#include <Windows.h>