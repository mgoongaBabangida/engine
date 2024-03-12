#pragma once
#include "opengl_assets.h"

#include "Shader.h"
#include <base/base.h>
#include <math/Camera.h>
#include "ScreenMesh.h"

//---------------------------------------------------------------------------
class eScreenSpaceReflectionRender
{
public:
  eScreenSpaceReflectionRender(const std::string& vS, const std::string& fS, const std::string& fSblur);
  ~eScreenSpaceReflectionRender();
  void Render(const Camera& _camera);
  void RenderSSRBlur(const Camera& _camera);

  Shader& GetShader() { return mShaderSSR; }

  float& Step() {
    return step;
  }
  float& MinRayStep() {
    return minRayStep;
  }
  float& MaxSteps() {
    return maxSteps;
  }
  int& NumBinarySearchSteps() {
    return numBinarySearchSteps;
  }
  float& ReflectionSpecularFalloffExponent() {
    return reflectionSpecularFalloffExponent;
  }

  float& Metallic() {
    return metallic;
  }
  float& Spec() {
    return spec;
  }
  glm::vec4& Scale() {
    return scale;
  }
  float& K() {
    return k;
  }

protected:
  Shader mShaderSSR;
  Shader mShaderBlur;

  float step = 0.1f;
  float minRayStep = 0.1f;
  float maxSteps = 30.;
  int numBinarySearchSteps = 5;
  float reflectionSpecularFalloffExponent = 2.0f;

  float metallic = 0.5f;
  float spec = 0.2f;
  glm::vec4 scale = glm::vec4(.8, .8, .8, 1.);
  float k = 19.19f;

  std::unique_ptr<eScreenMesh>	screenMesh;
};