#include "stdafx.h"
#include "OpenGlRenderPipeline.h"

#include <math/Camera.h>
#include "GUI.h"

#include "GlBufferContext.h"

#include "RenderManager.h"
#include "TextureManager.h"
#include "ModelManager.h"

#include <algorithm>

void	eOpenGlRenderPipeline::SetSkyBoxTexture(const Texture* _t)
{ renderManager->SkyBoxRender()->SetSkyBoxTexture(_t); }

void eOpenGlRenderPipeline::AddParticleSystem(std::shared_ptr<IParticleSystem> _system)
{
	renderManager->AddParticleSystem(_system);
}

void eOpenGlRenderPipeline::AddParticleSystemGPU(glm::vec3 _startPos, const Texture* _texture)
{
	renderManager->AddParticleSystemGPU(_startPos, _texture);
}

const std::vector<ShaderInfo>& eOpenGlRenderPipeline::GetShaderInfos() const
{ return renderManager->GetShaderInfos(); }

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

eOpenGlRenderPipeline::~eOpenGlRenderPipeline()
{
}

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

void eOpenGlRenderPipeline::InitializeBuffers(bool _needsShadowCubeMap)
{
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFAULT, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_MTS, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFLECTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFRACTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW, width*2, height*2, _needsShadowCubeMap);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SQUERE, height, height); //squere
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_BRIGHT_FILTER, width, height);
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_GAUSSIAN_ONE, 600, 300); //@todo numbers
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_GAUSSIAN_TWO, 600, 300);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SSAO, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SSAO_BLUR, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_IBL_CUBEMAP, 512, 512);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_IBL_CUBEMAP_IRR, 32, 32);
}

//-----------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::InitializeRenders(eModelManager& modelManager, eTextureManager& texManager, const std::string& shadersFolderPath)
{
	renderManager->Initialize(modelManager, texManager, shadersFolderPath);
}

//-----------------------------------------------------------------------------------------------
Texture eOpenGlRenderPipeline::GetSkyNoiseTexture(const Camera& _camera)
{
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SQUERE);
	RenderSkyNoise(_camera); //do we need camera?
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SQUERE);
}

//-----------------------------------------------------------------------------------------------
void eOpenGlRenderPipeline::RenderFrame(std::map<eObject::RenderType, std::vector<shObject>> _objects,
																				Camera& _camera,
																				const Light& _light,
																				std::vector<std::shared_ptr<GUI>>& _guis,
																				std::vector<std::shared_ptr<Text>>& _texts)
{
  /*std::sort(focused.begin(), focused.end(), [_camera](const shObject& obj1, const shObject& obj2)
    { return glm::length2(_camera.getPosition() - obj1->GetTransform()->getTranslation())
    > glm::length2(_camera.getPosition() - obj2->GetTransform()->getTranslation()); });*/

	if (m_first_call) { RenderIBL(_camera); m_first_call = false; }

	std::vector<shObject> phong_objs = _objects.find(eObject::RenderType::PHONG)->second;
	std::vector<shObject> focused = _objects.find(eObject::RenderType::OUTLINED)->second;
	std::vector<shObject> pbr_objs = _objects.find(eObject::RenderType::PBR)->second;
	std::vector<shObject> flags = _objects.find(eObject::RenderType::FLAG)->second;
	std::vector<shObject> geometry_objs = _objects.find(eObject::RenderType::GEOMETRY)->second;
	std::vector<shObject> bezier_objs = _objects.find(eObject::RenderType::BEZIER_CURVE)->second;
	auto phong_pbr_objects = phong_objs;
	phong_pbr_objects.insert(phong_pbr_objects.end(), pbr_objs.begin(), pbr_objs.end());

	//Shadow Render Pass
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);
	if (shadows) { RenderShadows(_camera, _light, phong_pbr_objects); }

	if (_light.type == eLightType::DIRECTION || _light.type == eLightType::SPOT)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE1);
	else if (_light.type == eLightType::POINT)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE0);

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
	if (ssao)
	{
		RenderSSAO(_camera, _light, phong_pbr_objects);
		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SSAO_BLUR).id);
	}
	else
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
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			if (std::find(phong_objs.begin(), phong_objs.end(), obj) != phong_objs.end())
				RenderMain(_camera, _light, { obj });
			else
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
	else
		focused.clear();

	//Render not outlined objects
	{
		std::vector<shObject> not_outlined;
		std::set_difference(phong_objs.begin(), phong_objs.end(),
			focused.begin(), focused.end(),
			std::back_inserter(not_outlined),
			[](auto& a, auto& b) { return &a < &b; });

		RenderMain(_camera, _light, not_outlined);

		not_outlined.clear();
		std::set_difference(pbr_objs.begin(), pbr_objs.end(),
			focused.begin(), focused.end(),
			std::back_inserter(not_outlined),
			[](auto& a, auto& b) { return &a < &b; });

		RenderPBR(_camera, _light, not_outlined);
	}

	//Mesh Line
	renderManager->MeshLineRender()->Render(_camera, _light, phong_pbr_objects);

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

	// Bounding boxes
	if (draw_bounding_boxes)
	{
		std::vector<glm::vec3> extrems_total;
		std::vector<GLuint> indices_total;
		for (GLuint i = 0; i < phong_pbr_objects.size(); i++)
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
		renderManager->LinesRender()->Render(_camera, extrems_total, indices_total);
	}

	//  Draw skybox firs
	if (skybox)
	{
		RenderSkybox(_camera);
	}
	glDepthFunc(GL_LEQUAL);

	//7  Particles
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	if (particles) { RenderParticles(_camera); }

	if (mts) // resolving mts to default
	{
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SCREEN);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		eGlBufferContext::GetInstance().ResolveMtsToScreen();
		RenderBlur(_camera);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glViewport(0, 0, width, height);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		RenderContrast(_camera);
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

	RenderGui(_guis, _camera);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderManager->TextRender()->RenderText(_camera, _texts, width, height);
	glDisable(GL_BLEND);

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
	glm::ivec2 size = eGlBufferContext::GetInstance().GetSize(eBuffer::BUFFER_SHADOW);
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

	glDisable(GL_POLYGON_OFFSET_FILL);
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
	renderManager->MainRender()->SetClipPlane(-waterHeight);

	Camera tem_cam = _camera;
	_camera.setPosition(glm::vec3(tem_cam.getPosition().x, 2 * (tem_cam.getPosition().y - waterHeight), tem_cam.getPosition().z));
	_camera.setDirection(glm::reflect(_camera.getDirection(), glm::vec3(0, 1, 0))); //water normal

	renderManager->MainRender()->Render(_camera, _light, _phong_objects, false, false, true, tone_mapping, exposure);
	renderManager->PBRRender()->Render(_camera, _light, _pbr_objects);
	_camera = tem_cam;
}

