#include "stdafx.h"
#include "OpenGlRenderPipeline.h"

#include <math/Camera.h>
#include "GUI.h"

#include "GlBufferContext.h"
#include "GlDrawContext.h"

#include "RenderManager.h"
#include "TextureManager.h"
#include "ModelManager.h"

#include <algorithm>

float eOpenGlRenderPipeline::GetWaterHeight() const { return waterHeight; }
bool& eOpenGlRenderPipeline::GetKernelOnRef() { return kernel; }
bool& eOpenGlRenderPipeline::GetSkyNoiseOnRef() { return sky_noise; }
bool& eOpenGlRenderPipeline::GetOutlineFocusedRef() { return outline_focused; }
bool& eOpenGlRenderPipeline::IBL() { return ibl_on; }

//---------------------------------------------------------------------------------------------
void	eOpenGlRenderPipeline::SetSkyBoxTexture(const Texture* _t)
{ renderManager->SkyBoxRender()->SetSkyBoxTexture(_t); }

//---------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::SetSkyIBL(unsigned int _irr, unsigned int _prefilter)
{
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _irr);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _prefilter);
}

//---------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::AddParticleSystem(std::shared_ptr<IParticleSystem> _system)
{
	renderManager->AddParticleSystem(_system);
}

//---------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::AddParticleSystemGPU(glm::vec3 _startPos, const Texture* _texture)
{
	renderManager->AddParticleSystemGPU(_startPos, _texture);
}

void eOpenGlRenderPipeline::SwitchSkyBox(bool on) { skybox = on; }

void eOpenGlRenderPipeline::SwitchWater(bool on) { water = on; }

//---------------------------------------------------------------------------------------------
const std::vector<ShaderInfo>& eOpenGlRenderPipeline::GetShaderInfos() const
{ return renderManager->GetShaderInfos(); }

//---------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::UpdateShadersInfo()
{
	renderManager->UpdateShadersInfo();
}

//-------------------------------------------------------------------------------------------
bool eOpenGlRenderPipeline::SetUniformData(const std::string& _renderName, const std::string& _uniformName, const UniformData& _data)
{
	return renderManager.get()->SetUniformData(_renderName, _uniformName, _data);
}

//-------------------------------------------------------------------------------------------
eOpenGlRenderPipeline::eOpenGlRenderPipeline(uint32_t _width, uint32_t _height)
: width(_width),
  height(_height),
  renderManager(new eRenderManager)
{
}

//-------------------------------------------------------------------------------------------
eOpenGlRenderPipeline::~eOpenGlRenderPipeline()
{
}

//-------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::UpdateSharedUniforms()
{
	//SetUniformData("class ePBRRender", "Fog.maxDist", m_foginfo.maxDist);
	//SetUniformData("class ePBRRender", "Fog.minDist", m_foginfo.minDist);
	SetUniformData("class ePBRRender", "Fog.color", m_foginfo.color);
	SetUniformData("class ePBRRender", "Fog.fog_on", m_foginfo.fog_on);
	SetUniformData("class ePBRRender", "Fog.density", m_foginfo.density);
	SetUniformData("class ePBRRender", "Fog.gradient", m_foginfo.gradient);

	SetUniformData("class ePBRRender", "ibl_on", ibl_on);

	//SetUniformData("class ePhongRender", "Fog.maxDist", m_foginfo.maxDist);
	//SetUniformData("class ePhongRender", "Fog.minDist", m_foginfo.minDist);
	SetUniformData("class ePhongRender", "Fog.color", m_foginfo.color);
	SetUniformData("class ePhongRender", "Fog.fog_on", m_foginfo.fog_on);
	SetUniformData("class ePhongRender", "Fog.density", m_foginfo.density);
	SetUniformData("class ePhongRender", "Fog.gradient", m_foginfo.gradient);
}

//-------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
}

//-------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::InitializeBuffers()
{
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFAULT, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN_WITH_SSR, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_MTS, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFLECTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFRACTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SSR, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SSR_BLUR, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW_DIR, width*2, height*2);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW_CUBE_MAP, width*2, height*2);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW_CSM, width * 2, height * 2);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SQUERE, height, height); //squere
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_BRIGHT_FILTER, width, height);
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_GAUSSIAN_ONE, 600, 300); //@todo numbers
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_GAUSSIAN_TWO, 600, 300);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SSAO, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SSAO_BLUR, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_IBL_CUBEMAP, 512, 512);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_IBL_CUBEMAP_IRR, 32, 32);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_BLOOM, width, height);
	eGlBufferContext::GetInstance().BufferCustomInit("CameraInterpolationCoordsBuffer", width, height);
	eGlBufferContext::GetInstance().BufferCustomInit("ComputeParticleSystemBuffer", width, height);
}

//-----------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::InitializeRenders(eModelManager& modelManager, eTextureManager& texManager, const std::string& shadersFolderPath)
{
	renderManager->Initialize(modelManager, texManager, shadersFolderPath);
	m_texture_manager = &texManager;
}

//-----------------------------------------------------------------------------------------------
Texture eOpenGlRenderPipeline::GetSkyNoiseTexture(const Camera& _camera)
{
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SQUERE);
	RenderSkyNoise(_camera); //do we need camera?
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SQUERE);
}

float& eOpenGlRenderPipeline::GetSaoThresholdRef() { return renderManager->GetSsaoThresholdRef(); }
float& eOpenGlRenderPipeline::GetSaoStrengthRef() { return renderManager->GetSsaoStrengthRef(); }

float& eOpenGlRenderPipeline::GetExposureRef()
{
	return renderManager->ScreenRender()->GetExposure();
}

bool& eOpenGlRenderPipeline::GetGammaCorrectionRef()
{
	//@todo screen render as well
	return renderManager->PhongRender()->GetGammaCorrection();
}

