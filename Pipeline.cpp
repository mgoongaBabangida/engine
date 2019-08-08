#include "stdafx.h"
#include "Pipeline.h"
#include "Camera.h"
#include "GUI.h"
#include "GlBufferContext.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "ShipScript.h"	//$

ePipeline::ePipeline(std::vector<shObject>& _objs, 
					 uint32_t				_width, 
					 uint32_t				_height, 
					 float					_nearPlane, 
					 float					_farPlane, 
					 float					_waterHeight)
: m_Objects(_objs),
  width(_width),
  height(_height),
  nearPlane(_nearPlane),
  farPlane(_farPlane),
  waterHeight(_waterHeight)
{}

void ePipeline::Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

#define GLM_FORCE_RADIANS

	viewToProjectionMatrix = glm::perspective(glm::radians(60.0f), ((float)width) / height, nearPlane, farPlane);
	scale_bias_matrix = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
							 vec4(0.0f, 0.5f, 0.0f, 0.0f),
							 vec4(0.0f, 0.0f, 0.5f, 0.0f),
							 vec4(0.5f, 0.5f, 0.5f, 1.0f));
}


void ePipeline::InitializeBuffers()
{
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_MTS, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFLECTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFRACTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW, width * 2, height * 2);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED, width, height);
}

void ePipeline::InitializeRenders(eModelManager& modelManager, eTextureManager& texManager, const std::string& shadersFolderPath)
{
	renderManager.Initialize(modelManager, texManager, shadersFolderPath);
}

void ePipeline::RanderFrame(Camera& m_camera, const Light& m_light ,std::vector<GUI>& guis, std::vector<shObject> m_focused)
{
	//1 Shadow Render Pass
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);

	if (shadows) { RanderShadows(m_camera, m_light, m_Objects); }

	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE1);

	//shadow
	mat4 worldToViewMatrix = glm::lookAt(glm::vec3(m_light.light_position), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	shadow_matrix = scale_bias_matrix * viewToProjectionMatrix * worldToViewMatrix;

	//3 Rendering reflaction and refraction to Textures
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFLECTION);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	if (skybox) { RanderSkybox(m_camera); }

	glEnable(GL_CLIP_DISTANCE0); //?
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	if (sky) { RanderSkyNoise(m_camera); }

	if (water)
	{
		RanderReflection(m_camera, m_light, m_Objects);

		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFRACTION);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

		RanderRefraction(m_camera, m_light, m_Objects);

		mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
			: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	}

	if (sky) { RanderSkyNoise(m_camera); }

	//4. Rendering to main FBO with stencil
	if (focuse) { RanderFocused(m_camera, m_light, m_focused); }

	RanderMain(m_camera, m_light, m_Objects);

	if (focuse) { RanderOutlineFocused(m_camera, m_light, m_focused); }

	if (flags) { RanderFlags(m_camera, m_light); }

	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	//6. Rendering WaterQuad
	if (water) { RanderWater(m_camera, m_light); }

	//Hexes
	if (geometry) { RanderGeometry(m_camera); }	//$need to transfer dots every frame

	//2  Draw skybox firs
	if (skybox)
	{
		RanderSkybox(m_camera);
	}
	glDepthFunc(GL_LEQUAL); //?

	//7  Particles
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	if (particles) { RanderParticles(m_camera); }

	if (mts) { RanderBlur(m_camera); }

	//8.1 Texture visualization
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //default?
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	if(mousepress)
	{
		renderManager.ScreenRender()->RenderFrame();
	}

	RanderGui(guis, m_camera);
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

	renderManager.ShadowRender()->Render(viewToProjectionMatrix, _camera, _light, _objects);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	glViewport(0, 0, width, height);
}

void ePipeline::RanderSkybox(const Camera& _camera)
{
	glDepthFunc(GL_LEQUAL);
	renderManager.SkyBoxRender()->Render(viewToProjectionMatrix, _camera);
}

