#include "Windows_Related_api.h"
#include <windows.h>

//-----------------------------------------------------
bool HasCurrentOpenGLContext()
{
  HGLRC currentContext = wglGetCurrentContext();
  return currentContext != NULL;
}