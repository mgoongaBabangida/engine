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

  m_tessellation_shader.SetUniformData("view", _camera.getWorldToViewMatrix());
  m_tessellation_shader.SetUniformData("projection", _camera.getProjectionMatrix());
  for (auto& object : _objects)
  {
    m_tessellation_shader.SetUniformData("model", object->GetTransform()->getModelMatrix());
    object->GetModel()->Draw();
  }
}
