#include "stdafx.h"
#include "GlPipelineState.h"

//------------------------------------------------
void eGlPipelineState::EnableDepthTest()
{
  glEnable(GL_DEPTH_TEST);
  m_depth_test = true;
}

//------------------------------------------------
void eGlPipelineState::DisableDepthTest()
{
  glDisable(GL_DEPTH_TEST);
  m_depth_test = false;
}

//------------------------------------------------
bool eGlPipelineState::IsDepthTestEnabled() const
{
  return m_depth_test;
}

//------------------------------------------------
void eGlPipelineState::EnableStencilTest()
{
  glEnable(GL_STENCIL_TEST);
  m_stencil_test = true;
}

//------------------------------------------------
void eGlPipelineState::DisableStencilTest()
{
  glDisable(GL_STENCIL_TEST);
  m_stencil_test = false;
}

//------------------------------------------------
bool eGlPipelineState::IsStencilTestEnabled() const
{
  return m_stencil_test;
}

//------------------------------------------------
void eGlPipelineState::EnableCullFace()
{
  glEnable(GL_CULL_FACE);
  m_cull_face = true;
}

//------------------------------------------------
void eGlPipelineState::DisableCullFace()
{
  glDisable(GL_CULL_FACE);
  m_cull_face = false;
}

//------------------------------------------------
bool eGlPipelineState::IsCullFaceEnabled() const
{
  return m_cull_face;
}

//------------------------------------------------
void eGlPipelineState::EnableMultisample()
{
  glEnable(GL_MULTISAMPLE);
  m_multisample = true;
}

//------------------------------------------------
void eGlPipelineState::DisableMultisample()
{
  glDisable(GL_MULTISAMPLE);
  m_multisample = false;
}

//------------------------------------------------
bool eGlPipelineState::IsMultisampleEnabled() const
{
  return m_multisample;
}

//------------------------------------------------
void eGlPipelineState::EnableLineSmooth()
{
  glEnable(GL_LINE_SMOOTH);
  m_line_smooth = true;
}

//------------------------------------------------
void eGlPipelineState::DisableLineSmooth()
{
  glDisable(GL_LINE_SMOOTH);
  m_line_smooth = false;
}

//------------------------------------------------
bool eGlPipelineState::IsLineSmoothEnabled() const
{
  return m_line_smooth;
}

//------------------------------------------------
void eGlPipelineState::EnableClipDistance0()
{
  glEnable(GL_CLIP_DISTANCE0);
  m_clip_distance_0 = true;
}

//------------------------------------------------
void eGlPipelineState::DisableClipDistance0()
{
  glDisable(GL_CLIP_DISTANCE0);
  m_clip_distance_0 = false;
}

//------------------------------------------------
bool eGlPipelineState::IsClipDistance0Enabled() const
{
  return m_clip_distance_0;
}

//------------------------------------------------
void eGlPipelineState::EnableBlend()
{
  glEnable(GL_BLEND);
  m_blend = true;
}

//------------------------------------------------
void eGlPipelineState::DisableBlend()
{
  glDisable(GL_BLEND);
  m_blend = false;
}

//------------------------------------------------
bool eGlPipelineState::IsBlendEnabled() const
{
  return m_blend;
}

//------------------------------------------------
void eGlPipelineState::EnableDepthClamp()
{
  glEnable(GL_DEPTH_CLAMP);
  m_depth_clamp = true;
}

//------------------------------------------------
void eGlPipelineState::DisableDepthClamp()
{
  glDisable(GL_DEPTH_CLAMP);
  m_depth_clamp = false;
}

//------------------------------------------------
bool eGlPipelineState::IsDepthClampEnabled() const
{
  return m_depth_clamp;
}

//------------------------------------------------
void eGlPipelineState::EnableTextureCubmapSeamless()
{
  glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  m_cubemap_seamless = true;
}

//------------------------------------------------
void eGlPipelineState::DisableTextureCubmapSeamless()
{
  glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
  m_cubemap_seamless = false;
}

//------------------------------------------------
bool eGlPipelineState::IsTextureCubmapSeamlessEnabled() const
{
  return m_cubemap_seamless;
}

//------------------------------------------------
void eGlPipelineState::EnableRasterizerDiscard()
{
  glEnable(GL_RASTERIZER_DISCARD);
  m_rasterizer_discard = true;
}

//------------------------------------------------
void eGlPipelineState::DisableRasterizerDiscard()
{
  glDisable(GL_RASTERIZER_DISCARD);
  m_rasterizer_discard = false;
}

//------------------------------------------------
bool eGlPipelineState::IsRasterizerDiscardEnabled() const
{
  return m_rasterizer_discard;
}

//------------------------------------------------
void eGlPipelineState::EnablePrimitiveRestart()
{
  glEnable(GL_PRIMITIVE_RESTART);
  m_primitive_restart = true;
}

//------------------------------------------------
void eGlPipelineState::DisablePrimitiveRestart()
{
  glDisable(GL_PRIMITIVE_RESTART);
  m_primitive_restart = false;
}

//------------------------------------------------
bool eGlPipelineState::IsPrimitiveRestartEnabled() const
{
  return m_primitive_restart;
}
