#include "Windows_Related_api.h"

//-----------------------------------------------------
bool HasCurrentOpenGLContext()
{
  HGLRC currentContext = wglGetCurrentContext();
  return currentContext != NULL;
}