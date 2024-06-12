#include "stdafx.h"

#include "bloomFBO.h"
#include <base/Log.h>

//----------------------------------------------------------------------------------------
BloomFBO::~BloomFBO()
{
  for (int i = 0; i < mMipChain.size(); i++) {
    glDeleteTextures(1, &mMipChain[i].texture_id);
    mMipChain[i].texture_id = 0;
  }
  glDeleteFramebuffers(1, &m_fbo);
  m_fbo = 0;
  mInit = false;
}

//----------------------------------------------------------------------------------------
bool BloomFBO::Init(unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength)
{
  if (mInit)
   return true;

  m_width = windowWidth;
  m_height = windowHeight;

  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  glm::vec2 mipSize((float)windowWidth, (float)windowHeight);
  glm::ivec2 mipIntSize((int)windowWidth, (int)windowHeight);

  // Safety check
  if (windowWidth > (unsigned int)INT_MAX || windowHeight > (unsigned int)INT_MAX) {
    base::Log("Window size conversion overflow - cannot build bloom FBO!");
    return false;
  }

  for (unsigned int i = 0; i < mipChainLength; ++i)
  {
    bloomMip mip;

    mipSize *= 0.5f;
    mipIntSize /= 2;
    mip.size = mipSize;
    mip.intSize = mipIntSize;

    glGenTextures(1, &mip.texture_id);
    glBindTexture(GL_TEXTURE_2D, mip.texture_id);
    // we are downscaling an HDR color buffer, so we need a float texture format
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
      mip.intSize.x, mip.intSize.y,
      0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    mMipChain.emplace_back(mip);
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D, mMipChain[0].texture_id, 0);

  // setup attachments
  unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
  glDrawBuffers(1, attachments);

  // check completion status
  int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (status != GL_FRAMEBUFFER_COMPLETE)
  {
    base::Log("Bloom Framebuffer not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return false;
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  mInit = true;
  return true;
}

//----------------------------------------------------------------------------------------
void BloomFBO::BindForWriting()
{
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
}

//----------------------------------------------------------------------------------------
const std::vector<bloomMip>& BloomFBO::MipChain() const
{
  return mMipChain;
}
