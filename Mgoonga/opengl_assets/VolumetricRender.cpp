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

  m_volumetric_shader.SetUniformData("cameraPos", glm::vec4(_camera.getPosition(), 1.0f));
  m_volumetric_shader.SetUniformData("lightDir", glm::vec4(_light.light_direction));
  m_volumetric_shader.SetUniformData("density", (float)m_density/1000.f);
  m_volumetric_shader.SetUniformData("absorption", (float)m_absorption / 1000.f);
  m_volumetric_shader.SetUniformData("perlinWeight", m_perlin);
  m_volumetric_shader.SetUniformData("time", (float)m_clock.timeElapsedMsc()/1000.f);
  m_volumetric_shader.SetUniformData("perlinMotionScale", (float)m_perlin_motion / 100.f);
  m_volumetric_shader.SetUniformData("worleyMotionScale", (float)m_worley_motion / 100.f);
  m_volumetric_shader.SetUniformData("g", m_g);
  m_volumetric_shader.SetUniformData("noiseScale", glm::vec4(m_noise_scale, 1.0f));
  m_volumetric_shader.SetUniformData("apply_powder", m_apply_powder);
  m_volumetric_shader.SetUniformData("silver_lining", m_silver_lining);
  m_volumetric_shader.SetUniformData("silver_lining_density", m_silver_lining_density);
  m_volumetric_shader.SetUniformData("silver_lining_strength", m_silver_lining_strength);
  m_volumetric_shader.SetUniformData("alphaThreshold", m_alpha_threshold);

  glm::mat4 worldToProjectionMatrix = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();

  // Scale matrix
  glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
  glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
  // Combined transformation matrix from [-1,1] to [0, 1] space
  glm::mat4 localToNoiseSpace = translation * scale;
  m_volumetric_shader.SetUniformData("localToNoiseSpace", localToNoiseSpace);

  m_volumetric_shader.SetUniformData("cloudColor", glm::vec4(m_color, 1.0f));
  // Bind the 3D texture
  GLint noiseTextureLocation = glGetUniformLocation(m_volumetric_shader.ID(), "noiseTexture");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_3D, mWorleyNoiseImage3DID);
  glUniform1i(noiseTextureLocation, 0);

  for (auto& object : _objects)
  {
    m_volumetric_shader.SetUniformData("MVP", worldToProjectionMatrix * object->GetTransform()->getModelMatrix());
    m_volumetric_shader.SetUniformData("modelMatrix", object->GetTransform()->getModelMatrix());
    object->GetModel()->Draw();
  }
}