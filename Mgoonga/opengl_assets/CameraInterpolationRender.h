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
  eCameraInterpolationRender(const std::string& vS, const std::string& fS, const std::string& fS2);
  void Render(const Camera& _camera);
  void RenderApply(const Camera& _camera, float _textureWidth, float _textureHeight);

  Shader& GetShader() { return mShaderCoords; }

  glm::vec3& GetSecondCameraPositionRef() { return seconCameraPosition; }
  float& GetDisplacementRef() { return displacement; }

protected:
  Shader mShaderCoords;
  Shader mShaderApplyCoords;

  std::unique_ptr<eScreenMesh>	screenMesh;

  glm::vec3 seconCameraPosition;
  float displacement = 1.0f;
};