#include "stdafx.h"
#include "OpenGlRenderPipeline.h"

#include <math/Camera.h>
#include "GUI.h"

#include "GlBufferContext.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "RenderManager.h"

#include "WaterRender.h"
#include "SkyBoxRender.h"
#include "ScreenRender.h"
#include "ParticleRender.h"
#include "MainRender.h"
#include "OutlinRender.h"
#include "SkyNoiseRender.h"
#include "WaveRender.h"
#include "HexRender.h"
#include "GaussianBlurRender.h"
#include "BrightFilterrender.h"
#include "ShadowRender.h"
#include "LinesRender.h"
#include "TextRender.h"
#include "PBRRender.h"

#include <algorithm>

void	eOpenGlRenderPipeline::SetSkyBoxTexture(Texture* _t)
{ renderManager->SkyBoxRender()->SetSkyBoxTexture(_t); }

void eOpenGlRenderPipeline::AddParticleSystem(IParticleSystem* _system)
{
	renderManager->AddParticleSystem(_system);
}

void eOpenGlRenderPipeline::AddParticleSystemGPU(glm::vec3 _startPos, Texture* _texture)
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
  //eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED2, width, height);
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
void eOpenGlRenderPipeline::RenderFrame(std::map<RenderType, std::vector<shObject>> _objects,
																			  Camera& _camera,
																			  const Light& _light,
																			  std::vector<std::shared_ptr<GUI>>& guis)
{
  /*std::sort(focused.begin(), focused.end(), [_camera](const shObject& obj1, const shObject& obj2)
    { return glm::length2(_camera.getPosition() - obj1->GetTransform()->getTranslation())
    > glm::length2(_camera.getPosition() - obj2->GetTransform()->getTranslation()); });*/
	
	std::vector<shObject> phong_objs = _objects.find(RenderType::PHONG)->second;
	std::vector<shObject> focused = _objects.find(RenderType::OUTLINED)->second;
	std::vector<shObject> pbr_objs = _objects.find(RenderType::PBR)->second;
	std::vector<shObject> flags = _objects.find(RenderType::FLAG)->second;
	std::vector<shObject> geometry_obj = _objects.find(RenderType::GEOMETRY)->second;
	std::vector<shObject> bezier_objs = _objects.find(RenderType::BEZIER_CURVE)->second;

	//Shadow Render Pass
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);
	auto shadow_objects = phong_objs;
	shadow_objects.insert(shadow_objects.end(), pbr_objs.begin(), pbr_objs.end());
	if (shadows) { RenderShadows(_camera, _light, shadow_objects); }

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
		RenderReflection(_camera, _light, phong_objs);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFRACTION);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RenderRefraction(_camera, _light, phong_objs);
	}
  glDisable(GL_CLIP_DISTANCE0);

		mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
			: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (sky_noise) { RenderSkyNoise(_camera); }

	glEnable(GL_STENCIL_TEST);
	//4. Rendering to main FBO with stencil
	if (focus)
	{
	  for (const auto& obj : focused)
	  {
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);

			if(std::find(phong_objs.begin(), phong_objs.end(), obj)!= phong_objs.end())
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
	//Render not outlined objects
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

	if (flags_on) { RenderFlags(_camera, _light, flags); }


	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	// Rendering WaterQuad
	if (water) { RenderWater(_camera, _light); }

	// Hexes
	if (geometry)
	{
		if (!geometry_obj.empty())
		{
			auto* mesh = dynamic_cast<const SimpleGeometryMesh*>(geometry_obj[0]->GetModel()->GetMeshes()[0]);
			RenderGeometry(_camera, *mesh);
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
		for (GLuint i = 0; i < phong_objs.size(); i++)
		{
			std::vector<glm::vec3> extrems = phong_objs[i]->GetCollider()->GetExtrems(*phong_objs[i]->GetTransform());
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
		renderManager->ScreenRender()->RenderFrame(_camera.getCameraRay().GetFrame().first, _camera.getCameraRay().GetFrame().second, 
																							static_cast<float>(width), 
																							static_cast<float>(height));
	}

	RenderGui(guis, _camera);

  static math::eClock clock;
  std::string fps;
  if (clock.isActive())
    fps = { "FPS " + std::to_string(1000 / clock.newFrame()) };
  if (!clock.isActive())
    clock.start();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderManager->TextRender()->RenderText(fps, 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f), width, height);
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

void eOpenGlRenderPipeline::RenderReflection(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager->MainRender()->SetClipPlane(-waterHeight);

	Camera tem_cam = _camera;
	_camera.setPosition(glm::vec3(tem_cam.getPosition().x, 2 * (tem_cam.getPosition().y - waterHeight), tem_cam.getPosition().z));
	_camera.setDirection(glm::reflect(_camera.getDirection(), glm::vec3(0, 1, 0)));//water normal

	renderManager->MainRender()->Render(_camera, _light, _objects, false, false, true, tone_mapping, exposure);
	_camera = tem_cam;
}

void eOpenGlRenderPipeline::RenderRefraction(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager->MainRender()->SetClipPlane(waterHeight);
	renderManager->MainRender()->Render(_camera, _light, _objects, false, false, true, tone_mapping, exposure);
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
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	
	for(auto& gui : guis)
	{
		if (gui->GetTexture() && gui->IsVisible())
		{
			renderManager->ScreenRender()->SetTexture(*(gui->GetTexture())); //copy texture
			renderManager->ScreenRender()->Render(gui->getTopLeft(), gui->getBottomRight(),
																						gui->getTopLeftTexture(), gui->getBottomRightTexture(), 
																						width, height);
		}
		gui->UpdateSync();
		for (auto& child : gui->GetChildren())
		{
			if (child->GetTexture() && child->IsVisible())
			{
				renderManager->ScreenRender()->SetTexture(*(child->GetTexture())); //copy texture
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
