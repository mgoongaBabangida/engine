#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/Object.h>
#include <base/base.h>

//---------------------------------------------------------------
class eCSMRender
{
public:
  eCSMRender(const std::string& vS, const std::string& fS, const std::string& gS);
  ~eCSMRender();

  void Render(const Camera& camera,
              const Light& light,
              const std::vector<shObject>& objects);

  Shader& GetShader() { return csmShader; }

  float& ZMult() { return zMult; }
  std::vector<float> GetCascadeFlaneDistances() const;

protected:
  std::vector<glm::mat4> _getLightSpaceMatrices(const Camera& camera, const Light& light);
  glm::mat4              _getLightSpaceMatrix(const Camera& camera, const Light& light, float _cameraNearPlane, float _cameraFarPlane);

  Shader csmShader;
  std::vector<glm::mat4> bone_matrices;

  std::vector<float> m_shadowCascadeLevels;

  unsigned int matricesUBO;

  GLuint			ModelLocationDir;
  GLuint			BonesMatLocationDir;
  float       zMult = 5.0f;
};