bool& eOpenGlRenderPipeline::GetToneMappingRef()
{
	return renderManager->ScreenRender()->GetToneMapping();
}

bool& eOpenGlRenderPipeline::GetDebugWhite()
{
	return renderManager->PhongRender()->GetDebugWhite();
}

bool& eOpenGlRenderPipeline::GetDebugTexCoords()
{
	return renderManager->PhongRender()->GetDebugTextCoords();
}

float& eOpenGlRenderPipeline::GetEmissionStrengthRef()
{
	return renderManager->PhongRender()->GetEmissionStrength();
}

float& eOpenGlRenderPipeline::WaveSpeedFactor()
{
	return renderManager->WaterRender()->WaveSpeedFactor();
}

float& eOpenGlRenderPipeline::Tiling()
{
	return renderManager->WaterRender()->Tiling();
}

float& eOpenGlRenderPipeline::WaveStrength()
{
	return renderManager->WaterRender()->WaveStrength();
}

float& eOpenGlRenderPipeline::ShineDumper()
{
	return renderManager->WaterRender()->ShineDumper();
}

float& eOpenGlRenderPipeline::Reflactivity()
{
	return renderManager->WaterRender()->Reflactivity();
}

glm::vec4& eOpenGlRenderPipeline::WaterColor()
{
	return renderManager->WaterRender()->WaterColor();
}

float& eOpenGlRenderPipeline::ColorMix()
{
	return renderManager->WaterRender()->ColorMix();
}

float& eOpenGlRenderPipeline::RefrectionFactor()
{
	return renderManager->WaterRender()->RefrectionFactor();
}

float& eOpenGlRenderPipeline::DistortionStrength()
{
	return renderManager->WaterRender()->DistortionStrength();
}

float& eOpenGlRenderPipeline::ZMult()
{
	return renderManager->CSMRender()->ZMult();
}

float& eOpenGlRenderPipeline::Step()
{
	return renderManager->SSRRenderer()->Step();
}

float& eOpenGlRenderPipeline::MinRayStep()
{
	return renderManager->SSRRenderer()->MinRayStep();
}

float& eOpenGlRenderPipeline::MaxSteps()
{
	return renderManager->SSRRenderer()->MaxSteps();
}

int& eOpenGlRenderPipeline::NumBinarySearchSteps()
{
	return renderManager->SSRRenderer()->NumBinarySearchSteps();
}

float& eOpenGlRenderPipeline::ReflectionSpecularFalloffExponent()
{
	return renderManager->SSRRenderer()->ReflectionSpecularFalloffExponent();
}

glm::vec3& eOpenGlRenderPipeline::GetSecondCameraPositionRef()
{
	return renderManager->CameraInterpolationRender()->GetSecondCameraPositionRef();
}

float& eOpenGlRenderPipeline::GetDisplacementRef()
{
	return renderManager->CameraInterpolationRender()->GetDisplacementRef();
}

glm::mat4& eOpenGlRenderPipeline::GetLookAtMatrix()
{
	return renderManager->CameraInterpolationRender()->GetLookAtMatrix();
}

glm::mat4& eOpenGlRenderPipeline::GetProjectionMatrix()
{
	return renderManager->CameraInterpolationRender()->GetProjectionMatrix();
}

glm::mat4& eOpenGlRenderPipeline::GetLookAtProjectedMatrix()
{
	return renderManager->CameraInterpolationRender()->GetLookAtProjectedMatrix();
}

float& eOpenGlRenderPipeline::Metallic()
{
	return renderManager->SSRRenderer()->Metallic();
}

float& eOpenGlRenderPipeline::Spec()
{
	return renderManager->SSRRenderer()->Spec();
}

glm::vec4& eOpenGlRenderPipeline::Scale()
{
	return renderManager->SSRRenderer()->Scale();
}

float& eOpenGlRenderPipeline::K()
{
	return renderManager->SSRRenderer()->K();
}