void ePipeline::RanderReflection(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager.MainRender()->SetClipPlane(-waterHeight);

	Camera tem_cam = _camera;
	_camera.setPosition(glm::vec3(tem_cam.getPosition().x, 2 * (tem_cam.getPosition().y - waterHeight), tem_cam.getPosition().z));
	_camera.setDirection(glm::reflect(_camera.getDirection(), vec3(0, 1, 0)));  //water normal

	renderManager.MainRender()->Render(viewToProjectionMatrix, _camera, _light, shadow_matrix, m_Objects);
	_camera = tem_cam;
}

void ePipeline::RanderRefraction(Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	renderManager.MainRender()->SetClipPlane(waterHeight);
	renderManager.MainRender()->Render(viewToProjectionMatrix, _camera, _light, shadow_matrix, m_Objects);
	renderManager.MainRender()->SetClipPlane(10);

	//glDisable(GL_CLIP_DISTANCE0);
}

void ePipeline::RanderSkyNoise(const Camera& _camera)
{
	//sky noise
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	renderManager.SkyNoiseRender()->Render(viewToProjectionMatrix, _camera);
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
		renderManager.MainRender()->Render(viewToProjectionMatrix, _camera, _light, shadow_matrix, _focused);
	}
}

void ePipeline::RanderMain(const Camera& _camera, const Light& _light, std::vector<shObject>& _objects)
{
	glDisable(GL_STENCIL_TEST);
	renderManager.MainRender()->Render(viewToProjectionMatrix, _camera, _light, shadow_matrix, _objects);
	glEnable(GL_STENCIL_TEST);
}

void ePipeline::RanderOutlineFocused(const Camera& _camera, const Light& _light, std::vector<shObject>& _focused)
{
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	//5. Rendering Stencil Outlineing
	if(!_focused.empty())
	{
		renderManager.OutlineRender()->Render(viewToProjectionMatrix, _camera, _light, shadow_matrix, std::vector<shObject> {_focused });
	}
	StencilFuncDefault();
	glClear(GL_STENCIL_BUFFER_BIT);
}

void ePipeline::RanderFlags(const Camera& _camera, const Light& _light)
{
	std::vector<Flag> flags;
	// Render Flags
	for (auto object : m_Objects.get())
	{
		eShipScript* script = dynamic_cast<eShipScript*>(object->getScript());
		if (script)
			flags.push_back(script->GetFlag(_camera));
	}
	renderManager.WaveRender()->Render(viewToProjectionMatrix, _camera, _light, shadow_matrix, flags);
}

void ePipeline::RanderWater(const Camera& _camera, const Light& _light)
{
	glDisable(GL_CULL_FACE);
	renderManager.WaterRender()->Render(viewToProjectionMatrix, _camera, _light);
	glEnable(GL_CULL_FACE);
}

void ePipeline::RanderGeometry(const Camera& _camera)
{
	mat4 MVP = viewToProjectionMatrix * _camera.getWorldToViewMatrix();
	renderManager.HexRender()->Render(MVP);
}

void ePipeline::RanderParticles(const Camera& _camera)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glCullFace(GL_TRUE);
	renderManager.ParticleRender()->Render(viewToProjectionMatrix, _camera);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
}

void ePipeline::RanderBlur(const Camera& _camera)
{
	eGlBufferContext::GetInstance().ResolveMtsToScreen();
	renderManager.BrightFilterRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_BRIGHT_FILTER);
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	renderManager.BrightFilterRender()->Render();
	renderManager.GaussianBlurRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER));
	renderManager.GaussianBlurRender()->Render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //default?
	glViewport(0, 0, width, height);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	renderManager.ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
	renderManager.ScreenRender()->SetTextureContrast(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO));
	renderManager.ScreenRender()->Render(viewToProjectionMatrix, _camera);
}

void ePipeline::RanderGui(std::vector<GUI>& guis, const Camera& _camera)
{
	//$optimaze for each, add other possibilities
	if(!guis.empty())
	{
		guis[0].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION));
		//8.2 Second quad
		guis[1].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION));
	}

	for(auto gui : guis)
	{
		glViewport(gui.getViewPort().x, gui.getViewPort().y, gui.getViewPort().z, gui.getViewPort().w);
		renderManager.ScreenRender()->SetTexture(*(gui.GetTexture())); //copy texture
		renderManager.ScreenRender()->Render(viewToProjectionMatrix, _camera);
	}
}