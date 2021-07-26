#pragma once

#include "Shader.h"
#include <functional>
#include <map>
#include<glm/glm/vec2.hpp>
#include<glm/glm/vec3.hpp>

struct Character
{
  unsigned int TextureID;  // ID handle of the glyph texture
  glm::ivec2   Size;       // Size of glyph
  glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
  unsigned int Advance;    // Offset to advance to next glyph
};

//-----------------------------------------
class eTextRender
{
public:
  eTextRender(const std::string& vS, const std::string& fS);
  ~eTextRender();

  void RenderText(std::string text, float x, float y, float scale, glm::vec3 color, float scr_width, float scr_height);

protected:
  Shader							      textShader;
  unsigned int              VAO;
  unsigned int              VBO;
  GLuint							      textColorLoc;
  GLuint                    projectionMatrixLoc;
  std::map<char, Character> characters;
};