//-----------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::RenderFrame(std::map<eObject::RenderType, std::vector<shObject>> _objects,
																				std::vector<Camera>& _cameras,
																				const Light& _light,
																				std::vector<std::shared_ptr<GUI>>& _guis,
																				std::vector<std::shared_ptr<Text>>& _texts)
{
	/*const */Camera& _camera = _cameras[0]; //@todo

	if (m_first_call) { RenderIBL(_camera); m_first_call = false; }

	std::vector<shObject> phong_objs = _objects.find(eObject::RenderType::PHONG)->second;
	std::vector<shObject> terrain_tes_objs = _objects.find(eObject::RenderType::TERRAIN_TESSELLATION)->second;
	std::vector<shObject> focused = _objects.find(eObject::RenderType::OUTLINED)->second;
	std::vector<shObject> pbr_objs = _objects.find(eObject::RenderType::PBR)->second;
	std::vector<shObject> flags = _objects.find(eObject::RenderType::FLAG)->second;
	std::vector<shObject> geometry_objs = _objects.find(eObject::RenderType::GEOMETRY)->second;
	std::vector<shObject> bezier_objs = _objects.find(eObject::RenderType::BEZIER_CURVE)->second;
	std::vector<shObject> lines_objs = _objects.find(eObject::RenderType::LINES)->second;
	std::vector<shObject> arealighted_objs = _objects.find(eObject::RenderType::AREA_LIGHT_ONLY)->second;

	auto comparator = [_camera](const shObject& obj1, const shObject& obj2)
	{ 
		if (float dist = glm::length2(_camera.getPosition() - obj1->GetTransform()->getTranslation())
									 - glm::length2(_camera.getPosition() - obj2->GetTransform()->getTranslation()); dist < 0)
			return true;
		else if (dist > 0)
			return false;
		else
			return obj1->Name() < obj2->Name();
	};

	//@todo should be sorted beforhead, stored in set not vector
	std::sort(focused.begin(), focused.end(), comparator);
	std::sort(phong_objs.begin(), phong_objs.end(), comparator);
	std::sort(pbr_objs.begin(), pbr_objs.end(), comparator);

	auto phong_pbr_objects = phong_objs;
	phong_pbr_objects.insert(phong_pbr_objects.end(), pbr_objs.begin(), pbr_objs.end());
	phong_pbr_objects.insert(phong_pbr_objects.end(), arealighted_objs.begin(), arealighted_objs.end());
	phong_pbr_objects.insert(phong_pbr_objects.end(), terrain_tes_objs.begin(), terrain_tes_objs.end());

	//Shadow Render Pass
	if (_light.type == eLightType::DIRECTION || _light.type == eLightType::SPOT)
	{
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW_DIR);
		if (shadows) { RenderShadows(_camera, _light, phong_pbr_objects); }
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW_DIR, GL_TEXTURE1);
	}
	else if (_light.type == eLightType::POINT)
	{
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW_CUBE_MAP);
		if (shadows) { RenderShadows(_camera, _light, phong_pbr_objects); }
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW_CUBE_MAP, GL_TEXTURE0);
	}
	else if(_light.type == eLightType::CSM)
	{
		renderManager->PhongRender()->SetShadowCascadeLevels(renderManager->CSMRender()->GetCascadeFlaneDistances());
		renderManager->PBRRender()->SetShadowCascadeLevels(renderManager->CSMRender()->GetCascadeFlaneDistances());
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW_CSM);
		if (shadows) { RenderShadowsCSM(_camera, _light, phong_pbr_objects); }
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW_CSM, GL_TEXTURE13);
	}

	//Rendering reflection and refraction to Textures
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFLECTION);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/*if (skybox)*/ { RenderSkybox(_camera); }

	glEnable(GL_CLIP_DISTANCE0); //?
	glEnable(GL_DEPTH_TEST);

	if (water)
	{
		RenderReflection(_camera, _light, phong_objs, pbr_objs);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFRACTION);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RenderRefraction(_camera, _light, phong_objs, pbr_objs);
	}
  glDisable(GL_CLIP_DISTANCE0);

	// SSAO
	if (ssao || ssr || camera_interpolation) // need g - buffer
	{
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFFERED);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderManager->SSAORender()->RenderGeometry(_camera, _light, phong_pbr_objects);

		if (ssao)
		{
			RenderSSAO(_camera, _light, phong_pbr_objects);
			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SSAO_BLUR).id);
		}
	}

	if(!ssao)
	{
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, Texture::GetTexture1x1(WHITE).id);
	}

		mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
			: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (sky_noise) { RenderSkyNoise(_camera); }

	glEnable(GL_STENCIL_TEST);
	//4. Rendering to main FBO with stencil
	if (outline_focused)
	{
		for (const auto& obj : focused)
		{
			if (obj->IsVisible())
			{
				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glStencilFunc(GL_ALWAYS, 1, 0xFF);
				glStencilMask(0xFF);

				if (std::find(phong_objs.begin(), phong_objs.end(), obj) != phong_objs.end())
					RenderMain(_camera, _light, { obj });
				else if (std::find(pbr_objs.begin(), pbr_objs.end(), obj) != pbr_objs.end())
					RenderPBR(_camera, _light, { obj });

				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
				glStencilMask(0x00);

				RenderOutlineFocused(_camera, _light, { obj });

				glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glStencilMask(0xFF);
			}
		}
	}
	else
		focused.clear();

	//Render not outlined objects
	{
		std::vector<shObject> not_outlined;
		std::set_difference(phong_objs.begin(), phong_objs.end(),
												focused.begin(), focused.end(),
												std::back_inserter(not_outlined), comparator);

		RenderMain(_camera, _light, not_outlined);

		not_outlined.clear();
		std::set_difference(pbr_objs.begin(), pbr_objs.end(),
												focused.begin(), focused.end(),
												std::back_inserter(not_outlined), comparator);

		RenderPBR(_camera, _light, not_outlined);

		RenderAreaLightsOnly(_camera, _light, arealighted_objs);

		RenderTerrainTessellated(_camera, _light, terrain_tes_objs);
	}

	//Mesh Line
	if(m_mesh_line_on)
		renderManager->MeshLineRender()->Render(_camera, _light, phong_pbr_objects); //@todo !!!!

	//Flags
	if (flags_on) { RenderFlags(_camera, _light, flags); }

	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	// Rendering WaterQuad
	if (water) { RenderWater(_camera, _light); }

	// Geometry
	if (geometry)
	{
		if (!geometry_objs.empty())
		{
			for (auto& obj : geometry_objs)
			{
				auto* mesh = dynamic_cast<const SimpleGeometryMesh*>(obj->GetModel()->GetMeshes()[0]);
				RenderGeometry(_camera, *mesh);
			}
		}
	}

	//Bezier
	if (bezier_curve)
	{
		if (!bezier_objs.empty())
		{
			std::vector<const BezierCurveMesh*> meshes;
			for (auto& bezier : bezier_objs)
			{
				auto* mesh = dynamic_cast<const BezierCurveMesh*>(bezier->GetModel()->GetMeshes()[0]);
				if (mesh)
					meshes.push_back(mesh);
			}
			renderManager->BezierRender()->Render(_camera, meshes);
		}
	}

	std::vector<const LineMesh*> line_meshes{};
	for (auto obj : lines_objs)
	{
		const LineMesh* mesh = dynamic_cast<const LineMesh*>(obj->GetModel()->GetMeshes()[0]);
		if (mesh)
			line_meshes.push_back(mesh);
	}
	// Visualize cameras
	if (_cameras.size() > 1)
	{
		static LineMesh camera_frustum_mesh({}, {}, {});
		std::vector<glm::vec3> extrems_total;
		std::vector<GLuint> indices_total;
		for (int i = 1; i < _cameras.size(); ++i)
		{
			if (_cameras[i].VisualiseFrustum())
			{
				std::vector<glm::vec4> cornders = _cameras[i].getFrustumCornersWorldSpace();
				for (auto& c : cornders)
					extrems_total.push_back(glm::vec3(c));
				indices_total.push_back(2 + (i - 1) * 8);
				indices_total.push_back(0 + (i - 1) * 8);
				indices_total.push_back(0 + (i - 1) * 8);
				indices_total.push_back(4 + (i - 1) * 8);
				indices_total.push_back(4 + (i - 1) * 8);
				indices_total.push_back(6 + (i - 1) * 8);
				indices_total.push_back(6 + (i - 1) * 8);
				indices_total.push_back(2 + (i - 1) * 8);

				indices_total.push_back(0 + (i - 1) * 8);
				indices_total.push_back(1 + (i - 1) * 8);
				indices_total.push_back(2 + (i - 1) * 8);
				indices_total.push_back(3 + (i - 1) * 8);
				indices_total.push_back(4 + (i - 1) * 8);
				indices_total.push_back(5 + (i - 1) * 8);
				indices_total.push_back(6 + (i - 1) * 8);
				indices_total.push_back(7 + (i - 1) * 8);

				indices_total.push_back(1 + (i - 1) * 8);
				indices_total.push_back(3 + (i - 1) * 8);
				indices_total.push_back(3 + (i - 1) * 8);
				indices_total.push_back(7 + (i - 1) * 8);
				indices_total.push_back(7 + (i - 1) * 8);
				indices_total.push_back(5 + (i - 1) * 8);
				indices_total.push_back(5 + (i - 1) * 8);
				indices_total.push_back(1 + (i - 1) * 8);
			}
		}
		camera_frustum_mesh.UpdateData(extrems_total, indices_total, { 1.0f, 1.0f, 0.0f, 1.0f });
		line_meshes.push_back(&camera_frustum_mesh);
	}

	// Bounding boxes
	if (draw_bounding_boxes)
	{
		static LineMesh bounding_boxes_mesh({}, {}, {});
		std::vector<glm::vec3> extrems_total;
		std::vector<GLuint> indices_total;

		for (GLuint i = 0; i < phong_pbr_objects.size(); ++i)
		{
			if (!phong_pbr_objects[i]->GetCollider() || !phong_pbr_objects[i]->GetTransform())
				continue;

			if (auto* terrain = dynamic_cast<ITerrainModel*>(phong_pbr_objects[i]->GetModel()); terrain)
			{
				auto indexSize = extrems_total.size();
				auto vec_of_extrems = terrain->GetExtremsOfMeshesLocalSpace();
				for (GLuint j = 0; j < vec_of_extrems.size(); ++j)
				{
					extrems_total.insert(extrems_total.end(), vec_of_extrems[j].begin(), vec_of_extrems[j].end());
					indices_total.push_back(indexSize + 0 + j * 8);
					indices_total.push_back(indexSize + 1 + j * 8);
					indices_total.push_back(indexSize + 1 + j * 8);
					indices_total.push_back(indexSize + 2 + j * 8);
					indices_total.push_back(indexSize + 2 + j * 8);
					indices_total.push_back(indexSize + 3 + j * 8);
					indices_total.push_back(indexSize + 3 + j * 8);
					indices_total.push_back(indexSize + 0 + j * 8);
					indices_total.push_back(indexSize + 4 + j * 8);
					indices_total.push_back(indexSize + 5 + j * 8);
					indices_total.push_back(indexSize + 5 + j * 8);
					indices_total.push_back(indexSize + 6 + j * 8);
					indices_total.push_back(indexSize + 6 + j * 8);
					indices_total.push_back(indexSize + 7 + j * 8);
					indices_total.push_back(indexSize + 7 + j * 8);
					indices_total.push_back(indexSize + 4 + j * 8);
					indices_total.push_back(indexSize + 0 + j * 8);
					indices_total.push_back(indexSize + 4 + j * 8);
					indices_total.push_back(indexSize + 1 + j * 8);
					indices_total.push_back(indexSize + 5 + j * 8);
					indices_total.push_back(indexSize + 2 + j * 8);
					indices_total.push_back(indexSize + 6 + j * 8);
					indices_total.push_back(indexSize + 3 + j * 8);
					indices_total.push_back(indexSize + 7 + j * 8);
				}
			}
			else
			{
				std::vector<glm::vec3> extrems = phong_pbr_objects[i]->GetCollider()->GetExtrems(*phong_pbr_objects[i]->GetTransform());
				extrems_total.insert(extrems_total.end(), extrems.begin(), extrems.end());
				indices_total.push_back(0 + i * 8);
				indices_total.push_back(1 + i * 8);
				indices_total.push_back(1 + i * 8);
				indices_total.push_back(2 + i * 8);
				indices_total.push_back(2 + i * 8);
				indices_total.push_back(3 + i * 8);
				indices_total.push_back(3 + i * 8);
				indices_total.push_back(0 + i * 8);
				indices_total.push_back(4 + i * 8);
				indices_total.push_back(5 + i * 8);
				indices_total.push_back(5 + i * 8);
				indices_total.push_back(6 + i * 8);
				indices_total.push_back(6 + i * 8);
				indices_total.push_back(7 + i * 8);
				indices_total.push_back(7 + i * 8);
				indices_total.push_back(4 + i * 8);
				indices_total.push_back(0 + i * 8);
				indices_total.push_back(4 + i * 8);
				indices_total.push_back(1 + i * 8);
				indices_total.push_back(5 + i * 8);
				indices_total.push_back(2 + i * 8);
				indices_total.push_back(6 + i * 8);
				indices_total.push_back(3 + i * 8);
				indices_total.push_back(7 + i * 8);
			}
		}
		bounding_boxes_mesh.UpdateData(extrems_total, indices_total, { 1.0f, 1.0f, 0.0f, 1.0f});
		line_meshes.push_back(&bounding_boxes_mesh);
	}
	renderManager->LinesRender()->Render(_camera, line_meshes);

	//  Draw skybox firs
	if (skybox)
	{
		RenderSkybox(_camera);
	}
	glDepthFunc(GL_LEQUAL);

	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	//7  Particles
	if (particles) { RenderParticles(_camera); }

	if (mts) // resolving mts to default
	{
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SCREEN);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		eGlBufferContext::GetInstance().ResolveMtsToScreen();

		// screen + ssr
		if (ssr)
		{
			RenderSSR(_camera);

			//blur @todo blur
			eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SSR_BLUR);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SSR, GL_TEXTURE2);
			renderManager->SSRRenderer()->RenderSSRBlur(_camera);//@todo separate render

			//@todo mix ssr with ssr blured based on roughness mask and use mixed texture

			// mix ssr with screen buffer
			eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SCREEN_WITH_SSR);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderManager->ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
			renderManager->ScreenRender()->SetTextureContrast(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SSR_BLUR));
			renderManager->ScreenRender()->SetTextureMask(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN_MASK));
			renderManager->ScreenRender()->SetRenderingFunction(GUI::RenderFunc::MaskBlend);
			renderManager->ScreenRender()->Render({ 0,0 }, { width, height },
																						{ 0,height }, { width, 0 }, //@todo y is inverted
																						(float)width, (float)height);
		}

		Texture screen = ssr ? eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN_WITH_SSR)
												 : eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN);

		Texture contrast;
		if (m_pb_bloom)//Physicly Based Bloom
		{
			RenderBloom();
			glViewport(0, 0, width, height);
			contrast = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BLOOM);
		}
		else
		{
			RenderBlur(_camera); // gaussian
			contrast = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO);
		}

		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glViewport(0, 0, width, height);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		RenderContrast(_camera, screen, contrast); // blend screen with gaussian (or pb bloom) -> to default
	}
	
	//Post-processing, need stencil
	if (kernel)
	{
		eGlBufferContext::GetInstance().BlitFromTo(eBuffer::BUFFER_SCREEN, eBuffer::BUFFER_DEFAULT, GL_STENCIL_BUFFER_BIT);
		glEnable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_EQUAL, 1, 0xFF);
		glStencilMask(0xFF);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		renderManager->ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
		renderManager->ScreenRender()->RenderKernel();
		glClear(GL_STENCIL_BUFFER_BIT);
		glDisable(GL_STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilMask(0xFF);
	}

	if (camera_interpolation)
		RenderCameraInterpolationCompute(_camera);

	//8.1 Texture visualization
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	if(mousepress  && _camera.getCameraRay().IsPressed())
	{
		renderManager->ScreenRender()->RenderFrame(_camera.getCameraRay().GetFrame().first,
																							 _camera.getCameraRay().GetFrame().second, 
																							 static_cast<float>(width), 
																							 static_cast<float>(height));
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderManager->TextRender()->RenderText(_camera, _texts, static_cast<float>(width), static_cast<float>(height));
	glDisable(GL_BLEND);

	RenderGui(_guis, _camera);

	m_draw_calls = eGlDrawContext::GetInstance().GetDrawCallsCount();
	eGlDrawContext::GetInstance().Flush();

	if (m_compute_shader)
	{
		renderManager->ComputeShaderRender()->DispatchCompute(_camera);
		// set up buffer for writing
		eGlBufferContext::GetInstance().EnableCustomWrittingBuffer("ComputeParticleSystemBuffer");
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		renderManager->ComputeShaderRender()->RenderComputeResult(_camera);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	//render final texture to screen
	/*Texture total_screen = GetDefaultBufferTexture();
	renderManager->ScreenRender()->SetTexture(total_screen);
	renderManager->ScreenRender()->Render({ 0,0 }, { width, height },
																				{ 0,0 }, { width, height },
																				width, height);*/
}

void eOpenGlRenderPipeline::RenderShadows(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	// Bind the "depth only" FBO and set the viewport to the size of the depth texture
	glm::ivec2 size = _light.type == eLightType::POINT ? eGlBufferContext::GetInstance().GetSize(eBuffer::BUFFER_SHADOW_CUBE_MAP)
																										 : eGlBufferContext::GetInstance().GetSize(eBuffer::BUFFER_SHADOW_DIR);
	glViewport(0, 0, size.x, size.y);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// Clear
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glClearDepth(1.0f);
	// Enable polygon offset to resolve depth-fighting isuses 
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(2.0f, -2000.0f);

	renderManager->ShadowRender()->Render(_camera, _light, _objects);

	//glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	glViewport(0, 0, width, height);
}

void eOpenGlRenderPipeline::RenderShadowsCSM(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	glm::ivec2 size = eGlBufferContext::GetInstance().GetSize(eBuffer::BUFFER_SHADOW_CSM);
	glViewport(0, 0, size.x, size.y);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	// Clear
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	//glClearDepth(1.0f);
// Enable polygon offset to resolve depth-fighting isuses 
//glEnable(GL_POLYGON_OFFSET_FILL);
//glPolygonOffset(2.0f, -2000.0f);

	glEnable(GL_DEPTH_CLAMP);
	renderManager->CSMRender()->Render(_camera, _light, _objects);
	glDisable(GL_DEPTH_CLAMP);

	//glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	glViewport(0, 0, width, height);
}

void eOpenGlRenderPipeline::RenderSkybox(const Camera& _camera)
{
	glDepthFunc(GL_LEQUAL);
	renderManager->SkyBoxRender()->Render(_camera);
  glDepthFunc(GL_LESS);
}

void eOpenGlRenderPipeline::RenderReflection(Camera& _camera, const Light& _light, std::vector<shObject>& _phong_objects, std::vector<shObject>& _pbr_objects)
{
	renderManager->PhongRender()->SetClipPlane(-waterHeight);

	Camera temp_cam = _camera;
	_camera.setPosition(glm::vec3(temp_cam.getPosition().x, waterHeight - (temp_cam.getPosition().y - waterHeight), temp_cam.getPosition().z));
	_camera.setDirection(glm::reflect(_camera.getDirection(), glm::vec3(0, 1, 0))); //water normal

	renderManager->PhongRender()->Render(_camera, _light, _phong_objects);
	renderManager->PBRRender()->Render(_camera, _light, _pbr_objects);
	_camera = temp_cam;
}

void eOpenGlRenderPipeline::RenderRefraction(Camera& _camera, const Light& _light, std::vector<shObject>& _phong_objects, std::vector<shObject>& _pbr_objects)
{
	renderManager->PhongRender()->SetClipPlane(waterHeight);
	renderManager->PhongRender()->Render(_camera, _light, _phong_objects);
	renderManager->PBRRender()->Render(_camera, _light, _pbr_objects);
	renderManager->PhongRender()->SetClipPlane(10);

	//glDisable(GL_CLIP_DISTANCE0);
}

void eOpenGlRenderPipeline::RenderSkyNoise(const Camera& _camera)
{
	//sky noise
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	renderManager->SkyNoiseRender()->Render(_camera);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void eOpenGlRenderPipeline::RenderMain(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
	renderManager->PhongRender()->Render(_camera, _light, _objects);
}

void eOpenGlRenderPipeline::RenderAreaLightsOnly(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
	renderManager->AreaLightsRender()->Render(_camera, _light, _objects);
}

void eOpenGlRenderPipeline::RenderOutlineFocused(const Camera& _camera, const Light& _light, const std::vector<shObject>& focused)
{
	//5. Rendering Stencil Outlineing
	if(!focused.empty())
	{
		renderManager->OutlineRender()->Render(_camera, _light, focused);
	}
}

void eOpenGlRenderPipeline::RenderFlags(const Camera& _camera, const Light& _light, std::vector<shObject> flags)
{
	renderManager->PhongRender()->RenderWaves(_camera, _light, flags);
}

void eOpenGlRenderPipeline::RenderWater(const Camera& _camera, const Light& _light)
{
	glDisable(GL_CULL_FACE);
	renderManager->WaterRender()->Render(_camera, _light);
	glEnable(GL_CULL_FACE);
}

void eOpenGlRenderPipeline::RenderGeometry(const Camera& _camera, const SimpleGeometryMesh& _mesh)
{
	glm::mat4 MVP = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
	renderManager->HexRender()->Render(MVP, const_cast<SimpleGeometryMesh&>(_mesh));//@ Draw is not const func unfortunately
}

void eOpenGlRenderPipeline::RenderParticles(const Camera& _camera)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glCullFace(GL_TRUE);
	renderManager->ParticleRender()->Render(_camera);
	if(renderManager->ParticleRenderGPU())
		renderManager->ParticleRenderGPU()->Render(_camera);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

//-------------------------------------------------------
void eOpenGlRenderPipeline::RenderBloom()
{
	// Bind srcTexture (HDR color buffer) as initial texture input
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_BLOOM);
	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // ?
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SCREEN, GL_TEXTURE0);
	renderManager->BloomRenderer()->RenderDownsamples(eGlBufferContext::GetInstance().MipChain(), glm::vec2{ (float)width , (float)height });

	// Enable additive blending !!!
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	renderManager->BloomRenderer()->RenderUpsamples(eGlBufferContext::GetInstance().MipChain());
	// Disable additive blending !!!!!
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA); // Restore if this was default
	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void eOpenGlRenderPipeline::RenderTerrainTessellated(const Camera& _camera, const Light& _light, std::vector<shObject> _objs)
{
	renderManager->TerrainTessellatedRender()->Render(_camera, _light, _objs);
}

