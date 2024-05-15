#include "stdafx.h"

#include "VolumetricRender.h"
#include <math/Camera.h>

//-------------------------------------------------------------------------------------------------------------
eVolumetricRender::eVolumetricRender(const std::string& _vS, const std::string& _fS)
{
  m_volumetric_shader.installShaders(_vS.c_str(), _fS.c_str());
  glUseProgram(m_volumetric_shader.ID());
  m_volumetric_shader.GetUniformInfoFromShader();
}

//-------------------------------------------------------------------------------------------------------------
void eVolumetricRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
  glUseProgram(m_volumetric_shader.ID());

  m_volumetric_shader.SetUniformData("viewDir", glm::vec4(_camera.getDirection(), 1.0f));
  glm::mat4 worldToProjectionMatrix = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
  m_volumetric_shader.SetUniformData("MVP", worldToProjectionMatrix);

  for (auto& object : _objects)
  {
    object->GetModel()->Draw();
  }
}