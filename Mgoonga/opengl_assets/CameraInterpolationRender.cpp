#include "CameraInterpolationRender.h"

//---------------------------------------------------------------
eCameraInterpolationRender::eCameraInterpolationRender(const std::string& vS, const std::string& fS, const std::string& fS2)
{
  mShaderCoords.installShaders(vS.c_str(), fS.c_str());

  mShaderApplyCoords.installShaders(vS.c_str(), fS2.c_str());
  mShaderApplyCoords.GetUniformInfoFromShader();

  screenMesh.reset(new eScreenMesh({}, {}));
  screenMesh->SetViewPortToDefault();
}

//---------------------------------------------------------------
void eCameraInterpolationRender::Render(const Camera& _camera)
{
  glUseProgram(mShaderCoords.ID());

  Camera secondCamera(_camera);
  glm::vec3 focusPoint = _camera.getPosition() + (glm::normalize(_camera.getDirection()) * 3.f);
  glm::vec3 right = glm::cross(glm::normalize(_camera.getDirection()) , glm::vec3(0.f, 1.f, 0.f));
  right = right * displacement;
  seconCameraPosition = _camera.getPosition() + right;
  secondCamera.setPosition(seconCameraPosition);
  secondCamera.setDirection(focusPoint - secondCamera.getPosition());

  mShaderCoords.SetUniformData("proj", secondCamera.getProjectionMatrix());
  mShaderCoords.SetUniformData("view", secondCamera.getWorldToViewMatrix());

  mShaderCoords.SetUniformData("invProj", glm::inverse(_camera.getProjectionMatrix()));
  mShaderCoords.SetUniformData("invView", glm::inverse(_camera.getWorldToViewMatrix()));

  screenMesh->DrawUnTextured();
}

//---------------------------------------------------------------
void eCameraInterpolationRender::RenderApply(const Camera& _camera, float _textureWidth, float _textureHeight)
{
  glUseProgram(mShaderApplyCoords.ID());
  screenMesh->DrawUnTextured();
}
