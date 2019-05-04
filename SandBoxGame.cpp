#include "stdafx.h"
#include "SandBoxGame.h"

#include "ImGuiContext.h"
#include "GlBufferContext.h"

#include "SandBoxScript.h"

eSandBoxGame::eSandBoxGame(eInputController*  _input,
						   IWindowImGui*	  _guiWnd,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath,
						   const std::string& _shadersPath)
: eMainContextBase(_input, _guiWnd, _modelsPath, _assetsPath, _shadersPath)
{
	_guiWnd->Add(SLIDER_FLOAT, "Ydir", m_light.light_position.y);
	_guiWnd->Add(SLIDER_FLOAT, "Zdir", m_light.light_position.z);
	_guiWnd->Add(SLIDER_FLOAT, "Xdir", m_light.light_position.x);

	//Light init!
	m_light.ambient = vec3(0.4f, 0.4f, 0.4f);
	m_light.diffuse = vec3(0.75f, 0.75f, 0.75f);
	m_light.specular = vec3(0.5f, 0.5f, 0.5f);
	m_light.light_position = vec4(1.0f, 4.0f, 1.0f, 1.0f);
}

void eSandBoxGame::InitializeGL()
{
	texManager.InitContext(assetsFolderPath);
	texManager.LoadAllTextures();
	InitializePipline();
	InitializeBuffers();
	modelManager.InitializePrimitives();
	InitializeModels();
	InitializeRenders();
	camRay.init(width, height, nearPlane, farPlane);

	inputController->AddObserver(this, STRONG);
	inputController->AddObserver(&m_camera, WEAK);
	inputController->AddObserver(&camRay, WEAK);
}

void eSandBoxGame::PaintGL()
{
	for(auto &object : m_Objects)
	{
		if (object->getScript())
			object->getScript()->Update(m_Objects);
	}
	Pipeline();
}

bool eSandBoxGame::OnMouseMove(uint32_t x, uint32_t y)
{
	return false;
}

bool eSandBoxGame::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	case ASCII_G:
	{
		if (m_focused != nullptr)
			m_focused->getScript()->OnKeyPress(ASCII_G);
	}
	return true;
	default: return false;
	}
}

bool eSandBoxGame::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	camRay.Update(m_camera, x, y, width, height);
	if (left)
	{
		camRay.press(x, y);
		m_focused = camRay.calculateIntersaction(m_Objects);
	}
	if (m_focused && m_focused->getScript())
	{
		m_focused->getScript()->OnMousePress(x, y, left);
	}
	return true;
}

bool eSandBoxGame::OnMouseRelease()
{
	camRay.release();
	return true;
}

void eSandBoxGame::InitializePipline()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);

#define GLM_FORCE_RADIANS

	viewToProjectionMatrix = glm::perspective(glm::radians(60.0f), ((float)width) / height, nearPlane, farPlane);
	scale_bias_matrix = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f));
}

void eSandBoxGame::InitializeBuffers()
{
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW, width * 2, height * 2);
}

void eSandBoxGame::InitializeModels()
{
	eMainContextBase::InitializeModels();
	//OBJECTS
	shObject wallCube = shObject(new eObject(modelManager.Find("wall_cube").get()));
	wallCube->getTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	wallCube->setScript(new eSandBoxScript());
	m_Objects.push_back(wallCube);
	
	shObject grassPlane = shObject(new eObject(modelManager.Find("grass_plane").get(), "Graund"));
	grassPlane->getTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_Objects.push_back(grassPlane);
}

void eSandBoxGame::Pipeline()
{
	//1 Shadow Render Pass
	glViewport(0, 0, width * 2, height * 2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Bind the "depth only" FBO and set the viewport to the size of the depth texture 
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);
	// Clear
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	// Enable polygon offset to resolve depth-fighting isuses 
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(2.0f, -2000.0f);
	// Draw from the light’s point of view DrawScene(true);

	renderManager.ShadowRender()->Render(viewToProjectionMatrix, m_camera, m_light, m_Objects);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	glViewport(0, 0, width, height);

	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE1);
	//shadow
	mat4 worldToViewMatrix = glm::lookAt(glm::vec3(m_light.light_position), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 shadow_matrix = scale_bias_matrix * viewToProjectionMatrix * worldToViewMatrix;

	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, m_Objects);

	//glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	//glDisable(GL_DEPTH_TEST);
	//renderManager.ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SHADOW)); //copy texture
	//renderManager.ScreenRender()->Render(viewToProjectionMatrix, m_camera);
}
