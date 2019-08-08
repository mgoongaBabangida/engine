#include "stdafx.h"
#include "SandBoxGame.h"

#include "ImGuiContext.h"
#include "GUI.h"
#include "SandBoxScript.h"

eSandBoxGame::eSandBoxGame(eInputController*  _input,
						   IWindowImGui*	  _guiWnd,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath,
						   const std::string& _shadersPath)
: eMainContextBase(_input, _guiWnd, _modelsPath, _assetsPath, _shadersPath)
, pipeline(m_Objects, width, height, nearPlane, farPlane, 0)
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
	pipeline.RanderFrame(m_camera, m_light, std::vector<GUI> {}, std::vector<shObject> {});
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
	pipeline.Initialize();
	// call all the enable pipeline functions
}

void eSandBoxGame::InitializeBuffers()
{
	pipeline.InitializeBuffers(); //todo add possibility to choose buffers
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