#pragma once

#include "opengl_assets.h"

#include "Shader.h"
#include <base/Object.h>
#include <math/Clock.h>

class Camera;

//------------------------------------------------
class eVolumetricRender
{
public:
  eVolumetricRender(const std::string& vS, const std::string& fS, GLuint _worlyID);
  void Render(const Camera& _camera, const Light& light, const std::vector<shObject>& objects);
  
  Shader& GetShader() { return m_volumetric_shader; }

  int32_t& GetCloudDensity() { return m_density; }
  int32_t& GetCloudAbsorption() { return m_absorption; }
  glm::vec3& GetCloudColor() { return m_color; }
  float& GetPerilnWeight() { return m_perlin; }
  int32_t& GetCloudPerlinMotion() { return m_perlin_motion; }
  int32_t& GetCloudWorleyMotion() { return m_worley_motion; }
  float& GetGParam() { return m_g; }
  glm::vec3& GetNoiseScale() { return m_noise_scale; }
  bool& GetApplyPowder() { return m_apply_powder; }
  bool& GetFixedColor() { return m_fixed_color; }

protected:
  Shader m_volumetric_shader;
  GLuint mWorleyNoiseImage3DID;
  int32_t m_density = 10;
  glm::vec3 m_color = {10, 10, 10};
  int32_t m_absorption = 5;
  float m_perlin = 1.5f;
  int32_t m_perlin_motion = 10;
  int32_t m_worley_motion = 2;
  float m_g = 0.5f;
  glm::vec3 m_noise_scale = { 1., 1., 1. };
  bool m_apply_powder = true;
  bool m_fixed_color = false;
  math::eClock		m_clock;
};
