#include "stdafx.h"
#include "RenderManager.h"

void eRenderManager::Initialize(eModelManager& modelManager, eTextureManager& texManager, const string& _folderPath)
{
	folderPath = _folderPath;
	//Water Renderer adjust
	m_waterRender.reset(new eWaterRender(modelManager.ClonePrimitive("brick_square"),
										texManager.Find("Twaves0_n"),
										texManager.Find("TwaterDUDV"),
										folderPath + "WaterVertexShader.glsl",
										folderPath + "WaterFragmentShader.glsl"));
	//SkyBox Renderer
	m_skyboxRender.reset(new eSkyBoxRender(*(texManager.Find("TcubeSkyWater1")),
										folderPath + "SkyBoxVertexShader.glsl",
										folderPath + "SkyBoxFragmentShader.glsl"));
	//Screen Renderer
	m_screenRender.reset(new eScreenRender(*(texManager.Find("TcubeSkyWater1")),
										folderPath + "PostProcessingVertexShader.glsl",
										folderPath + "PostProcessingFragmentShader.glsl"));
	//MainRender
	m_mainRender.reset(new eMainRender(folderPath + "VertexShaderCode.glsl", folderPath + "FragmentShaderCode.glsl"));
	//Shadow Render
	m_shadowRender.reset(new eShadowRender(folderPath + "VertexShades.glsl", folderPath + "FragmentShades.glsl"));
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

	m_hexrender.reset(new eHexRender(folderPath + "VertexShades.glsl", folderPath + "StencilFragmentShader.glsl", folderPath + "HexGeometry.glsl"));
	m_gaussianRender.reset(new eGaussianBlurRender(1200, 600, folderPath + "GaussianVertexShader.glsl", folderPath + "GaussianFragmentShader.glsl")); // buffer width height!!!
	m_brightRender.reset(new eBrightFilterRender(1200, 600, folderPath + "PostProcessingVertexShader.glsl", folderPath + "BrightFilterFragment.glsl"));  // buffer width height!!!

	// Particle Renderer
	m_particleRender.reset(new eParticleRender(modelManager.FindMesh("square"),
											   texManager.Find("Tatlas2"),
												folderPath + "ParticleVertexShader.glsl",
												folderPath + "ParticleFragmentShader.glsl"));
}
