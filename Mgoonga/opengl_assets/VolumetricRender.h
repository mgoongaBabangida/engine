#pragma once

#include "opengl_assets.h"

#include "Shader.h"
#include <base/Object.h>

class Camera;

//------------------------------------------------
class eVolumetricRender
{
public:
  eVolumetricRender(const std::string& vS, const std::string& fS, GLuint _worlyID);
  void Render(const Camera& _camera, const Light& light, const std::vector<shObject>& objects);
  
  Shader& GetShader() { return m_volumetric_shader; }
protected:
  Shader m_volumetric_shader;
  GLuint mWorleyNoiseImage3DID;
};
