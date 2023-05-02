#pragma once

#include <math/ShootingParticleSystem.h> //@todo separate particle system base

#include "opengl_assets.h"

#include "ModelManager.h"
#include "TextureManager.h"

#include "WaterRender.h"
#include "SkyBoxRender.h"
#include "ScreenRender.h"
#include "ParticleRender.h"
#include "MainRender.h"
#include "OutlinRender.h"
#include "SkyNoiseRender.h"
#include "WaveRender.h"
#include "HexRender.h"
#include "GaussianBlurRender.h"
#include "BrightFilterrender.h"
#include "ShadowRender.h"
#include "LinesRender.h"
#include "TextRender.h"
#include "PBRRender.h"
#include "BezierRender.h"
#include "ParticleSystemRenderGPUTransformfeedback.h"
#include "MeshLineRender.h"

//----------------------------------------------------------------------------------------------
class eRenderManager
{
public:
	friend class eOpenGlRenderPipeline;

	eRenderManager() = default;
	~eRenderManager() = default;

	const std::vector<ShaderInfo>&	GetShaderInfos() const { return shaders;}
	void														UpdateShadersInfo();
	bool														SetUniformData(const std::string& _renderName,
																								 const std::string& _uniformName,
																								 const UniformData& _data);

	void				Initialize(eModelManager&	modelManager,
								         eTextureManager&  texManager,
								         const std::string&	folderPath);

	void				AddParticleSystem(IParticleSystem* system);
	void				AddParticleSystemGPU(glm::vec3 _startPos, Texture* _texture);

private:

	eWaterRender*		WaterRender();
	eSkyBoxRender*		SkyBoxRender();
	eScreenRender*		ScreenRender();
	eParticleRender*	ParticleRender();
	eParticleSystemRenderGPU_V2* ParticleRenderGPU();
	eShadowRender*		ShadowRender();
	eMainRender*		MainRender();
	eOutlineRender*		OutlineRender();
	eSkyNoiseRender*	SkyNoiseRender();
	eWaveRender*		WaveRender();
	eHexRender*			HexRender();
	eGaussianBlurRender*GaussianBlurRender();
	eBrightFilterRender*BrightFilterRender();
	eLinesRender*		LinesRender();
	eTextRender* TextRender();
	ePBRRender* PBRRender();
	eBezierRender* BezierRender();
	eMeshLineRender* MeshLineRender();

private:
	std::unique_ptr<eWaterRender>		 m_waterRender;
	std::unique_ptr<eSkyBoxRender>		 m_skyboxRender;
	std::unique_ptr<eScreenRender>		 m_screenRender;
	std::unique_ptr<eParticleRender>	 m_particleRender;
	std::unique_ptr <eParticleSystemRenderGPU_V2> m_particleRenderGPU;
	std::unique_ptr<eShadowRender>		 m_shadowRender;
	std::unique_ptr<eMainRender>		 m_mainRender;
	std::unique_ptr<eOutlineRender>		 m_outlineRender;
	std::unique_ptr<eSkyNoiseRender>	 m_skynoiseRender;
	std::unique_ptr<eWaveRender>		 m_waverender;
	std::unique_ptr<eHexRender>			 m_hexrender;
	std::unique_ptr<eGaussianBlurRender> m_gaussianRender;
	std::unique_ptr<eBrightFilterRender> m_brightRender;
	std::unique_ptr<eLinesRender>		 m_linesRender;
	std::unique_ptr<eTextRender>     m_textRender;
	std::unique_ptr<ePBRRender>      m_pbrRender;
	std::unique_ptr<eBezierRender>   m_bezierRender;
	std::unique_ptr<eMeshLineRender>   m_meshlineRender;

	std::string							folderPath;
	std::vector<ShaderInfo>	shaders;
};

