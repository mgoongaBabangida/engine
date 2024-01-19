#pragma once
#include "opengl_assets.h"

#include "Shader.h"
#include <base/base.h>
#include <math/Camera.h>
#include "ScreenMesh.h"

//------------------------------------------
class eCameraInterpolationRender
{
public:
  eCameraInterpolationRender(const std::string& vS, const std::string& fS,
    const std::string& cS, const Texture* _computeShaderImage);
  void Render(const Camera& _camera);
  void DispatchCompute(const Camera& _camera);

  Shader& GetShader() { return mComputeShader; }

  glm::vec3& GetSecondCameraPositionRef() { return secondCameraPosition; }
  float& GetDisplacementRef() { return displacement; }

protected:
  Shader mShaderCoords;

  Shader mComputeShader;
  GLuint mImageId;
  GLuint mImageWidth;
  GLuint mImageHeight;

  std::unique_ptr<eScreenMesh>	screenMesh;

  glm::vec3 secondCameraPosition;
  float displacement = 1.0f;
};