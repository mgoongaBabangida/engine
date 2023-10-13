#include "stdafx.h"
#include "TextRender.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glew-2.1.0\include\GL\glew.h>

#include <glm/glm/mat4x4.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>

#include <algorithm>

FT_Library ft;

//--------------------------------------------------------
eTextRender::eTextRender(const std::string& vS, const std::string& fS)
{
  textShader.installShaders(vS.c_str(), fS.c_str());

  if (FT_Init_FreeType(&ft))
  {
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
  }
  Font ARIALN, edwardian;
  ARIALN.Load("ARIALN", "../game_assets/fonts/ARIALN.ttf");
  edwardian.Load("edwardian", "../game_assets/fonts/edwardianscriptitc.ttf");
  m_fonts.push_back(ARIALN);
  m_fonts.push_back(edwardian);

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

//-------------------------------------------------------
void eTextRender::RenderText(const Camera& _camera, const std::vector<std::shared_ptr<Text>>& _texts, float _scr_width, float _scr_height)
{
  // activate corresponding render state
  glUseProgram(textShader.ID());
  for (auto _text : _texts)
  {
    textShader.SetUniformData("textColor", glm::vec4(_text->color,1.0f));
    textShader.SetUniformData("projection", _text->mvp);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);

    float x = _text->pos_x, y = _text->pos_y;
    // iterate through all characters
    std::string::const_iterator c;
    std::string content = _text->content;
    auto font = std::find_if(m_fonts.begin(), m_fonts.end(), [_text](const Font& _font) { return _font.name == _text->font; });
    if (font != m_fonts.end())
    {
      for (c = content.begin(); c != content.end(); c++)
      {
        Character ch = font->characters[*c];

        float xpos = x + ch.Bearing.x * _text->scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * _text->scale;

        float w = ch.Size.x * _text->scale;
        float h = ch.Size.y * _text->scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * _text->scale; // bitshift by 6 to get value in pixels (2^6 = 64)
      }
    }
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

//-------------------------------------------------------
eTextRender::~eTextRender()
{
  for(auto& font : m_fonts)
    font.Unload();
  glDeleteVertexArrays(1,&VAO);
  glDeleteBuffers(1,&VBO);
  FT_Done_FreeType(ft);
}

//-------------------------------------------------------
void Font::Load(const std::string& _name, const std::string& _path)
{
  if (!_name.empty())
    name = _name;

  if (!_path.empty())
    path = _path;

  if (FT_New_Face(ft, path.c_str(), 0, &face))
  {
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
  }
  FT_Set_Pixel_Sizes(face, 0, 48);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

  for (unsigned char c = 0; c < 128; c++)
  {
    // load character glyph 
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    {
      std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
      continue;
    }

    // generate texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RED,
      face->glyph->bitmap.width,
      face->glyph->bitmap.rows,
      0,
      GL_RED,
      GL_UNSIGNED_BYTE,
      face->glyph->bitmap.buffer
    );
    // set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // now store character for later use
    Character character = {
        texture,
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        face->glyph->advance.x
    };
    characters.insert(std::pair<char, Character>(c, character));
  }
  loaded = true;
}

//-------------------------------------------------------
void Font::Unload()
{
  for (auto& c : characters)
    glDeleteTextures(1, &c.second.TextureID);
  FT_Done_Face(face);
}