void eOpenGlRenderPipeline::RenderBlur(const Camera& _camera)
{
	renderManager->BrightFilterRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_BRIGHT_FILTER);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	renderManager->BrightFilterRender()->Render();
	renderManager->GaussianBlurRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER));
	renderManager->GaussianBlurRender()->Render();
}

//------------------------------------------------
void eOpenGlRenderPipeline::RenderContrast(const Camera& _camera, const Texture& _screen, const Texture& _contrast)
{
	renderManager->ScreenRender()->SetTexture(_screen);
	renderManager->ScreenRender()->SetTextureContrast( _contrast);
	renderManager->ScreenRender()->RenderContrast(_camera, blur_coef);
}

//------------------------------------------------
void eOpenGlRenderPipeline::RenderGui(std::vector<std::shared_ptr<GUI>>& guis, const Camera& _camera)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //todo@ for real transparency

	for(auto& gui : guis)
	{
		if (gui->GetTexture() && gui->IsVisible())
		{
			if (gui->IsTransparent())
				glEnable(GL_BLEND);
			else
				glDisable(GL_BLEND);

			renderManager->ScreenRender()->SetTexture(*(gui->GetTexture())); //copy texture
			renderManager->ScreenRender()->SetTextureMask(*(gui->GetTextureMask()));
			renderManager->ScreenRender()->SetRenderingFunction(gui->GetRenderingFunc());
			renderManager->ScreenRender()->Render(gui->getTopLeft(),				gui->getBottomRight(),
																						gui->getTopLeftTexture(), gui->getBottomRightTexture(),
																						static_cast<float>(width), static_cast<float>(height));
		}

		gui->UpdateSync();
		for (auto& child : gui->GetChildren())
		{
			if (child->GetTexture() && child->IsVisible())
			{
				if (child->IsTransparent())
					glEnable(GL_BLEND);
				else
					glDisable(GL_BLEND);

				renderManager->ScreenRender()->SetTexture(*(child->GetTexture())); //copy texture
				renderManager->ScreenRender()->SetTextureMask(*(child->GetTextureMask()));
				renderManager->ScreenRender()->Render(child->getTopLeft(), child->getBottomRight(), 
																							child->getTopLeftTexture(), child->getBottomRightTexture(), 
																							static_cast<float>(width), static_cast<float>(height));
			}
			child->UpdateSync();
		}
	}
	glDisable(GL_BLEND);
}

