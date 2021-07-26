#pragma once

#include <math/ShootingParticleSystem.h>

#include "opengl_assets.h"
#include "ModelManager.h"
#include "TextureManager.h"

class eWaterRender;
class eSkyBoxRender;
class eScreenRender;
class eParticleRender;
class eShadowRender;
class eMainRender;
class eOutlineRender;
class eSkyNoiseRender;
class eWaveRender;
class eHexRender;
class eGaussianBlurRender;
class eBrightFilterRender;
class eLinesRender;
class eTextRender;

//----------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS eRenderManager
{
public:
	friend class ePipeline;

	eRenderManager();
	~eRenderManager();

	void				AddHex(glm::vec3 _v);
	void				SetHexRadius(float _r);

	void				Initialize(eModelManager&	modelManager, 
								   eTextureManager&  texManager,
								   const string&	folderPath);
	void				AddParticleSystem(IParticleSystem* system);

private:
	eWaterRender*		WaterRender();
	eSkyBoxRender*		SkyBoxRender();
	eScreenRender*		ScreenRender();
	eParticleRender*	ParticleRender();
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

private:
	std::unique_ptr<eWaterRender>		 m_waterRender;
	std::unique_ptr<eSkyBoxRender>		 m_skyboxRender;
	std::unique_ptr<eScreenRender>		 m_screenRender;
	std::unique_ptr<eParticleRender>	 m_particleRender;
	std::unique_ptr<eShadowRender>		 m_shadowRender;
	std::unique_ptr<eMainRender>		 m_mainRender;
	std::unique_ptr<eOutlineRender>		 m_outlineRender;
	std::unique_ptr<eSkyNoiseRender>	 m_skynoiseRender;
	std::unique_ptr<eWaveRender>		 m_waverender;
	std::unique_ptr<eHexRender>			 m_hexrender;
	std::unique_ptr<eGaussianBlurRender> m_gaussianRender;
	std::unique_ptr<eBrightFilterRender> m_brightRender;
	std::unique_ptr<eLinesRender>		 m_linesRender;
	std::unique_ptr<eTextRender>    m_textRender;

	std::string							 folderPath;

	std::vector<glm::vec3>	 dots; //extra copy $change design
	float								     radius;
};

