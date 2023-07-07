#pragma once
#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>

class CubeMesh;

class eIBLRender
{
public:
  eIBLRender(const std::string& _vS, const std::string& _fS, const std::string& _irrfS, const Texture* _hdr);
  ~eIBLRender();

  void RenderCubemap(const Camera& camera, GLuint _cubemap_id);
  void RenderIBLMap(const Camera& camera, GLuint _irr_id);
  Texture GetHdrTexture() const { return *m_hdr; }

  Shader& GetShader() { return m_cubemap_shader; }

protected:
  Shader m_cubemap_shader;
  Shader m_irr_shader;
  std::unique_ptr<CubeMesh> m_cube;
  const Texture* m_hdr = nullptr;
};