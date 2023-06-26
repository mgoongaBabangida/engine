#include "stdafx.h"
#include "RenderManager.h"
#include "MyModel.h"
#include <algorithm>

//--------------------------------------------------------------------------------
eWaterRender*		 eRenderManager::WaterRender() { return m_waterRender.get(); }
eSkyBoxRender*		 eRenderManager::SkyBoxRender() { return m_skyboxRender.get(); }
eScreenRender*		 eRenderManager::ScreenRender() { return m_screenRender.get(); }
eParticleRender*	 eRenderManager::ParticleRender() { return m_particleRender.get(); }
eParticleSystemRenderGPU_V2* eRenderManager::ParticleRenderGPU(){ return m_particleRenderGPU.get();}
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
eBezierRender* eRenderManager::BezierRender() { return m_bezierRender.get(); }
eMeshLineRender* eRenderManager::MeshLineRender(){ return m_meshlineRender.get(); }

//----------------------------------------------------------------------------------------------------------------
void eRenderManager::AddParticleSystem(std::shared_ptr<IParticleSystem> system)
{
	m_particleRender->AddParticleSystem(system);
}

//----------------------------------------------------------------------------------------------------------------
void eRenderManager::AddParticleSystemGPU(glm::vec3 _startPos, const Texture* _texture)
{
	m_particleRenderGPU->AddParticleSystem(_startPos, _texture);
}

//---------------------------------------------------------------------------------------------------------------
void eRenderManager::Initialize(eModelManager& modelManager, eTextureManager& texManager, const std::string& _folderPath)
{
	folderPath = _folderPath;

	auto shader_lambda = [this](const auto& _render) {
		std::cout << "------------" << std::endl; 
		_render->GetShader().GetUniformInfoFromShader();
		_render->GetShader().GetUniformDataFromShader();
		std::string name = typeid(*_render).name(); //std::to_string(static_cast<int32_t>(_render->GetShader().ID()));
		shaders.push_back({ name, static_cast<int32_t>(_render->GetShader().ID()), _render->GetShader().GetUniforms()});
	};

	//Water Renderer adjust
	m_waterRender.reset(new eWaterRender(modelManager.ClonePrimitive("brick_square"),
																			 texManager.Find("Twaves0_n"),
																			 texManager.Find("TwaterDUDV"),
																			 folderPath + "WaterVertexShader.glsl",
																			 folderPath + "WaterFragmentShader.glsl"));
	shader_lambda(m_waterRender.get());
	//SkyBox Renderer
	m_skyboxRender.reset(new eSkyBoxRender(texManager.Find("TcubeSkyWater2"),
																				folderPath + "SkyBoxVertexShader.glsl",
																				folderPath + "SkyBoxFragmentShader.glsl"));
	shader_lambda(m_skyboxRender.get());
	//Screen Renderer
	m_screenRender.reset(new eScreenRender(*(texManager.Find("TcubeSkyWater1")),
										folderPath + "PostProcessingVertexShader.glsl",
										folderPath + "PostProcessingFragmentShader.glsl"));
	shader_lambda(m_screenRender.get());
	//PhongRender
	m_mainRender.reset(new eMainRender(folderPath + "PhongVertexShader.glsl", folderPath + "PhongFragmentShader.glsl"));
	shader_lambda(m_mainRender.get());
	//Shadow Render
	m_shadowRender.reset(new eShadowRender(folderPath + "VertexShades.glsl", //@todo rendame
																				 folderPath + "FragmentShades.glsl",
																				 folderPath + "GeometryPointShadowShader.glsl",
																				 folderPath + "FragmentPointShadowShader.glsl"));
	shader_lambda(m_shadowRender.get());
	//eOutlineRender
	m_outlineRender.reset(new eOutlineRender(folderPath + "PhongVertexShader.glsl", folderPath + "StencilFragmentShader.glsl"));
	shader_lambda(m_outlineRender.get());
	//eSkyNoiseRender
	m_skynoiseRender.reset(new eSkyNoiseRender(modelManager.ClonePrimitive("brick_square"),
																						texManager.Find("Tperlin_n"), 
																						folderPath + "SkyNoiseVertexShader.glsl", 
																						folderPath + "SkyNoiseFragmentShader.glsl"));
	shader_lambda(m_skynoiseRender.get());

	m_waverender.reset(new eWaveRender(modelManager.CloneTerrain("simple"),
																		texManager.Find("TSpanishFlag0_s"),
																		texManager.Find("Tblue"),
																		texManager.Find("Tblack"), 
																		folderPath + "WaveVertexShader.glsl",
																		folderPath + "PhongFragmentShader.glsl"));
	shader_lambda(m_waverender.get());

	m_hexrender.reset(new eHexRender(folderPath + "Vertex3DSimple.glsl",
																	 folderPath + "StencilFragmentShader.glsl",
																	 folderPath + "HexGeometry.glsl"));
	shader_lambda(m_hexrender.get());

	m_gaussianRender.reset(new eGaussianBlurRender(1200, 600, //@todo
																								folderPath + "GaussianVertexShader.glsl", 
																								folderPath + "GaussianFragmentShader.glsl")); //@todo buffer width height!!!
	shader_lambda(m_gaussianRender.get());
	m_brightRender.reset(new eBrightFilterRender(1200, 600, //@todo
																								folderPath + "PostProcessingVertexShader.glsl", 
																								folderPath + "BrightFilterFragment.glsl"));  //@todo buffer width height!!!
	shader_lambda(m_brightRender.get());
	// Particle Renderer
	m_particleRender.reset(new eParticleRender(modelManager.FindMesh("square"),
											   texManager.Find("Tatlas2"),
											   folderPath + "ParticleVertexShader.glsl",
											   folderPath + "ParticleFragmentShader.glsl"));
	shader_lambda(m_particleRender.get());
	// Particle Renderer GPU
	/*m_particleRenderGPU.reset(new eParticleSystemRenderGPU(folderPath + "ParticleGPUUpdateVertexShader.glsl",
																												 folderPath + "ParticleGPURenderFragmentShader.glsl",
																												 folderPath + "ParticleGPUUpdateGeometryShader.glsl",
																												 folderPath + "ParticleGPURenderVertexShader.glsl",
																												 folderPath + "ParticleGPURenderGeometryShader.glsl"));*/
	m_particleRenderGPU.reset(new eParticleSystemRenderGPU_V2(folderPath + "transfeedbackVS.glsl",
																														folderPath + "transfeedbackFS.glsl",
																														folderPath + "ParticleGPURenderGeometryShader.glsl"));

	shader_lambda(m_particleRenderGPU.get());
	//Lines
	m_linesRender.reset(new eLinesRender(folderPath + "Vertex3DSimple.glsl",
																			 folderPath + "StencilFragmentShader.glsl"));
	shader_lambda(m_linesRender.get());
	//Text
	m_textRender.reset(new eTextRender(folderPath + "TextVertex.glsl", folderPath + "TextFragment.glsl"));
	shader_lambda(m_textRender.get());
	//PBR
	m_pbrRender.reset(new ePBRRender(folderPath + "PBRVertexShaderCode.glsl", folderPath + "PBRFragmentShader.glsl"));
	shader_lambda(m_pbrRender.get());
	//Bezier
	m_bezierRender.reset(new eBezierRender(folderPath + "SimpleVertexShader.glsl",
																				 folderPath + "StencilFragmentShader.glsl",
																				 folderPath + "BezierTessellation1Shader.glsl",
																				 folderPath + "BezierTessellation2Shader.glsl"));
	shader_lambda(m_bezierRender.get());
	// Mesh line
	m_meshlineRender.reset(new eMeshLineRender(folderPath + "PhongVertexShader.glsl", folderPath + "StencilFragmentShader.glsl"));
	shader_lambda(m_meshlineRender.get());
}

