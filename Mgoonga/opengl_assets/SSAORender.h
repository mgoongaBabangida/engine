#pragma once
#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include "ScreenMesh.h"

class eSSAORender
{
public:
  eSSAORender(const std::string& _vSG, const std::string& _fSG,
              const std::string& _vS, const std::string& _fS, const std::string& _fSblur);
  ~eSSAORender();

  void RenderGeometry(const Camera& camera, const Light& light, const std::vector<shObject>& objects);
  void RenderSSAO(const Camera& camera);
  void RenderSSAOBlur(const Camera& camera);

  Shader& GetShader() { return m_main_shader; }
protected:
  Shader m_geometry_shader;
  Shader m_main_shader;
  Shader m_blur_shader;

  std::unique_ptr<eScreenMesh>	screenMesh;

  std::vector<glm::mat4> matrices;

  std::vector<glm::vec3> m_ssao_kernel;
  unsigned int noiseTexture;
};