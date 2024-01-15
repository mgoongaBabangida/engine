#pragma once
#include "opengl_assets.h"

#include "Shader.h"
#include <base/base.h>
#include <math/Camera.h>

//------------------------------------------
class eComputeShaderRender
{
public:
  eComputeShaderRender(const std::string& cS, const Texture* _computeShaderImage);
  void Render(const Camera& _camera);

  Shader& GetShader() { return mComputeShader; }
protected:
  Shader mComputeShader;
  GLuint mImageId;
  GLuint mImageWidth;
  GLuint mImageHeight;
};