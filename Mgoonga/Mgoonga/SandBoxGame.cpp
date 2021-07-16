#include "stdafx.h"
#include "SandBoxGame.h"

#include <base/InputController.h>

#include <math/Rigger.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>

#include <opengl_assets/GUI.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <sdl_assets/ImGuiContext.h>

#include "SandBoxScript.h"

//-------------------------------------------------------------------------
eSandBoxGame::eSandBoxGame(eInputController*  _input,
  std::vector<IWindowImGui*> _externalGui,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath,
						   const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
, pipeline(m_objects, width, height, nearPlane, farPlane, 0)
, m_camera(width, height, nearPlane, farPlane)
{
	_externalGui[0]->Add(SLIDER_FLOAT, "Ydir", &m_light.light_position.y);
	_externalGui[0]->Add(SLIDER_FLOAT, "Zdir", &m_light.light_position.z);
	_externalGui[0]->Add(SLIDER_FLOAT, "Xdir", &m_light.light_position.x);

	//Light init!
	m_light.ambient = vec3(0.4f, 0.4f, 0.4f);
	m_light.diffuse = vec3(0.75f, 0.75f, 0.75f);
	m_light.specular = vec3(0.5f, 0.5f, 0.5f);
	m_light.light_position = vec4(1.0f, 4.0f, 1.0f, 1.0f);
}

void eSandBoxGame::InitializeGL()
{
	eMainContextBase::InitializeGL();

	camRay.init(static_cast<float>(width), static_cast<float>(height), nearPlane, farPlane);

	inputController->AddObserver(this, STRONG);
	inputController->AddObserver(&m_camera, WEAK);
	inputController->AddObserver(&camRay, WEAK);
}

void eSandBoxGame::PaintGL()
{
	eMainContextBase::PaintGL();

	for(auto &object : m_objects)
	{
		if (object->GetScript())
			object->GetScript()->Update(m_objects);
	}
	pipeline.RanderFrame(m_camera, m_light, std::vector<GUI> {}, std::vector<shObject> {}, std::vector<Flag>{});
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
			m_focused->GetScript()->OnKeyPress(ASCII_G);
	}
	return true;
	default: return false;
	}
}

bool eSandBoxGame::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	camRay.Update(m_camera, x, y, static_cast<float>(width), static_cast<float>(height));
	if (left)
	{
		camRay.press(x, y);
		m_focused = camRay.calculateIntersaction(m_objects);
	}
	if (m_focused && m_focused->GetScript())
	{
		m_focused->GetScript()->OnMousePress(x, y, left);
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
	pipeline.SwitchSkyBox(false);
	pipeline.SwitchWater(false);
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
	shObject wallCube = shObject(new eObject);
	wallCube->SetModel(modelManager->Find("wall_cube").get());
	wallCube->SetTransform(new Transform);
	wallCube->SetCollider(new BoxCollider);
	wallCube->SetRigidBody(new eRigidBody);
	wallCube->GetRigidBody()->SetObject(wallCube.get());
	wallCube->GetCollider()->CalculateExtremDots(wallCube->GetModel()->GetPositions());
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	wallCube->SetScript(new eSandBoxScript());
	m_objects.push_back(wallCube);
	
	shObject grassPlane = shObject(new eObject);
	grassPlane->SetModel(modelManager->Find("grass_plane").get());
	grassPlane->SetTransform(new Transform);
	grassPlane->SetCollider(new BoxCollider);
	grassPlane->SetRigidBody(new eRigidBody);
	grassPlane->GetRigidBody()->SetObject(grassPlane.get());
	grassPlane->GetCollider()->CalculateExtremDots(grassPlane->GetModel()->GetPositions());
	grassPlane->SetName("Graund");
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_objects.push_back(grassPlane);

	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());

	shObject wolf = shObject(new eObject);
	wolf->SetModel(modelManager->Find("wolf").get());
	wolf->SetTransform(new Transform);
	wolf->SetCollider(new BoxCollider);
	wolf->SetRigidBody(new eRigidBody);
	wolf->GetRigidBody()->SetObject(wolf.get());
	wolf->GetCollider()->CalculateExtremDots(wolf->GetModel()->GetPositions());
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setScale(vec3(1.5f, 1.5f, 1.5f));
	wolf->GetTransform()->setTranslation(vec3(3.0f, -2.0f, 0.0f));
	wolf->SetRigger(new Rigger((Model*)modelManager->Find("wolf").get()));
	wolf->GetRigger()->ChangeName(std::string(), "Running");
	m_objects.push_back(wolf);
}

void eSandBoxGame::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
}