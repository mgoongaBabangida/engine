#include "CameraInterpolationRender.h"

//---------------------------------------------------------------
eCameraInterpolationRender::eCameraInterpolationRender(const std::string& vS, const std::string& fS,
  const std::string& cS, const Texture* _computeShaderImage)
{
  mShaderCoords.installShaders(vS.c_str(), fS.c_str());
  mShaderCoords.GetUniformInfoFromShader();
  screenMesh.reset(new eScreenMesh({}, {}));
  screenMesh->SetViewPortToDefault();

  mComputeShader.installShaders(cS.c_str());
  mComputeShader.GetUniformInfoFromShader();

  mImageId = _computeShaderImage->m_id;
  mImageWidth = _computeShaderImage->m_width;
  mImageHeight = _computeShaderImage->m_height;
}

//---------------------------------------------------------------
void eCameraInterpolationRender::Render(const Camera& _camera)
{
  glUseProgram(mShaderCoords.ID());

  Camera secondCamera(_camera);
  glm::vec3 focusPoint = _camera.getPosition() + (glm::normalize(_camera.getDirection()) * 3.f);
  glm::vec3 right = glm::cross(glm::normalize(_camera.getDirection()) , glm::vec3(0.f, 1.f, 0.f));
  right = right * displacement;
  secondCameraPosition = _camera.getPosition() + right;
  secondCamera.setPosition(secondCameraPosition);
  secondCamera.setDirection(focusPoint - secondCamera.getPosition());

  mShaderCoords.SetUniformData("proj", secondCamera.getProjectionMatrix());
  mShaderCoords.SetUniformData("view", secondCamera.getWorldToViewMatrix());

  mShaderCoords.SetUniformData("invProj", glm::inverse(_camera.getProjectionMatrix()));
  mShaderCoords.SetUniformData("invView", glm::inverse(_camera.getWorldToViewMatrix()));

  lookAt_matrix = _camera.getWorldToViewMatrix();
  lookAt_matrix_projected = secondCamera.getWorldToViewMatrix();
  projection_matrix = _camera.getProjectionMatrix();

  screenMesh->DrawUnTextured();
}

//-------------------------------------------------------------------
void eCameraInterpolationRender::DispatchCompute(const Camera& _camera)
{
  glUseProgram(mComputeShader.ID());

  glActiveTexture(GL_TEXTURE0);
  glBindImageTexture(0, mImageId, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

  Camera secondCamera(_camera);
  glm::vec3 focusPoint = _camera.getPosition() + (glm::normalize(_camera.getDirection()) * 3.f);
  glm::vec3 right = glm::cross(glm::normalize(_camera.getDirection()), glm::vec3(0.f, 1.f, 0.f));
  right = right * displacement;
  secondCameraPosition = _camera.getPosition() + right;
  secondCamera.setPosition(secondCameraPosition);
  secondCamera.setDirection(focusPoint - secondCamera.getPosition());

  mComputeShader.SetUniformData("proj", secondCamera.getProjectionMatrix());
  mComputeShader.SetUniformData("view", secondCamera.getWorldToViewMatrix());

  mComputeShader.SetUniformData("invProj", glm::inverse(_camera.getProjectionMatrix()));
  mComputeShader.SetUniformData("invView", glm::inverse(_camera.getWorldToViewMatrix()));

  lookAt_matrix = _camera.getWorldToViewMatrix();
  lookAt_matrix_projected = secondCamera.getWorldToViewMatrix();
  projection_matrix = _camera.getProjectionMatrix();

  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
  glDispatchCompute((unsigned int)mImageWidth, (unsigned int)mImageHeight, 1);
  // make sure writing to image has finished before read
  glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_TEXTURE_FETCH_BARRIER_BIT);
}