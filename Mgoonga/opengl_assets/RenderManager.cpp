#include "stdafx.h"
#include "RenderManager.h"
#include "MyModel.h"

eWaterRender*		 eRenderManager::WaterRender() { return m_waterRender.get(); }
eSkyBoxRender*		 eRenderManager::SkyBoxRender() { return m_skyboxRender.get(); }
eScreenRender*		 eRenderManager::ScreenRender() { return m_screenRender.get(); }
eParticleRender*	 eRenderManager::ParticleRender() { return m_particleRender.get(); }
eShadowRender*		 eRenderManager::ShadowRender() { return m_shadowRender.get(); }
eMainRender*		 eRenderManager::MainRender() { return m_mainRender.get(); }
eOutlineRender*		 eRenderManager::OutlineRender() { return m_outlineRender.get(); }
eSkyNoiseRender*	 eRenderManager::SkyNoiseRender() { return m_skynoiseRender.get(); }
eWaveRender*		 eRenderManager::WaveRender() { return m_waverender.get(); }
eHexRender*			 eRenderManager::HexRender() { return m_hexrender.get(); }
eGaussianBlurRender* eRenderManager::GaussianBlurRender() { return m_gaussianRender.get(); }
eBrightFilterRender* eRenderManager::BrightFilterRender() { return m_brightRender.get(); }
eLinesRender*		 eRenderManager::LinesRender() { return m_linesRender.get(); }
eTextRender* eRenderManager::TextRender() { return m_textRender.get(); }
ePBRRender* eRenderManager::PBRRender(){ return m_pbrRender.get(); }

eRenderManager::eRenderManager()
{
}

eRenderManager::~eRenderManager()
{
}

void				eRenderManager::AddHex(glm::vec3 _v) { dots.push_back(_v); }
void				eRenderManager::SetHexRadius(float _r) { radius = _r; }
void				eRenderManager::AddParticleSystem(IParticleSystem* system) { m_particleRender->AddParticleSystem(system); }

//---------------------------------------------------------------------------------------------------------------
void eRenderManager::Initialize(eModelManager& modelManager, eTextureManager& texManager, const std::string& _folderPath)
{
	folderPath = _folderPath;
	//Water Renderer adjust
	m_waterRender.reset(new eWaterRender(modelManager.ClonePrimitive("brick_square"),
										texManager.Find("Twaves0_n"),
										texManager.Find("TwaterDUDV"),
										folderPath + "WaterVertexShader.glsl",
										folderPath + "WaterFragmentShader.glsl"));
	//SkyBox Renderer
	m_skyboxRender.reset(new eSkyBoxRender(texManager.Find("TcubeSkyWater2"),
										folderPath + "SkyBoxVertexShader.glsl",
										folderPath + "SkyBoxFragmentShader.glsl"));
	//Screen Renderer
	m_screenRender.reset(new eScreenRender(*(texManager.Find("TcubeSkyWater1")),
										folderPath + "PostProcessingVertexShader.glsl",
										folderPath + "PostProcessingFragmentShader.glsl"));
	//MainRender
	m_mainRender.reset(new eMainRender(folderPath + "VertexShaderCode.glsl", folderPath + "FragmentShaderCode.glsl"));
	//Shadow Render
	m_shadowRender.reset(new eShadowRender(folderPath + "VertexShades.glsl", folderPath + "FragmentShades.glsl",
										   folderPath + "GeometryPointShadowShader.glsl", folderPath + "FragmentPointShadowShader.glsl"));
	//eOutlineRender
	m_outlineRender.reset(new eOutlineRender(folderPath + "VertexShaderCode.glsl", folderPath + "StencilFragmentShader.glsl"));
	
	//eSkyNoiseRender
	m_skynoiseRender.reset(new eSkyNoiseRender(modelManager.ClonePrimitive("brick_square"),
											   texManager.Find("Tperlin_n"), 
											   folderPath + "SkyNoiseVertexShader.glsl", 
											   folderPath + "SkyNoiseFragmentShader.glsl"));

	m_waverender.reset(new eWaveRender(modelManager.CloneTerrain("simple"),
										texManager.Find("TSpanishFlag0_s"),
										texManager.Find("Tblue"),
										texManager.Find("Tblack"), 
										folderPath + "WaveVertexShader.glsl",
										folderPath + "FragmentShaderCode.glsl"));

	m_hexrender.reset(new eHexRender(folderPath + "VertexShades.glsl", folderPath + "StencilFragmentShader.glsl", folderPath + "HexGeometry.glsl", dots, radius));
	m_gaussianRender.reset(new eGaussianBlurRender(1200, 600, folderPath + "GaussianVertexShader.glsl", folderPath + "GaussianFragmentShader.glsl")); // buffer width height!!!
	m_brightRender.reset(new eBrightFilterRender(1200, 600, folderPath + "PostProcessingVertexShader.glsl", folderPath + "BrightFilterFragment.glsl"));  // buffer width height!!!

	// Particle Renderer
	m_particleRender.reset(new eParticleRender(modelManager.FindMesh("square"),
											   texManager.Find("Tatlas2"),
											   folderPath + "ParticleVertexShader.glsl",
											   folderPath + "ParticleFragmentShader.glsl"));
	//Lines
	m_linesRender.reset(new eLinesRender(folderPath + "SkyBoxVertexShader.glsl",
										 folderPath + "StencilFragmentShader.glsl"));

	//Text
	m_textRender.reset(new eTextRender(folderPath + "TextVertex.glsl", folderPath + "TextFragment.glsl"));
	//PBR
	m_pbrRender.reset(new ePBRRender(folderPath + "PBRVertexShaderCode.glsl", folderPath + "PBRFragmentShader.glsl"));
}
