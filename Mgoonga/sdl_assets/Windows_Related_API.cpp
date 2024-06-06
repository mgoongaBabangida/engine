#include "Windows_Related_API.h"
#include <windows.h>

glm::ivec2 GetScreenSize()
{
  return glm::ivec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
}
