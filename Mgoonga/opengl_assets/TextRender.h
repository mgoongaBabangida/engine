#pragma once

#include "Shader.h"
#include <math/Camera.h>

#include <functional>
#include <map>

#include<glm/glm/vec2.hpp>
#include<glm/glm/vec3.hpp>

struct FT_FaceRec_;

struct Character
{
  unsigned int TextureID;  // ID handle of the glyph texture
  glm::ivec2   Size;       // Size of glyph
  glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
  unsigned int Advance;    // Offset to advance to next glyph
};

struct Font
{
  void Load(const std::string& = "", const std::string& = "");
  void Unload();

  FT_FaceRec_* face;
  std::string name;
  std::string path;
  std::map<char, Character> characters;
  bool loaded = false;
};

//-----------------------------------------
class eTextRender
{
public:
  eTextRender(const std::string& vS, const std::string& fS);
  ~eTextRender();

  void RenderText(const Camera& _camera,
                  const std::vector<std::shared_ptr<Text>>& _texts,
                  float scr_width,
                  float scr_height);

  Shader& GetShader() { return textShader; }
protected:
  Shader							      textShader;

  unsigned int              VAO;
  unsigned int              VBO;

  std::vector<Font>         m_fonts;
};