//------------------------------------------------
void eOpenGlRenderPipeline::RenderPBR(const Camera& _camera, const Light& _light, std::vector<shObject> _objs)
{
	if (!_objs.empty())
	{
		renderManager->PBRRender()->Render(_camera, _light, _objs);
	}
}

//-------------------------------------------------------
void eOpenGlRenderPipeline::RenderSSAO(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SSAO);
	glClear(GL_COLOR_BUFFER_BIT);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED, GL_TEXTURE2);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED1, GL_TEXTURE3);
	renderManager->SSAORender()->RenderSSAO(_camera);
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SSAO_BLUR);
	glClear(GL_COLOR_BUFFER_BIT);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SSAO, GL_TEXTURE2);
	renderManager->SSAORender()->RenderSSAOBlur(_camera);//@to separate render
}

//-------------------------------------------------------
void eOpenGlRenderPipeline::RenderSSR(const Camera& _camera)
{
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SSR);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED, GL_TEXTURE2);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED1, GL_TEXTURE3);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SCREEN_MASK, GL_TEXTURE4);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SCREEN, GL_TEXTURE5);
	renderManager->SSRRenderer()->Render(_camera);
}

//-------------------------------------------------------
void eOpenGlRenderPipeline::RenderIBL(const Camera& _camera)
{
	glDisable(GL_CULL_FACE);

	// @todo should it be this framebuffer ?
	//Pre-computing the BRDF
// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, eGlBufferContext::GetInstance().GetId(eBuffer::BUFFER_IBL_CUBEMAP));
	glBindRenderbuffer(GL_RENDERBUFFER, eGlBufferContext::GetInstance().GetRboID(eBuffer::BUFFER_IBL_CUBEMAP));
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager->IBLRender()->GetLUTTextureID(), 0);

	glViewport(0, 0, 512, 512);
	renderManager->IBLRender()->RenderBrdf();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, renderManager->IBLRender()->GetLUTTextureID());

	for (int i = 0; i < renderManager->IBLRender()->HDRCount(); ++i)
	{
		Texture cube, irr, prefilter;
		// load hdr env
		glViewport(0, 0, (GLsizei)512, (GLsizei)512); //@todo numbers // don't forget to configure the viewport to the capture dimensions.
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_IBL_CUBEMAP);
		auto cube_id = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP).id;
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
		renderManager->IBLRender()->RenderCubemap(_camera, cube_id); // write hdr texture into cube_id

		cube.makeCubemap(512, false); //@todo works only the first time, does not second @bugfix
		glCopyImageSubData(cube_id, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			cube.id, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			512, 512, 6); //copy from buffer to texture

   // irradiance
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);

		glViewport(0, 0, 32, 32);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_IBL_CUBEMAP_IRR);
		auto irr_id = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP_IRR).id;
		renderManager->IBLRender()->RenderIBLMap(_camera, irr_id); // write irr to irr buffer
		irr.makeCubemap(32);
		glCopyImageSubData(irr_id, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			irr.id, GL_TEXTURE_CUBE_MAP, 0, 0, 0, 0,
			32, 32, 6); //copy from buffer to texture

   // dots artifacts
		glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

		// prefilter
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_IBL_CUBEMAP, GL_TEXTURE2);
		//glActiveTexture(GL_TEXTURE2); //any
		//glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
		prefilter.makeCubemap(128, true);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_IBL_CUBEMAP);
		renderManager->IBLRender()->RenderPrefilterMap(_camera, prefilter.id, eGlBufferContext::GetInstance().GetRboID(eBuffer::BUFFER_IBL_CUBEMAP));

		//set textures to pbr
		SetSkyIBL(irr.id, prefilter.id);

		glViewport(0, 0, width, height);

		m_texture_manager->AddExisting("cube_id"+ std::to_string(i), &cube);
		m_texture_manager->AddCubeMapId(cube.id);
		m_texture_manager->AddExisting("irr_id" + std::to_string(i), &irr);
		m_texture_manager->AddCubeMapId(irr.id);
		m_texture_manager->AddExisting("prefilter" + std::to_string(i), &prefilter);
		m_texture_manager->AddCubeMapId(prefilter.id);
		m_texture_manager->AddIBLId(irr.id, prefilter.id);
	}

	glEnable(GL_CULL_FACE);
}

