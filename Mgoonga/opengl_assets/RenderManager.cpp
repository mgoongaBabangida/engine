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
ePhongRender*		 eRenderManager::PhongRender() { return m_phongRender.get(); }
eOutlineRender*		 eRenderManager::OutlineRender() { return m_outlineRender.get(); }
eSkyNoiseRender*	 eRenderManager::SkyNoiseRender() { return m_skynoiseRender.get(); }
eGeometryRender*			 eRenderManager::HexRender() { return m_hexrender.get(); }
eGaussianBlurRender* eRenderManager::GaussianBlurRender() { return m_gaussianRender.get(); }
eBrightFilterRender* eRenderManager::BrightFilterRender() { return m_brightRender.get(); }
eLinesRender*		 eRenderManager::LinesRender() { return m_linesRender.get(); }
eTextRender* eRenderManager::TextRender() { return m_textRender.get(); }
ePBRRender* eRenderManager::PBRRender(){ return m_pbrRender.get(); }
eBezierRender* eRenderManager::BezierRender() { return m_bezierRender.get(); }
eMeshLineRender* eRenderManager::MeshLineRender(){ return m_meshlineRender.get(); }
eSSAORender* eRenderManager::SSAORender() { return m_SSAORender.get(); }
eIBLRender* eRenderManager::IBLRender() { return  m_iblRender.get(); }
eCSMRender* eRenderManager::CSMRender() {return m_csmRender.get();}
eBloomRenderer* eRenderManager::BloomRenderer(){ return m_blomRender.get();}
eScreenSpaceReflectionRender* eRenderManager::SSRRenderer(){return m_ssrRender.get();}
eAreaLightsOnlyRender* eRenderManager::AreaLightsRender(){return m_area_lights.get();}
eCameraInterpolationRender* eRenderManager::CameraInterpolationRender(){return m_cameraInrepolationRender.get();}
eComputeShaderRender* eRenderManager::ComputeShaderRender() { return m_computeRender.get();}
eTerrainTessellatedRender* eRenderManager::TerrainTessellatedRender() { return m_terrainTesRender.get(); }
eVolumetricRender* eRenderManager::VolumetricRender() { return  m_volumetricRender.get(); }

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
	m_phongRender.reset(new ePhongRender(folderPath + "PhongVertexShader.glsl", folderPath + "WaveVertexShader.glsl", folderPath + "PhongFragmentShader.glsl",
																			 modelManager.CloneTerrain("simple"), texManager.Find("TSpanishFlag0_s")));
	shader_lambda(m_phongRender.get());
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

	m_hexrender.reset(new eGeometryRender(folderPath + "Vertex3DSimple.glsl",
																				folderPath + "StencilFragmentShader.glsl",
																				folderPath + "Geometry.glsl"));
	shader_lambda(m_hexrender.get());

	m_gaussianRender.reset(new eGaussianBlurRender(1200, 750, //@todo screen width/ height
																								folderPath + "GaussianVertexShader.glsl", 
																								folderPath + "GaussianFragmentShader.glsl"));
	shader_lambda(m_gaussianRender.get());
	m_brightRender.reset(new eBrightFilterRender(1200, 750, //@todo screen width/ height
																								folderPath + "PostProcessingVertexShader.glsl",
																								folderPath + "BrightFilterFragment.glsl"));
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

	//SSAO
	m_SSAORender.reset(new  eSSAORender(folderPath + "GeometryPassVertexShader.glsl",
																			folderPath + "GeometryPassFragmentShader.glsl",
																			folderPath + "PostProcessingVertexShader.glsl",
																			folderPath + "SSAOFragmentShader.glsl",
																			folderPath + "SSAOFragmentBlur.glsl"));
	shader_lambda(m_SSAORender.get());

	//IBL
	m_iblRender.reset(new  eIBLRender(folderPath + "hdrToCubemapVs.glsl",
																		folderPath + "hdrToCubemapFs.glsl",
																		folderPath + "irradianceFs.glsl",
																		folderPath + "prefilterFs.glsl",
																		folderPath + "SimpleVertexShader.glsl",
																		folderPath + "BrdfFs.glsl",
																		texManager.GetHdrIds()));
	//CSM
	m_csmRender.reset(new  eCSMRender(folderPath + "CSMVertexShader.glsl",
																		folderPath + "FragmentShades.glsl",
																		folderPath + "CSMGeometryShader.glsl"));
	shader_lambda(m_csmRender.get());

	//Physically Based Bloom
	m_blomRender.reset(new  eBloomRenderer(folderPath + "PostProcessingVertexShader.glsl",
																				 folderPath + "BloomFragmentDownSampling.glsl",
																				 folderPath + "BloomFragmentUpSampling.glsl"));
	shader_lambda(m_blomRender.get());

	// Screen Space Reflection
	m_ssrRender.reset(new  eScreenSpaceReflectionRender(folderPath + "PostProcessingVertexShader.glsl",
																											folderPath + "SSR.glsl",
																											folderPath + "ScreenSpaceReflactionBlur.glsl"));
	shader_lambda(m_ssrRender.get());

	//Area lights
	m_area_lights.reset(new eAreaLightsOnlyRender(folderPath + "PBRVertexShaderCode.glsl", folderPath + "AreaLightOnlyFS.glsl"));
	shader_lambda(m_area_lights.get());

	//Camera Interpolation
	m_cameraInrepolationRender.reset(new  eCameraInterpolationRender(folderPath + "PostProcessingVertexShader.glsl",
																																	 folderPath + "CameraInterpolationNewCoords.glsl",
																																	 folderPath + "CameraInterpolationCompute.glsl",
																																	 texManager.Find("computeImageRWCameraInterpolation")));
	shader_lambda(m_cameraInrepolationRender.get());

	//Compute
	m_computeRender.reset(new  eComputeShaderRender(folderPath + "ComputeShader.glsl", folderPath + "ComputePSShader.glsl",
																									folderPath + "PassThroughVertex.glsl", folderPath + "PassThroughFragment.glsl",
																									folderPath + "ClothSimulationVertex.glsl", folderPath + "ClothSimulationFragment.glsl", 
																									folderPath + "ClothSimulationCompute.glsl", folderPath + "ClothSimulationNormalsCompute.glsl",
																									folderPath + "EdgeFindingCompute.glsl",
																									folderPath + "Worley3DComputeShader.glsl",
																									folderPath + "PostProcessingVertexShader.glsl", //result
																									folderPath + "3DtextureDebugFragment.glsl", //debug 3d
																									texManager.Find("TSpanishFlag0_s"),
																									texManager.Find("computeImageRW")));
	shader_lambda(m_computeRender.get());

	//Terrain Tessellation
	m_terrainTesRender.reset(new eTerrainTessellatedRender(folderPath + "TerrainTessellationVertex.glsl",
																												 folderPath + "TerrainTessellationFragment.glsl",
																												 folderPath + "TerrainTessellationTes1.glsl",
																												 folderPath + "TerrainTessellationTes2.glsl"));
	shader_lambda(m_terrainTesRender.get());

	//Volumetric Tessellation
	m_volumetricRender.reset(new eVolumetricRender(folderPath + "VolumetricVertex.glsl", folderPath + "VolumetricFragment.glsl", m_computeRender->GetWorley3DID()));
	shader_lambda(m_volumetricRender.get());
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
	shader_lambda(m_phongRender.get());
	shader_lambda(m_shadowRender.get());
	shader_lambda(m_outlineRender.get());
	shader_lambda(m_skynoiseRender.get());
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
	shader_lambda(m_SSAORender.get());
	shader_lambda(m_csmRender.get());
	shader_lambda(m_blomRender.get());
	shader_lambda(m_ssrRender.get());
	shader_lambda(m_area_lights.get());
	shader_lambda(m_cameraInrepolationRender.get());
	shader_lambda(m_computeRender.get());
	shader_lambda(m_terrainTesRender.get());
	shader_lambda(m_volumetricRender.get());
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
	if (shader_lambda(m_phongRender.get()))
		return true;
	if (shader_lambda(m_shadowRender.get()))
		return true;
	if (shader_lambda(m_outlineRender.get()))
		return true;
	if (shader_lambda(m_skynoiseRender.get()))
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
	if (shader_lambda(m_pbrRender.get()))
		return true;
	if (shader_lambda(m_bezierRender.get()))
		return true;
	if(shader_lambda(m_meshlineRender.get()))
		return true;
	if (shader_lambda(m_SSAORender.get()))
		return true;
	if (shader_lambda(m_csmRender.get()))
		return true;
	if (shader_lambda(m_blomRender.get()))
		return true;
	if (shader_lambda(m_ssrRender.get()))
		return true;
	if (shader_lambda(m_area_lights.get()))
		return true;
	if (shader_lambda(m_cameraInrepolationRender.get()))
		return true;
	if (shader_lambda(m_computeRender.get()))
		return true;
	if (shader_lambda(m_terrainTesRender.get()))
		return true;
	if (shader_lambda(m_volumetricRender.get()))
		return true;
	return false;
}
