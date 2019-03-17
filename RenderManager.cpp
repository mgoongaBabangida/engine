#include "RenderManager.h"

void eRenderManager::Initialize(ModelManager& modelManager, TextureManager& texManager)
{
	//Water Renderer adjust
	m_waterRender.reset(new eWaterRender(modelManager.clonePrimitive("brick_square"),
										texManager.find("Twaves0_n"),
										texManager.find("TwaterDUDV"),
										folderPath + "WaterVertexShader.glsl",
										folderPath + "WaterFragmentShader.glsl"));
	//SkyBox Renderer
	m_skyboxRender.reset(new eSkyBoxRender(*(texManager.find("TcubeSkyWater1")),
										folderPath + "SkyBoxVertexShader.glsl",
										folderPath + "SkyBoxFragmentShader.glsl"));
	//Screen Renderer
	m_screenRender.reset(new eScreenRender(*(texManager.find("TcubeSkyWater1")),
										folderPath + "PostProcessingVertexShader.glsl",
										folderPath + "PostProcessingFragmentShader.glsl"));
	//MainRender
	m_mainRender.reset(new eMainRender(folderPath + "VertexShaderCode.glsl", folderPath + "FragmentShaderCode.glsl"));
	//Shadow Render
	m_shadowRender.reset(new eShadowRender(folderPath + "VertexShades.glsl", folderPath + "FragmentShades.glsl"));
	//eOutlineRender
	m_outlineRender.reset(new eOutlineRender(folderPath + "VertexShaderCode.glsl", folderPath + "StencilFragmentShader.glsl"));

	//m_skynoiseRender = new eSkyNoiseRender(&m_MyModels[5], texManager.find("Tperlin_n"));

	m_waverender.reset(new eWaveRender(modelManager.cloneTerrain("simple"),
										texManager.find("TSpanishFlag0_s"),
										texManager.find("Tblue"),
										texManager.find("Tblack"), 
										folderPath + "WaveVertexShader.glsl",
										folderPath + "FragmentShaderCode.glsl"));

	m_hexrender.reset(new eHexRender(folderPath + "VertexShades.glsl", folderPath + "StencilFragmentShader.glsl", folderPath + "HexGeometry.glsl"));
	m_gaussianRender.reset(new eGaussianBlurRender(1200, 600, folderPath + "GaussianVertexShader.glsl", folderPath + "GaussianFragmentShader.glsl")); // buffer width height!!!
	m_brightRender.reset(new eBrightFilterRender(1200, 600, folderPath + "PostProcessingVertexShader.glsl", folderPath + "BrightFilterFragment.glsl"));  // buffer width height!!!

	// Particle Renderer
	m_particleRender.reset(new eParticleRender(modelManager.findMesh("square"),
											   texManager.find("Tatlas2"),
											   "ParticleVertexShader.glsl",
											   "ParticleFragmentShader.glsl"));
}
