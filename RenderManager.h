#pragma once
#include "ModelManager.h"
#include "TextureManager.h"

#include "WaterRender.h"
#include "SkyBoxRender.h"
#include "ScreenRender.h"
#include "ParticleRender.h"
#include "ParticleSystem.h"
#include "ShootingParticleSystem.h"
#include "MainRender.h"
#include "OutlinRender.h"
#include "SkyNoiseRender.h"
#include "WaveRender.h"
#include "HexRender.h"
#include "GaussianBlurRender.h"
#include "BrightFilterrender.h"
#include "ShadowRender.h"
#include "ShipScript.h"

class eRenderManager
{
protected:
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
	std::string							 folderPath;
public:
	void				Initialize(eModelManager&	modelManager, 
								   eTextureManager&  texManager,
								   const string&	folderPath);
	
	eWaterRender*		WaterRender()		 { return m_waterRender.get();		}
	eSkyBoxRender*		SkyBoxRender()		 { return m_skyboxRender.get();		}
	eScreenRender*		ScreenRender()		 { return m_screenRender.get();		}
	eParticleRender*	ParticleRender()	 { return m_particleRender.get();	}
	eShadowRender*		ShadowRender()		 { return m_shadowRender.get();		}
	eMainRender*		MainRender()		 { return m_mainRender.get();		}
	eOutlineRender*		OutlineRender()		 { return m_outlineRender.get();	}
	eSkyNoiseRender*	SkyNoiseRender()	 { return m_skynoiseRender.get();	}
	eWaveRender*		WaveRender()		 { return m_waverender.get();		}
	eHexRender*			HexRender()			 { return m_hexrender.get();		}
	eGaussianBlurRender*GaussianBlurRender() { return m_gaussianRender.get();	}
	eBrightFilterRender*BrightFilterRender() { return m_brightRender.get();		}
};
