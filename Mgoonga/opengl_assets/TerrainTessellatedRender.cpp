#include "stdafx.h"
#include "TerrainTessellatedRender.h"
#include <math/Camera.h>

//------------------------------------------------------------------------------------------------------------------------------------------------
eTerrainTessellatedRender::eTerrainTessellatedRender(const std::string& vS, const std::string& fS, const std::string& tS1, const std::string& tS2)
{
  m_tessellation_shader.installShaders(vS.c_str(), fS.c_str(), tS1.c_str(), tS2.c_str());
  glUseProgram(m_tessellation_shader.ID());
  m_tessellation_shader.GetUniformInfoFromShader();
}

//------------------------------------------------------------------------------------------------------------------------------------------------
eTerrainTessellatedRender::~eTerrainTessellatedRender()
{
}

//------------------------------------------------------------------------------------------------------------------------------------------------
void eTerrainTessellatedRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
  glUseProgram(m_tessellation_shader.ID());

  m_tessellation_shader.SetUniformData("light.ambient", _light.ambient);
  m_tessellation_shader.SetUniformData("light.diffuse", _light.diffuse);
  m_tessellation_shader.SetUniformData("light.specular", _light.specular);
  m_tessellation_shader.SetUniformData("light.position", _light.light_position);
  m_tessellation_shader.SetUniformData("light.direction", _light.light_direction);

  m_tessellation_shader.SetUniformData("light.constant", _light.constant);
  m_tessellation_shader.SetUniformData("light.linear", _light.linear);
  m_tessellation_shader.SetUniformData("light.quadratic", _light.quadratic);
  m_tessellation_shader.SetUniformData("light.cutOff", _light.cutOff);
  m_tessellation_shader.SetUniformData("light.outerCutOff", _light.outerCutOff);

  m_tessellation_shader.SetUniformData("view", _camera.getWorldToViewMatrix());
  m_tessellation_shader.SetUniformData("projection", _camera.getProjectionMatrix());
  m_tessellation_shader.SetUniformData("eyePositionWorld", glm::vec4(_camera.getPosition(), 1.0f));
  for (auto& object : _objects)
  {
    m_tessellation_shader.SetUniformData("model", object->GetTransform()->getModelMatrix());
    object->GetModel()->Draw();
  }
}
