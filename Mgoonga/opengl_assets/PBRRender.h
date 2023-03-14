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
protected:
  Shader pbrShader;

  std::vector<glm::mat4> matrices;

  GLuint albedoLoc;
  GLuint metallicLoc;
  GLuint roughnessLoc;
  GLuint aoLoc;
  GLuint lightPositionsLoc;
  GLuint lightColorsLoc;
  GLuint camPosLoc;

  GLuint fullTransformationUniformLocation;
  GLuint modelToWorldMatrixUniformLocation;
  GLuint shadowMatrixUniformLocation;
};
