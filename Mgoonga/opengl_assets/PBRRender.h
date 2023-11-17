#pragma once

#include "Shader.h"
#include <base/Object.h>

class Camera;

//----------------------------------------------------
class ePBRRender
{
public:
  ePBRRender(const std::string& vS, const std::string& fS);

  void Render(const Camera& camera,
              const Light& _light,
              std::vector<shObject>& objects);

  Shader& GetShader() { return pbrShader; }

  void SetShadowCascadeLevels(const std::vector<float>& _scl) { m_shadowCascadeLevels = _scl; }
protected:
  Shader pbrShader;

  void _SetMaterial(shObject _obj);

  std::vector<glm::mat4> matrices;

  GLuint albedoLoc;
  GLuint metallicLoc;
  GLuint roughnessLoc;
  GLuint aoLoc;
  GLuint camPosLoc;

  GLuint BonesMatLocation;
  GLuint fullTransformationUniformLocation;
  GLuint modelToWorldMatrixUniformLocation;
  GLuint shadowMatrixUniformLocation;

  GLuint LightingIndexDirectional;
  GLuint LightingIndexPoint;
  GLuint LightingIndexSpot;

  glm::mat4 modelToProjectionMatrix;
  glm::mat4	 shadowMatrix;

  std::vector<float> m_shadowCascadeLevels;
};
