#pragma once
#include "opengl_assets.h"

#include "Shader.h"
#include <math/Camera.h>
#include <base/base.h>
#include "ScreenMesh.h"

//---------------------------------------------------------------------------
class eBloomRenderer
{
public:
  eBloomRenderer(const std::string& vS, const std::string& fSDown, const std::string& fSUp);
  ~eBloomRenderer();

  void RenderDownsamples(const std::vector<bloomMip>& _mipChain, const glm::vec2& _srcViewportSizeFloat);
  void RenderUpsamples(const std::vector<bloomMip>& _mipChain);

  float& FilterRadius() { return m_filterRadius; }

  Shader& GetShader() { return mDownsampleShader; }

protected:
  Shader mDownsampleShader;
  Shader mUpsampleShader;

  std::unique_ptr<eScreenMesh>	screenMesh;
  float m_filterRadius = 0.005f;
};