//-------------------------------------------------------
bool& eOpenGlRenderPipeline::GetRotateSkyBoxRef()
{
	return renderManager->SkyBoxRender()->GetRotateSkyBoxRef();
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetDefaultBufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_DEFAULT);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetReflectionBufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetRefractionBufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetShadowBufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SHADOW_DIR);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetGausian1BufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_ONE);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetGausian2BufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetMtsBufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_MTS);
}

//-------------------------------------------------------
Texture eOpenGlRenderPipeline::GetScreenBufferTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetBrightFilter() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetSSAO() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SSAO_BLUR);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetDefferedOne() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_DEFFERED);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetDefferedTwo() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_DEFFERED1);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetHdrCubeMap() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetLUT() const
{
	return Texture(renderManager->IBLRender()->GetLUTTextureID(), 512, 512, 3);
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetCSMMapLayer1() const{return csm_dump1;}
Texture eOpenGlRenderPipeline::GetCSMMapLayer2() const{return csm_dump2;}
Texture eOpenGlRenderPipeline::GetCSMMapLayer3() const{return csm_dump3;}
Texture eOpenGlRenderPipeline::GetCSMMapLayer4() const{return csm_dump4;}
Texture eOpenGlRenderPipeline::GetCSMMapLayer5() const{return csm_dump5;}

Texture eOpenGlRenderPipeline::GetBloomTexture() const
{ 
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BLOOM);
}

