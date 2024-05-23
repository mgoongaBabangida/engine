#include "stdafx.h"

#include "VolumetricRender.h"
#include <math/Camera.h>

//-------------------------------------------------------------------------------------------------------------
eVolumetricRender::eVolumetricRender(const std::string& _vS, const std::string& _fS, GLuint _worlyID)
{
  m_volumetric_shader.installShaders(_vS.c_str(), _fS.c_str());
  glUseProgram(m_volumetric_shader.ID());
  m_volumetric_shader.GetUniformInfoFromShader();
  mWorleyNoiseImage3DID = _worlyID;
  m_clock.start();
}

//-------------------------------------------------------------------------------------------------------------
void eVolumetricRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
  glUseProgram(m_volumetric_shader.ID());

  m_volumetric_shader.SetUniformData("viewDir", glm::vec4(_camera.getDirection(), 1.0f));
  m_volumetric_shader.SetUniformData("lightDir", glm::vec4(_light.light_direction));
  m_volumetric_shader.SetUniformData("density", (float)m_density/1000.f);
  m_volumetric_shader.SetUniformData("absorption", (float)m_absorption / 1000.f);
  m_volumetric_shader.SetUniformData("perlinWeight", m_perlin);
  m_volumetric_shader.SetUniformData("time", (float)m_clock.timeEllapsedMsc()/1000.f);
  m_volumetric_shader.SetUniformData("perlinMotionScale", (float)m_perlin_motion / 100.f);
  m_volumetric_shader.SetUniformData("worleyMotionScale", (float)m_worley_motion / 100.f);
  m_volumetric_shader.SetUniformData("g", m_g);

  glm::mat4 worldToProjectionMatrix = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();

  m_volumetric_shader.SetUniformData("cloudColor", glm::vec4(m_color, 1.0f));
  // Bind the 3D texture
  GLint noiseTextureLocation = glGetUniformLocation(m_volumetric_shader.ID(), "noiseTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, mWorleyNoiseImage3DID);
  glUniform1i(noiseTextureLocation, 0);

  for (auto& object : _objects)
  {
    m_volumetric_shader.SetUniformData("MVP", worldToProjectionMatrix * object->GetTransform()->getModelMatrix());
    object->GetModel()->Draw();
  }
}