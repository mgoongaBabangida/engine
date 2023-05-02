#pragma once

#include "Shader.h"
#include <math/Camera.h>
#include <base/Object.h>
#include <base/base.h>

//---------------------------------------------------------------
class eMeshLineRender
{
public:
  eMeshLineRender(const std::string& vS, const std::string& fS);
  ~eMeshLineRender();

  void Render(const Camera& camera,
              const Light& light,
              const std::vector<shObject>& objects);

  Shader& GetShader() { return m_shader; }
protected:
  Shader m_shader;
  std::vector<glm::mat4> matrices;
};