Texture eOpenGlRenderPipeline::GetSSRTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SSR);
}

Texture eOpenGlRenderPipeline::GetSSRWithScreenTexture() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN_WITH_SSR);
}

Texture eOpenGlRenderPipeline::GetSSRTextureScreenMask() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN_MASK);
}

Texture eOpenGlRenderPipeline::GetCameraInterpolationCoords() const
{
	return eGlBufferContext::GetInstance().GetTexture("CameraInterpolationCoordsBuffer");
}

Texture eOpenGlRenderPipeline::GetComputeParticleSystem() const
{
	return eGlBufferContext::GetInstance().GetTexture("ComputeParticleSystemBuffer");
}

void eOpenGlRenderPipeline::DumpCSMTextures() const
{
	Texture t = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SHADOW_CSM);
	//t.saveToFile("dump_csm.png", GL_TEXTURE_2D_ARRAY, GL_DEPTH_COMPONENT, GL_FLOAT);

	static std::vector<GLfloat> buffer(2400 * 1200 * 5);
	glBindTexture(GL_TEXTURE_2D_ARRAY, t.id);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, &buffer[0]);

	Texture* csmp1 = const_cast<Texture*>(&csm_dump1);
	csmp1->TextureFromBuffer<GLfloat>((GLfloat*)&buffer[0], 2400 ,1200, GL_RED);

	Texture* csmp2 = const_cast<Texture*>(&csm_dump2);
	csmp2->TextureFromBuffer<GLfloat>((GLfloat*)&buffer[2400 * 1200], 2400, 1200, GL_RED);

	Texture* csmp3 = const_cast<Texture*>(&csm_dump3);
	csmp3->TextureFromBuffer<GLfloat>((GLfloat*)&buffer[2400 * 1200 * 2], 2400, 1200, GL_RED);

	Texture* csmp4 = const_cast<Texture*>(&csm_dump4);
	csmp4->TextureFromBuffer<GLfloat>((GLfloat*)&buffer[2400 * 1200 * 3], 2400, 1200, GL_RED);

	Texture* csmp5 = const_cast<Texture*>(&csm_dump5);
	csmp5->TextureFromBuffer<GLfloat>((GLfloat*)&buffer[2400 * 1200 * 4], 2400, 1200, GL_RED);
}

