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

//-----------------------------------------------------------------------------------------------------------------------
void eTerrainTessellatedRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
  glUseProgram(m_tessellation_shader.ID());

  m_tessellation_shader.SetUniformData("lights[0].ambient", _light.ambient);
  m_tessellation_shader.SetUniformData("lights[0].diffuse", _light.diffuse);
  m_tessellation_shader.SetUniformData("lights[0].specular", _light.specular);
  m_tessellation_shader.SetUniformData("lights[0].position", _light.light_position);
  m_tessellation_shader.SetUniformData("lights[0].direction", _light.light_direction);

  m_tessellation_shader.SetUniformData("lights[0].constant", _light.constant);
  m_tessellation_shader.SetUniformData("lights[0].linear", _light.linear);
  m_tessellation_shader.SetUniformData("lights[0].quadratic", _light.quadratic);
  m_tessellation_shader.SetUniformData("lights[0].cutOff", _light.cutOff);
  m_tessellation_shader.SetUniformData("lights[0].outerCutOff", _light.outerCutOff);

  m_tessellation_shader.SetUniformData("view", _camera.getWorldToViewMatrix());
  m_tessellation_shader.SetUniformData("projection", _camera.getProjectionMatrix());
  m_tessellation_shader.SetUniformData("eyePositionWorld", glm::vec4(_camera.getPosition(), 1.0f));

  for (auto& object : _objects)
  {
    m_tessellation_shader.SetUniformData("model", object->GetTransform()->getModelMatrix());
    object->GetModel()->Draw();
  }
}

//---------------------------------------------------
void eTerrainTessellatedRender::UpdateMeshUniforms(const TessellationRenderingInfo& _info)
{
  int counter = 0;
  for (const auto& height : _info.base_start_heights)
  {
    m_tessellation_shader.SetUniformData("base_start_heights[" + std::to_string(counter) + "]", height);
    ++counter;
  }
  counter = 0;
  for (const auto& texture_scale : _info.texture_scale)
  {
    m_tessellation_shader.SetUniformData("textureScale[" + std::to_string(counter) + "]", texture_scale);
    ++counter;
  }
  m_tessellation_shader.SetUniformData("base_start_heights[" + std::to_string(counter) + "]", 1.0f);
  m_tessellation_shader.SetUniformData("min_height",_info.min_height);
  m_tessellation_shader.SetUniformData("max_height",_info.height_scale);
  m_tessellation_shader.SetUniformData("height_scale",_info.height_scale);
}