void eOpenGlRenderPipeline::RenderRefraction(Camera& _camera, const Light& _light, std::vector<shObject>& _phong_objects, std::vector<shObject>& _pbr_objects)
{
	renderManager->MainRender()->SetClipPlane(waterHeight);
	renderManager->MainRender()->Render(_camera, _light, _phong_objects, false, false, true, tone_mapping, exposure);
	renderManager->PBRRender()->Render(_camera, _light, _pbr_objects);
	renderManager->MainRender()->SetClipPlane(10);

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
	renderManager->MainRender()->Render(_camera, _light, _objects, debug_white, debug_texcoords, gamma_correction, tone_mapping, exposure);
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
	renderManager->WaveRender()->Render(_camera, _light, flags);
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
	renderManager->ParticleRenderGPU()->Render(_camera);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
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

void eOpenGlRenderPipeline::RenderContrast(const Camera& _camera)
{
	renderManager->ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	renderManager->ScreenRender()->SetTextureContrast(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO));
	renderManager->ScreenRender()->RenderContrast(_camera, blur_coef);
}

void eOpenGlRenderPipeline::RenderGui(std::vector<std::shared_ptr<GUI>>& guis, const Camera& _camera)
{
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

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
			renderManager->ScreenRender()->Render(gui->getTopLeft(), gui->getBottomRight(),
																						gui->getTopLeftTexture(), gui->getBottomRightTexture(),
																						width, height);
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
																							width, height);
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
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFFERED);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	renderManager->SSAORender()->RenderGeometry(_camera, _light, _objects);
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SSAO);
	glClear(GL_COLOR_BUFFER_BIT);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED, GL_TEXTURE2);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_DEFFERED1, GL_TEXTURE3);
	renderManager->SSAORender()->RenderSSAO(_camera);
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SSAO_BLUR);
	glClear(GL_COLOR_BUFFER_BIT);
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SSAO, GL_TEXTURE2);
	renderManager->SSAORender()->RenderSSAOBlur(_camera);
}

//-------------------------------------------------------
void eOpenGlRenderPipeline::RenderIBL(const Camera& _camera)
{
	glDisable(GL_CULL_FACE);
	// load hdr env
	glViewport(0, 0, (GLsizei)512, (GLsizei)512); //@todo numbers // don't forget to configure the viewport to the capture dimensions.
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_IBL_CUBEMAP);
	auto cube_id = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP).id;
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
	renderManager->IBLRender()->RenderCubemap(_camera, cube_id);

	//irradiance
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
	glViewport(0, 0, 32, 32);
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_IBL_CUBEMAP_IRR);
	auto irr_id = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP_IRR).id;
	renderManager->IBLRender()->RenderIBLMap(_camera, irr_id);

	//dots artifacts
	glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// prefilter
	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_IBL_CUBEMAP, GL_TEXTURE2);
	//glActiveTexture(GL_TEXTURE2); //any
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cube_id);
	Texture prefilter; //free ?
	prefilter.makeCubemap(128, true);
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_IBL_CUBEMAP);
	renderManager->IBLRender()->RenderPrefilterMap(_camera, prefilter.id, eGlBufferContext::GetInstance().GetRboID(eBuffer::BUFFER_IBL_CUBEMAP));

	//Pre-computing the BRDF
	// then re-configure capture framebuffer object and render screen-space quad with BRDF shader.
	glBindFramebuffer(GL_FRAMEBUFFER, eGlBufferContext::GetInstance().GetId(eBuffer::BUFFER_IBL_CUBEMAP));
	glBindRenderbuffer(GL_RENDERBUFFER, eGlBufferContext::GetInstance().GetRboID(eBuffer::BUFFER_IBL_CUBEMAP));
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderManager->IBLRender()->GetLUTTextureID(), 0);

	glViewport(0, 0, 512, 512);
	renderManager->IBLRender()->RenderBrdf();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	//set textures to pbr
	glActiveTexture(GL_TEXTURE9);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irr_id);
	
	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilter.id);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, renderManager->IBLRender()->GetLUTTextureID());

	glViewport(0, 0, width, height);
	/*static Texture skybox = eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP_IRR);
	renderManager->SkyBoxRender()->SetSkyBoxTexture(&skybox);*/
	glEnable(GL_CULL_FACE);
}

//-------------------------------------------------------
bool& eOpenGlRenderPipeline::GetRotateSkyBox()
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
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SHADOW);
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
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SSAO);
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
Texture eOpenGlRenderPipeline::GetHdrTexture() const
{
	return renderManager->IBLRender()->GetHdrTexture();
}

//----------------------------------------------------
Texture eOpenGlRenderPipeline::GetHdrCubeMap() const
{
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_IBL_CUBEMAP);
}
