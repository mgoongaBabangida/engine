#pragma once
#include "opengl_assets.h"

#include <vector>

#include "Texture.h"

//----------------------------------------------------------------------------------------
class BloomFBO
{
public:
  BloomFBO() = default;
  ~BloomFBO();

  bool Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength);

  GLuint    ID() { return m_fbo; }
  GLuint		Width() { return  m_width; }
  GLuint		Height() { return  m_height; }
  glm::ivec2 Size() { return { m_width , m_height }; }

  void BindForWriting();
  void BindForReading(GLenum TextureUnit)
  {
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, MipChain()[0].texture_id); // first level
  }

  const std::vector<bloomMip>& MipChain() const;

  Texture GetTexture()
  {
    return  Texture(MipChain()[0].texture_id, MipChain()[0].intSize.x, MipChain()[0].intSize.y, 3);
  }
private:
  bool mInit = false;
  GLuint    m_fbo = -1;
  GLuint		m_width;
  GLuint		m_height;
  std::vector<bloomMip> mMipChain;
};
