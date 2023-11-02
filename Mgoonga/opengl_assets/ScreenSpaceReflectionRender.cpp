#include "ScreenSpaceReflectionRender.h"

//--------------------------------------------------------------
eScreenSpaceReflectionRender::eScreenSpaceReflectionRender(const std::string& _vS, const std::string& _fS, const std::string& _fSblur)
{
  mShaderSSR.installShaders(_vS.c_str(), _fS.c_str());

  mShaderBlur.installShaders(_vS.c_str(), _fSblur.c_str());
  mShaderBlur.GetUniformInfoFromShader();

  screenMesh.reset(new eScreenMesh({}, {}));
  screenMesh->SetViewPortToDefault();
}

//--------------------------------------------------------------
eScreenSpaceReflectionRender::~eScreenSpaceReflectionRender()
{
}

//--------------------------------------------------------------
void eScreenSpaceReflectionRender::Render(const Camera& _camera)
{
  glUseProgram(mShaderSSR.ID());

  mShaderSSR.SetUniformData("proj", _camera.getProjectionMatrix());
  mShaderSSR.SetUniformData("view", _camera.getWorldToViewMatrix());
  mShaderSSR.SetUniformData("invProj", glm ::inverse(_camera.getProjectionMatrix()));
  mShaderSSR.SetUniformData("invView", glm::inverse(_camera.getWorldToViewMatrix()));

  mShaderSSR.SetUniformData("step", step);
  mShaderSSR.SetUniformData("minRayStep", minRayStep);
  mShaderSSR.SetUniformData("maxSteps", maxSteps);
  mShaderSSR.SetUniformData("numBinarySearchSteps", numBinarySearchSteps);
  mShaderSSR.SetUniformData( "reflectionSpecularFalloffExponent", reflectionSpecularFalloffExponent);

  mShaderSSR.SetUniformData("Metallic", metallic);
  mShaderSSR.SetUniformData("spec", spec);
  mShaderSSR.SetUniformData("Scale", scale);
  mShaderSSR.SetUniformData("K", k);

  screenMesh->DrawUnTextured();
}

//--------------------------------------------------------------
void eScreenSpaceReflectionRender::RenderSSRBlur(const Camera& _camera)
{
  glUseProgram(mShaderBlur.ID());
  screenMesh->DrawUnTextured();
}
