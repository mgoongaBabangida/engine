#include "stdafx.h"
#include "RenderManager.h"

#include "ShapeGenerator.h"
#include "ShapeData.h"

void eRenderManager::Initialize(ModelManager& modelManager, TextureManager& texManager, remSnd* sound, Texture* pTex)
{
	ShapeData quad = ShapeGenerator::makeQuad(); //change design
	//Water Renderer adjust
	m_waterRender.reset(new eWaterRender(modelManager.clonePrimitive("brick_square"), texManager.find("Twaves0_n"), texManager.find("TwaterDUDV")));
	//SkyBox Renderer
	m_skyboxRender.reset(new eSkyBoxRender(*(texManager.find("TcubeSkyWater1"))));
	//Screen Renderer
	m_screenRender.reset(new eScreenRender(*(texManager.find("TcubeSkyWater1"))));
	//MainRender
	m_mainRender.reset(new eMainRender());
	//Shadow Render
	m_shadowRender.reset(new eShadowRender());
	//eOutlineRender
	m_outlineRender.reset(new eOutlineRender());

	//m_skynoiseRender = new eSkyNoiseRender(&m_MyModels[5], texManager.find("Tperlin_n"));

	m_waverender.reset(new eWaveRender(modelManager.cloneTerrain("simple"),
		texManager.find("TSpanishFlag0_s"),
		texManager.find("Tblue"),
		texManager.find("Tblack")));

	m_hexrender.reset(new eHexRender());
	m_gaussianRender.reset(new eGaussianBlurRender(1200, 600)); // buffer width height!!!
	m_brightRender.reset(new eBrightFilterRender(1200, 600));  // buffer width height!!!

	// Particle Renderer

	ParticleSystemInfo info;
	info.texture = pTex;
	info.scale = 0.05;
	info.systemCenter = glm::vec3(0.0f, 4.0f, -0.5f);
	m_particleRender.reset(new eParticleRender(new MyModel(modelManager.findMesh("square"),
												texManager.find("TShootAtlas0_n")),
												new ParticleMesh(quad),
												new ParticleSystem(10, 0, 0, 10000, sound), info)); // new!
	quad.cleanup();
}