//-------------------------------------------------------
void eOpenGlRenderPipeline::RenderCameraInterpolationCompute(const Camera& _camera)
{
	glViewport(0, 0, width, height);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFAULT, GL_TEXTURE1);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED, GL_TEXTURE2);
	renderManager->CameraInterpolationRender()->DispatchCompute(_camera);
}


#include <cmath>

float Lerp(int start, int end, float t)
{
	// Clamp t to the range [0, 1]
	t = std::max(0.0f, std::min(1.0f, t));

	// Perform linear interpolation
	return static_cast<float>(start) + t * (static_cast<float>(end) - static_cast<float>(start));
}

//-------------------------------------------------------
Texture* eOpenGlRenderPipeline::RenderCameraInterpolation(const Camera& _camera)
{
	static std::vector<GLfloat> buffer_image(width * height * 4);
	static std::vector<GLfloat> buffer_coords(width * height * 4);
	static std::vector<GLfloat> buffer_new_image(width * height * 4);
	static Texture new_image_texture;

	eGlBufferContext::GetInstance().EnableCustomWrittingBuffer("CameraInterpolationCoordsBuffer");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED, GL_TEXTURE2);
	renderManager->CameraInterpolationRender()->Render(_camera);

	if (ssr)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SCREEN_WITH_SSR, GL_TEXTURE2);
	else
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SCREEN, GL_TEXTURE2);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &buffer_image[0]);

	eGlBufferContext::GetInstance().EnableCustomReadingBuffer("CameraInterpolationCoordsBuffer", GL_TEXTURE3);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, &buffer_coords[0]);

	for (uint32_t row = 0; row < height; ++row)
	{
		for (uint32_t column = 0; column < width; ++column)
		{
			float r = buffer_image[row * width * 4 + column*4];
			float g = buffer_image[row * width * 4 + column*4 +1];
			float b = buffer_image[row * width * 4 + column*4 +2];
			float a = 1.f;

			float new_coord_x = buffer_coords[row * width * 4 + column * 4];
			float new_coord_y = buffer_coords[row * width * 4 + column * 4 + 1];
			float new_coord_z = buffer_coords[row * width * 4 + column * 4 + 2];
			float new_coord_a = buffer_coords[row * width * 4 + column * 4 + 3];

			int new_coord_x_int = static_cast<int>(std::round(Lerp(0, width, new_coord_x)));
			int new_coord_y_int = static_cast<int>(std::round(Lerp(0, height, new_coord_y)));

			int index = new_coord_y_int * width * 4 + new_coord_x_int * 4;
			if (index < buffer_new_image.size()-3 && index >= 0)
			{
				buffer_new_image[new_coord_y_int * width * 4 + new_coord_x_int *4] = r;
				buffer_new_image[new_coord_y_int * width * 4 + new_coord_x_int *4 + 1] = g;
				buffer_new_image[new_coord_y_int * width * 4 + new_coord_x_int *4 + 2] = b;
				buffer_new_image[new_coord_y_int * width * 4 + new_coord_x_int *4 + 3] = a;
			}
		}
	}
	new_image_texture.TextureFromBuffer<GLfloat>((GLfloat*)&buffer_new_image[0], width, height, GL_RGBA);
	return &new_image_texture;
}
