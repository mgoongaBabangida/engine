#include "stdafx.h"
#include "Pipeline.h"

#include <math/Camera.h>
#include "GUI.h"

#include "GlBufferContext.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "ModelManager.h"

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

void	ePipeline::SetSkyBoxTexture(Texture* _t)  { renderManager->SkyBoxRender()->SetSkyBoxTexture(_t); }
void	ePipeline::AddHex(glm::vec3 _v) { renderManager->AddHex(_v); }
void	ePipeline::SetHexRadius(float _r) { renderManager->SetHexRadius(_r); }

//-------------------------------------------------------------------------------------------
ePipeline::ePipeline(dbb::CameraRay& _cam_ray,
                     uint32_t				_width,
                     uint32_t				_height)
: camRay(_cam_ray),
  width(_width),
  height(_height),
  renderManager(new eRenderManager)
{
	material.roughness = 0.6f;
	material.metallic = 0.3f;
}

ePipeline::~ePipeline()
{
}

void ePipeline::Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
}


void ePipeline::InitializeBuffers(bool _needsShadowCubeMap)
{
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_MTS, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFLECTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFRACTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW, width * 2, height * 2, _needsShadowCubeMap);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SQUERE, height, height); //squere
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_BRIGHT_FILTER, width, height);
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_GAUSSIAN_ONE, 600, 300);
  eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_GAUSSIAN_TWO, 600, 300);
  //eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED2, width, height);
}

//-----------------------------------------------------------------------------------------------
void ePipeline::InitializeRenders(eModelManager& modelManager, eTextureManager& texManager, const std::string& shadersFolderPath)
{
	renderManager->Initialize(modelManager, texManager, shadersFolderPath);
}

//-----------------------------------------------------------------------------------------------
Texture ePipeline::GetSkyNoiseTexture(const Camera& _camera)
{
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SQUERE);
	RenderSkyNoise(_camera); //do we need camera?
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SQUERE);
}

//-----------------------------------------------------------------------------------------------
void ePipeline::RenderFrame(std::map<RenderType, std::vector<shObject>> _objects,
	Camera& _camera,
	const Light& _light,
	std::vector<GUI>& guis)
{
  /*std::sort(focused.begin(), focused.end(), [_camera](const shObject& obj1, const shObject& obj2)
    { return glm::length2(_camera.getPosition() - obj1->GetTransform()->getTranslation())
    > glm::length2(_camera.getPosition() - obj2->GetTransform()->getTranslation()); });*/
	  
	std::vector<shObject> objects = _objects.find(RenderType::MAIN)->second;
	std::vector<shObject> focused = _objects.find(RenderType::OUTLINED)->second;
	std::vector<shObject> pbr_objs = _objects.find(RenderType::PBR)->second;
	std::vector<shObject> flags = _objects.find(RenderType::FLAG)->second;

	//1 Shadow Render Pass
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);

	if (shadows) { RenderShadows(_camera, _light, objects); }

	if (_light.type == eLightType::DIRECTION)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE1);
	else if (_light.type == eLightType::POINT)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE0);
	else
		assert("spot light is not yet supported");

	//3 Rendering reflection and refraction to Textures
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFLECTION);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/*if (skybox)*/ { RenderSkybox(_camera); }

	glEnable(GL_CLIP_DISTANCE0); //?
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	if (water)
	{
		RenderReflection(_camera, _light, objects);
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFRACTION);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RenderRefraction(_camera, _light, objects);
	}
  glDisable(GL_CLIP_DISTANCE0);

		mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
			: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (sky) { RenderSkyNoise(_camera); }

	//4. Rendering to main FBO with stencil
	if (focuse)
	{
	  for (auto obj : focused)
	  {
	    RenderFocused(_camera, _light, std::vector<shObject>{ obj });
	    RenderOutlineFocused(_camera, _light, std::vector<shObject>{ obj });
	  }
	}

	std::vector<shObject> not_outlined;
	std::set_difference(objects.begin(), objects.end(),
		                  focused.begin(), focused.end(),
		                  std::back_inserter(not_outlined),
		                  [](auto& a, auto& b) { return &a < &b; });

	RenderMain(_camera, _light, not_outlined);

	RenderPBR(_camera, _light, pbr_objs);

	if (flags_on) { RenderFlags(_camera, _light, flags); }

	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	// Rendering WaterQuad
	if (water) { RenderWater(_camera, _light); }

	// Hexes
	if (geometry) { RenderGeometry(_camera); }	//$need to transfer dots every frame

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

	if (mts) { RenderBlur(_camera); }
	
	if (draw_bounding_boxes)
	{
		for (auto object : objects)
		{
			std::vector<glm::vec3> extrems = object->GetCollider()->GetExtrems(*object->GetTransform());
			std::vector<GLuint> indices{ 0,1,1,2,2,3,3,0,4,5,5,6,6,7,7,4,0,4,1,5,2,6,3,7 };
			renderManager->LinesRender()->Render(_camera, extrems, indices);
		}
	}

	//8.1 Texture visualization
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //default?
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	if(mousepress  && camRay.get().IsPressed())
	{
		renderManager->ScreenRender()->RenderFrame(camRay.get().GetFrame().first, camRay.get().GetFrame().second, width, height);
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
}

//-------------------------------------------------------
Texture ePipeline::GetReflectionBufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION);
}

//-------------------------------------------------------
Texture ePipeline::GetRefractionBufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION);
}

//-------------------------------------------------------
Texture ePipeline::GetShadowBufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SHADOW);
}

//-------------------------------------------------------
Texture ePipeline::GetGausian1BufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_ONE);
}

//-------------------------------------------------------
Texture ePipeline::GetGausian2BufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO);
}

