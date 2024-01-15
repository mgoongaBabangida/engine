#include "ComputeShaderRender.h"
#include "Texture.h"

//--------------------------------------------------------
eComputeShaderRender::eComputeShaderRender(const std::string& cS, const Texture* _computeShaderImage)
{
  mComputeShader.installShaders(cS.c_str());
  mComputeShader.GetUniformInfoFromShader();

  mImageId = _computeShaderImage->id;
  mImageWidth = _computeShaderImage->mTextureWidth;
  mImageHeight = _computeShaderImage->mTextureHeight;
}

//-------------------------------------------------------
void eComputeShaderRender::Render(const Camera& _camera)
{
  glUseProgram(mComputeShader.ID());

  glActiveTexture(GL_TEXTURE0);
  glBindImageTexture(0, mImageId, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
  /*computeShader.setFloat("t", currentFrame);*/
  glDispatchCompute((unsigned int)mImageWidth / 10, (unsigned int)mImageHeight / 10, 1);

  // make sure writing to image has finished before read
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
