#pragma once
#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include "ScreenMesh.h"

class CubeMesh;

class eIBLRender
{
public:
  eIBLRender(const std::string& _vS, const std::string& _fS,
    const std::string& _irrfS, const std::string& _prefilterfS,
    const std::string& _brdfvS, const std::string& _brdffS,
    const Texture* _hdr);
  ~eIBLRender();

  void RenderCubemap(const Camera& camera, GLuint _cubemap_id);
  void RenderIBLMap(const Camera& camera, GLuint _irr_id);
  void RenderPrefilterMap(const Camera& camera, GLuint _cubemap_id, GLuint _rbo_id);
  void RenderBrdf();

  Texture GetHdrTexture() const { return *m_hdr; }
  GLuint GetLUTTextureID() const { return brdfLUTTexture;  }

  Shader& GetShader() { return m_cubemap_shader; }

protected:
  Shader m_cubemap_shader;
  Shader m_irr_shader;
  Shader m_prefilter_shader;
  Shader m_brdf_shader;

  std::unique_ptr<eScreenMesh>	screenMesh;
  std::unique_ptr<CubeMesh> m_cube;
  const Texture* m_hdr = nullptr;
  glm::mat4 m_captureViews[6];
  GLuint brdfLUTTexture;
};