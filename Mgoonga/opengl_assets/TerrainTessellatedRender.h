#pragma once

#include "opengl_assets.h"

#include "Shader.h"
#include <base/Object.h>

class Camera;

//------------------------------------
class eTerrainTessellatedRender
{
public:
  eTerrainTessellatedRender(const std::string& vS, const std::string& fS, const std::string& tS1, const std::string& tS2);
  ~eTerrainTessellatedRender();

  void Render(const Camera& _camera, const Light& light, const std::vector<shObject>& objects);
  void UpdateMeshUniforms(const TessellationRenderingInfo& _info);

  Shader& GetShader() { return m_tessellation_shader; }
protected:
  Shader m_tessellation_shader;
};