//-------------------------------------------------------
Texture ePipeline::GetMtsBufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_MTS);
}

//-------------------------------------------------------
Texture ePipeline::GetScreenBufferTexture() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN);
}

//----------------------------------------------------
Texture ePipeline::GetBrightFilter() const
{
  return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER);
}

void ePipeline::RenderShadows(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	// Bind the "depth only" FBO and set the viewport to the size of the depth texture 
	glViewport(0, 0, width * 2, height * 2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Clear
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	// Enable polygon offset to resolve depth-fighting isuses 
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(2.0f, -2000.0f);
	// Draw from the light’s point of view DrawScene(true);

	renderManager->ShadowRender()->Render(_camera, _light, _objects);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	glViewport(0, 0, width, height);
}

void ePipeline::RenderSkybox(const Camera& _camera)
{
	glDepthFunc(GL_LEQUAL);
	renderManager->SkyBoxRender()->Render(_camera);
  glDepthFunc(GL_LESS);
}

void ePipeline::RenderReflection(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager->MainRender()->SetClipPlane(-waterHeight);

	Camera tem_cam = _camera;
	_camera.setPosition(glm::vec3(tem_cam.getPosition().x, 2 * (tem_cam.getPosition().y - waterHeight), tem_cam.getPosition().z));
	_camera.setDirection(glm::reflect(_camera.getDirection(), glm::vec3(0, 1, 0)));  //water normal

	renderManager->MainRender()->Render(_camera, _light, _objects, false, false);
	_camera = tem_cam;
}

void ePipeline::RenderRefraction(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager->MainRender()->SetClipPlane(waterHeight);
	renderManager->MainRender()->Render(_camera, _light, _objects, false, false);
	renderManager->MainRender()->SetClipPlane(10);

	//glDisable(GL_CLIP_DISTANCE0);
}

void ePipeline::RenderSkyNoise(const Camera& _camera)
{
	//sky noise
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	renderManager->SkyNoiseRender()->Render(_camera);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void ePipeline::StencilFuncDefault()
{
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
}

void ePipeline::RenderFocused(const Camera& _camera, const Light& _light, std::vector<shObject>& focused)
{
	//4. Rendering to main FBO with stencil
	StencilFuncDefault();
	if(!focused.empty())
	{
		renderManager->MainRender()->Render(_camera, _light, focused, false, false);
	}
}

void ePipeline::RenderMain(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	glDisable(GL_STENCIL_TEST);
	renderManager->MainRender()->Render(_camera, _light, _objects, debug_white, debug_texcoords);
	glEnable(GL_STENCIL_TEST);
}

void ePipeline::RenderOutlineFocused(const Camera& _camera, const Light& _light, std::vector<shObject>& focused)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//5. Rendering Stencil Outlineing
	if(!focused.empty())
	{
		renderManager->OutlineRender()->Render(_camera, _light, std::vector<shObject> {focused });
	}
	StencilFuncDefault();
	glClear(GL_STENCIL_BUFFER_BIT);
}

void ePipeline::RenderFlags(const Camera& _camera, const Light& _light, std::vector<shObject> flags)
{
	renderManager->WaveRender()->Render(_camera, _light, flags);
}

void ePipeline::RenderWater(const Camera& _camera, const Light& _light)
{
	glDisable(GL_CULL_FACE);
	renderManager->WaterRender()->Render(_camera, _light);
	glEnable(GL_CULL_FACE);
}

void ePipeline::RenderGeometry(const Camera& _camera)
{
	glm::mat4 MVP = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
	renderManager->HexRender()->Render(MVP);
}

void ePipeline::RenderParticles(const Camera& _camera)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glCullFace(GL_TRUE);
	renderManager->ParticleRender()->Render(_camera);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ePipeline::RenderBlur(const Camera& _camera)
{
	eGlBufferContext::GetInstance().ResolveMtsToScreen();
	renderManager->BrightFilterRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_BRIGHT_FILTER);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	renderManager->BrightFilterRender()->Render();
	renderManager->GaussianBlurRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER));
	renderManager->GaussianBlurRender()->Render();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); //default?
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	renderManager->ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	renderManager->ScreenRender()->SetTextureContrast(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO));
	renderManager->ScreenRender()->RenderContrast(_camera, blur_coef);
}

void ePipeline::RenderGui(std::vector<GUI>& guis, const Camera& _camera)
{
	if (!guis.empty())
	{
		guis[0].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION));
		//8.2 Second quad
		guis[1].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SHADOW));
	}

	for(auto& gui : guis)
	{
		if (gui.GetTexture())
		{
			glViewport(gui.getViewPort().x, gui.getViewPort().y, gui.getViewPort().z, gui.getViewPort().w);
			renderManager->ScreenRender()->SetTexture(*(gui.GetTexture())); //copy texture
			renderManager->ScreenRender()->Render(_camera);
		}
	}
	glViewport(0, 0, width, height);
}

#include "ShpereTexturedModel.h"

//------------------------------------------------
void ePipeline::RenderPBR(const Camera& _camera, const Light& _light, std::vector<shObject> _objs)
{
	if (!_objs.empty())
	{
		material.ao = 0.8f;
		material.diffuse = glm::vec3(0.5f, 0.0f, 0.0f);
		material.shininess = 0.5f;
		if (auto* mesh = _objs[0]->GetModel()->GetMeshes()[0]; mesh && mesh->HasMaterial())
			const_cast<SphereTexturedMesh*>(dynamic_cast<const SphereTexturedMesh*>(mesh))->SetMaterial(material);

		GetRenderManager().PBRRender()->Render(_camera, _light, _objs);
	}
}
