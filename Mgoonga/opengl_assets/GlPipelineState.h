#pragma once

//---------------------------------------------------------------------------
class eGlPipelineState
{
public:
	void EnableDepthTest();
	void DisableDepthTest();
	bool IsDepthTestEnabled() const;

	void EnableStencilTest();
	void DisableStencilTest();
	bool IsStencilTestEnabled() const;

	void EnableCullFace();
	void DisableCullFace();
	bool IsCullFaceEnabled() const;

	void EnableMultisample();
	void DisableMultisample();
	bool IsMultisampleEnabled() const;

	void EnableLineSmooth();
	void DisableLineSmooth();
	bool IsLineSmoothEnabled() const;

	void EnableClipDistance0();
	void DisableClipDistance0();
	bool IsClipDistance0Enabled() const;

	void EnableBlend();
	void DisableBlend();
	bool IsBlendEnabled() const;

	void EnableDepthClamp();
	void DisableDepthClamp();
	bool IsDepthClampEnabled() const;

	void EnableTextureCubmapSeamless();
	void DisableTextureCubmapSeamless();
	bool IsTextureCubmapSeamlessEnabled() const;

	void EnableRasterizerDiscard();
	void DisableRasterizerDiscard();
	bool IsRasterizerDiscardEnabled() const;

	void EnablePrimitiveRestart();
	void DisablePrimitiveRestart();
	bool IsPrimitiveRestartEnabled() const;

	static eGlPipelineState& GetInstance()
	{
		static eGlPipelineState  instance;
		return instance;
	}

private:
  eGlPipelineState() {}
	bool m_depth_test = false;
	bool m_stencil_test = false;
	bool m_cull_face = false;
	bool m_multisample = false;
	bool m_line_smooth = false;
	bool m_clip_distance_0 = false;
	bool m_blend = false;
	bool m_depth_clamp = false;
	bool m_cubemap_seamless = false;
	bool m_rasterizer_discard = false;
	bool m_primitive_restart = false;
};