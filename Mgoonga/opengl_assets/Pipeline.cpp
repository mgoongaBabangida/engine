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

void	ePipeline::SetSkyBoxTexture(Texture* _t)  { renderManager->SkyBoxRender()->SetSkyBoxTexture(_t); }
void	ePipeline::AddHex(glm::vec3 _v) { renderManager->AddHex(_v); }
void	ePipeline::SetHexRadius(float _r) { renderManager->SetHexRadius(_r); }

//-------------------------------------------------------------------------------------------
ePipeline::ePipeline(std::vector<shObject>& _objs, 
					 uint32_t				_width, 
					 uint32_t				_height, 
					 float					_nearPlane, 
					 float					_farPlane, 
					 float					_waterHeight)
: m_objects(_objs),
  width(_width),
  height(_height),
  nearPlane(_nearPlane),
  farPlane(_farPlane),
  waterHeight(_waterHeight),
  renderManager(new eRenderManager)
{}

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
}

void ePipeline::InitializeRenders(eModelManager& modelManager, eTextureManager& texManager, const std::string& shadersFolderPath)
{
	renderManager->Initialize(modelManager, texManager, shadersFolderPath);
}

Texture ePipeline::GetSkyNoiseTexture(const Camera& _camera)
{
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SQUERE);
	RanderSkyNoise(_camera); //do we need camera?
	return eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SQUERE);
}

//-----------------------------------------------------------------------------------------------
void ePipeline::RanderFrame(Camera& _camera, const Light& _light ,std::vector<GUI>& guis, std::vector<shObject> m_focused, std::vector<Flag>& _flags)
{
	//1 Shadow Render Pass
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);

	if (shadows) { RanderShadows(_camera, _light, m_objects); }

	if (_light.type == eLightType::DIRECTION)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE1);
	else if (_light.type == eLightType::POINT)
		eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE0);
	else
		assert("spot light is not yet supported");

	//3 Rendering reflaction and refraction to Textures
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFLECTION);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	/*if (skybox)*/ { RanderSkybox(_camera); }

	glEnable(GL_CLIP_DISTANCE0); //?
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	//if (sky) { RanderSkyNoise(m_camera); }

	if (water)
	{
		RanderReflection(_camera, _light, m_objects);

		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFRACTION);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		RanderRefraction(_camera, _light, m_objects);
	}
  glDisable(GL_CLIP_DISTANCE0);

		mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
			: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//if (sky) { RanderSkyNoise(m_camera); }

	//4. Rendering to main FBO with stencil
	if (focuse) { RanderFocused(_camera, _light, m_focused); }

	RanderMain(_camera, _light, m_objects);

	if (focuse) { RanderOutlineFocused(_camera, _light, m_focused); }

	if (flags) { RanderFlags(_camera, _light, _flags); }

	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	// Rendering WaterQuad
	if (water) { RanderWater(_camera, _light); }

	// Hexes
	if (geometry) { RanderGeometry(_camera); }	//$need to transfer dots every frame

	//  Draw skybox firs
	if (skybox)
	{
		RanderSkybox(_camera);
	}
	glDepthFunc(GL_LEQUAL); //?

	//7  Particles
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	if (particles) { RanderParticles(_camera); }

	if (mts) { RanderBlur(_camera); }
	
	if (draw_bounding_boxes)
	{
		for (auto object : m_objects.get())
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

	if(mousepress)
	{
		renderManager->ScreenRender()->RenderFrame();
	}

	RanderGui(guis, _camera);
}

void ePipeline::RanderShadows(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
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

void ePipeline::RanderSkybox(const Camera& _camera)
{
	glDepthFunc(GL_LEQUAL);
	renderManager->SkyBoxRender()->Render(_camera);
  glDepthFunc(GL_LESS);
}

void ePipeline::RanderReflection(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager->MainRender()->SetClipPlane(-waterHeight);

	Camera tem_cam = _camera;
	_camera.setPosition(glm::vec3(tem_cam.getPosition().x, 2 * (tem_cam.getPosition().y - waterHeight), tem_cam.getPosition().z));
	_camera.setDirection(glm::reflect(_camera.getDirection(), glm::vec3(0, 1, 0)));  //water normal

	renderManager->MainRender()->Render(_camera, _light, m_objects);
	_camera = tem_cam;
}

void ePipeline::RanderRefraction(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager->MainRender()->SetClipPlane(waterHeight);
	renderManager->MainRender()->Render(_camera, _light, m_objects);
	renderManager->MainRender()->SetClipPlane(10);

	//glDisable(GL_CLIP_DISTANCE0);
}

void ePipeline::RanderSkyNoise(const Camera& _camera)
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

void ePipeline::RanderFocused(const Camera& _camera, const Light& _light, std::vector<shObject>& _focused)
{
	//4. Rendering to main FBO with stencil
	StencilFuncDefault();
	if(!_focused.empty())
	{
		renderManager->MainRender()->Render(_camera, _light, _focused);
	}
}

void ePipeline::RanderMain(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	glDisable(GL_STENCIL_TEST);
	renderManager->MainRender()->Render(_camera, _light, _objects);
	glEnable(GL_STENCIL_TEST);
}

void ePipeline::RanderOutlineFocused(const Camera& _camera, const Light& _light, std::vector<shObject>& _focused)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//5. Rendering Stencil Outlineing
	if(!_focused.empty())
	{
		renderManager->OutlineRender()->Render(_camera, _light, std::vector<shObject> {_focused });
	}
	StencilFuncDefault();
	glClear(GL_STENCIL_BUFFER_BIT);
}

void ePipeline::RanderFlags(const Camera& _camera, const Light& _light, std::vector<Flag>& _flags)
{
	renderManager->WaveRender()->Render(_camera, _light, _flags);
}

void ePipeline::RanderWater(const Camera& _camera, const Light& _light)
{
	glDisable(GL_CULL_FACE);
	renderManager->WaterRender()->Render(_camera, _light);
	glEnable(GL_CULL_FACE);
}

void ePipeline::RanderGeometry(const Camera& _camera)
{
	glm::mat4 MVP = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
	renderManager->HexRender()->Render(MVP);
}

void ePipeline::RanderParticles(const Camera& _camera)
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

void ePipeline::RanderBlur(const Camera& _camera)
{
	eGlBufferContext::GetInstance().ResolveMtsToScreen();
	renderManager->BrightFilterRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_BRIGHT_FILTER);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	renderManager->BrightFilterRender()->Render();
	renderManager->GaussianBlurRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER));
	renderManager->GaussianBlurRender()->Render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //default?
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	renderManager->ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	renderManager->ScreenRender()->SetTextureContrast(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO));
	renderManager->ScreenRender()->Render(_camera);
}

void ePipeline::RanderGui(std::vector<GUI>& guis, const Camera& _camera)
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
}