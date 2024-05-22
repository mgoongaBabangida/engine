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

protected:
  Shader m_volumetric_shader;
  GLuint mWorleyNoiseImage3DID;
  int32_t m_density = 15;
  glm::vec3 m_color = {10, 10, 10};
  int32_t m_absorption = 5;
  float m_perlin = 0.5f;
  int32_t m_perlin_motion = 10;
  int32_t m_worley_motion = 10;

  math::eClock		m_clock;;
};