//----------------------------------------------------------------------------------------------------
void eRenderManager::UpdateShadersInfo()
{
	auto shader_lambda = [this](const auto& _render)
	{
		_render->GetShader().GetUniformDataFromShader();
	};

	shader_lambda(m_waterRender.get());
	shader_lambda(m_skyboxRender.get());
	shader_lambda(m_screenRender.get());
	shader_lambda(m_mainRender.get());
	shader_lambda(m_shadowRender.get());
	shader_lambda(m_outlineRender.get());
	shader_lambda(m_skynoiseRender.get());
	shader_lambda(m_waverender.get());
	shader_lambda(m_hexrender.get());
	shader_lambda(m_gaussianRender.get());
	shader_lambda(m_brightRender.get());
	shader_lambda(m_particleRender.get());
	shader_lambda(m_particleRenderGPU.get());
	shader_lambda(m_linesRender.get());
	shader_lambda(m_textRender.get());
	shader_lambda(m_pbrRender.get());
	shader_lambda(m_bezierRender.get());
	shader_lambda(m_meshlineRender.get());
}

//----------------------------------------------------------------------------------------------------
bool eRenderManager::SetUniformData(const std::string& _renderName, const std::string& _uniformName, const UniformData& _data)
{
	auto shader_lambda = [this, _renderName, _uniformName, _data](const auto& _render)
	{
		if (typeid(*_render).name() == _renderName)
			return _render->GetShader().SetUniformData(_uniformName, _data);
		else
			return false;
	};

	if (shader_lambda(m_waterRender.get()))
		return true;
	if (shader_lambda(m_skyboxRender.get()))
		return true;
	if (shader_lambda(m_screenRender.get()))
		return true;
	if (shader_lambda(m_mainRender.get()))
		return true;
	if (shader_lambda(m_shadowRender.get()))
		return true;
	if (shader_lambda(m_outlineRender.get()))
		return true;
	if (shader_lambda(m_skynoiseRender.get()))
		return true;
	if (shader_lambda(m_waverender.get()))
		return true;
	if (shader_lambda(m_hexrender.get()))
		return true;
	if (shader_lambda(m_gaussianRender.get()))
		return true;
	if (shader_lambda(m_brightRender.get()))
		return true;
	if (shader_lambda(m_particleRender.get()))
		return true;
	if (shader_lambda(m_particleRenderGPU.get()))
		return true;
	if (shader_lambda(m_linesRender.get()))
		return true;
	if (shader_lambda(m_textRender.get()))
		return true;
	return true;
	if (shader_lambda(m_pbrRender.get()))
		return true;
	if (shader_lambda(m_bezierRender.get()))
		return true;
	if(shader_lambda(m_meshlineRender.get()))
		return true;

	return false;
}
