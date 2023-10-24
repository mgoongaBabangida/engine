#include "stdafx.h"
#include "BloomRender.h"

//---------------------------------------------------------------------------
eBloomRenderer::eBloomRenderer(const std::string& _vS, const std::string& _fSDown, const std::string& _fSUp)
{
  // Shaders
  mDownsampleShader.installShaders(_vS.c_str(), _fSDown.c_str());
  mUpsampleShader.installShaders(_vS.c_str(), _fSUp.c_str());
  screenMesh.reset(new eScreenMesh({}, {}));
}

//---------------------------------------------------------------------------
eBloomRenderer::~eBloomRenderer()
{

}


//---------------------------------------------------------------------------
void eBloomRenderer::RenderDownsamples(const std::vector<bloomMip>& _mipChain, const glm::vec2& _srcViewportSizeFloat)
{
  glUseProgram(mDownsampleShader.ID());

  mDownsampleShader.SetUniformData("srcResolution", _srcViewportSizeFloat);

  // Progressively downsample through the mip chain
  for (int i = 0; i < _mipChain.size(); ++i)
  {
    const bloomMip& mip = _mipChain[i];
    glViewport(0, 0, mip.intSize.x, mip.intSize.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, mip.texture_id, 0);

    // Render screen-filled quad of resolution of current mip
    screenMesh->DrawUnTextured();

    // Set current mip resolution as srcResolution for next iteration
    mDownsampleShader.SetUniformData("srcResolution", mip.size);
    // Set current mip as texture input for next iteration
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mip.texture_id);
  }
}

//---------------------------------------------------------------------------
void eBloomRenderer::RenderUpsamples(const std::vector<bloomMip>& _mipChain)
{
  glUseProgram(mUpsampleShader.ID());
  mUpsampleShader.SetUniformData("filterRadius", m_filterRadius);

  for (int i = (int)_mipChain.size() - 1; i > 0; --i)
  {
    const bloomMip& mip = _mipChain[i];
    const bloomMip& nextMip = _mipChain[i - 1];

    // Bind viewport and texture from where to read
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mip.texture_id);

    // Set framebuffer render target (we write to this texture)
    glViewport(0, 0, nextMip.intSize.x, nextMip.intSize.y);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
      GL_TEXTURE_2D, nextMip.texture_id, 0);

    // Render screen-filled quad of resolution of current mip
    screenMesh->DrawUnTextured();
  }
}
