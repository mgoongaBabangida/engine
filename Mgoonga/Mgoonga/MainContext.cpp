#include "stdafx.h"
#include "MainContext.h"

#include <base/InputController.h>

#include <math/ParticleSystem.h>
#include <math/ShootingParticleSystem.h>
#include <math/Rigger.h>
#include <math/RigidBdy.h>
#include <math/BoxCollider.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/Texture.h>
#include <opengl_assets/RenderManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <sdl_assets/ImGuiContext.h>

#include "ShipScript.h"

//---------------------------------------------------------------------------
eMainContext::eMainContext(eInputController*  _input,
						   IWindowImGui*	  _guiWnd,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath, 
						   const std::string& _shadersPath)
: eMainContextBase(_input, _guiWnd, _modelsPath, _assetsPath, _shadersPath)
, pipeline(m_objects, width, height, nearPlane, farPlane, waterHeight)
, m_camera(width, height, nearPlane, farPlane)
{
	_guiWnd->Add(SLIDER_FLOAT, "Ydir", &m_light.light_position.y);
	_guiWnd->Add(SLIDER_FLOAT, "Zdir", &m_light.light_position.z);
	_guiWnd->Add(SLIDER_FLOAT, "Xdir", &m_light.light_position.x);

	//Light init!
	m_light.ambient			= vec3(0.05f, 0.05f, 0.05f);
	m_light.diffuse			= vec3(0.75f, 0.75f, 0.75f);
	m_light.specular		= vec3(0.5f, 0.5f, 0.5f);
	m_light.light_position	= vec4(1.0f, 4.0f, 1.0f, 1.0f);
	m_light.type			= eLightType::DIRECTION;
}

//--------------------------------------------------------------------------
bool eMainContext::OnMouseMove(uint32_t x, uint32_t y)
{
	m_framed = camRay.onMove(m_camera, m_objects, static_cast<float>(x), static_cast<float>(y)); 	//to draw a frame
	return true;
}

//--------------------------------------------------------------------------
bool eMainContext::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	case ASCII_J:	{ if (m_focused)	m_focused->GetRigidBody()->MoveLeft(m_objects);}				return true;
	case ASCII_L:	{ if (m_focused)	m_focused->GetRigidBody()->MoveRight(m_objects);}				return true;
	case ASCII_K:	{ if (m_focused)	m_focused->GetRigidBody()->MoveBack(m_objects);}				return true;
	case ASCII_I:	{ if (m_focused)	m_focused->GetRigidBody()->MoveForward(m_objects);}				return true;
	case ASCII_Z:	{ if (m_focused)	m_focused->GetRigidBody()->MoveUp(m_objects);}					return true;
	case ASCII_X:	{ if (m_focused)	m_focused->GetRigidBody()->MoveDown(m_objects);}				return true;
	case ASCII_C:	{ if (m_focused)	m_focused->GetRigidBody()->TurnRight(m_objects);}				return true;
	case ASCII_V:	{ if (m_focused)	m_focused->GetRigidBody()->TurnLeft(m_objects);}				return true;
	case ASCII_B:	{ if (m_focused)	m_focused->GetRigidBody()->LeanRight(m_objects);}				return true;
	case ASCII_N:	{ if (m_focused)	m_focused->GetRigidBody()->LeanLeft(m_objects);}				return true;
	case ASCII_U:	{ if (m_focused)	m_focused->GetRigidBody()->LeanForward(m_objects);}				return true;
	case ASCII_H:	{ if (m_focused)	m_focused->GetRigidBody()->LeanBack(m_objects);}				return true;
	case 27: {} return false; //ESC @todo
	//case ASCII_G:	{ if (m_focused)	m_focused->GetScript()->OnKeyPress(ASCII_G);}	return true;
	default: return false;
	}
}

//--------------------------------------------------------------------------
bool eMainContext::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	camRay.Update(m_camera, x, y, width, height);
	if(left)
	{
		camRay.press(x, y);
		m_focused = camRay.calculateIntersaction(m_objects);
	}
	if(m_focused && m_focused->GetScript())
	{
		m_focused->GetScript()->OnMousePress(x, y, left);
	}
	return true;
}

//---------------------------------------------------------------------------------
bool eMainContext::OnMouseRelease()
{
	camRay.release();
	return true;
}

//-------------------------------------------------------------------------------
void eMainContext::InitializeGL()
{
	eMainContextBase::InitializeGL();
	
	//Camera Ray
	camRay.init(width, height, nearPlane, farPlane);
	
	m_objects[4]->SetScript(new eShipScript(texManager->Find("TSpanishFlag0_s"),
											pipeline.GetRenderManager(),
											texManager->Find("Tatlas2"),
											soundManager->GetSound("shot_sound"),
											&camRay,
											waterHeight));
	m_focused = m_objects[4];

	guis.emplace_back(GUI(0, 0, width / 4, height / 4, width, height));
	guis[0].setCommand(std::make_shared<AnimStart>(AnimStart(m_objects[6])));
	guis.emplace_back(GUI(width / 4 * 3, height / 4 * 3, width / 4, height / 4, width, height));
	guis[1].setCommand(std::make_shared<AnimStop>(AnimStop(m_objects[6])));

	inputController->AddObserver(this, STRONG);
  inputController->AddObserver(&guis[0], STRONG);//monopoly takes keyboard as well which is wrong
  inputController->AddObserver(&guis[1], STRONG);
	inputController->AddObserver(&m_camera, WEAK);
	inputController->AddObserver(&camRay, WEAK);
}

//-------------------------------------------------------------------------------
void eMainContext::InitializeSounds()
{
	//sound->loadListner(m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);
}

//-------------------------------------------------------------------------------
void eMainContext::InitializePipline()
{
	pipeline.Initialize();
	// call all the enable pipeline functions
}

//-----------------------------------------------------------------------------
void eMainContext::InitializeBuffers()
{
	pipeline.InitializeBuffers(m_light.type == eLightType::POINT);
}

//-----------------------------------------------------------------------------
void eMainContext::InitializeModels()
{
	eMainContextBase::InitializeModels();

	//MODELS
	modelManager->Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());
	modelManager->Add("boat", (GLchar*)std::string(modelFolderPath + "Medieval Boat/Medieval Boat.obj").c_str());
	//modelManager.Add("spider", (GLchar*)std::string(ModelFolderPath + "ogldev-master/Content/spider.obj").c_str());
	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager->Add("guard", (GLchar*)std::string(modelFolderPath + "ogldev-master/Content/guard/boblampclean.md5mesh").c_str());
	//modelManager.Add("stairs", (GLchar*)std::string(modelFolderPath + "stairs.blend").c_str());

	//TERRAIN
	m_TerrainModel.swap(modelManager->CloneTerrain("simple"));
	m_TerrainModel->initialize(texManager->Find("Tgrass0_d"),
							   texManager->Find("Tgrass0_d"),
							   texManager->Find("Tblue"),
							   texManager->Find("TOcean0_s"));
	//OBJECTS
	shObject wallCube = shObject(new eObject);
	wallCube->SetModel(modelManager->Find("wall_cube").get());
	wallCube->SetTransform(new Transform);
	wallCube->SetCollider(new BoxCollider);
	wallCube->SetRigidBody(new eRigidBody);
	wallCube->GetRigidBody()->SetObject(wallCube.get());
	wallCube->GetCollider()->CalculateExtremDots(wallCube->GetModel()->GetPositions());
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	m_objects.push_back(wallCube);

	shObject containerCube = shObject(new eObject);
	containerCube->SetModel(modelManager->Find("container_cube").get());
	containerCube->SetTransform(new Transform);
	containerCube->SetCollider(new BoxCollider);
	containerCube->SetRigidBody(new eRigidBody);
	containerCube->GetRigidBody()->SetObject(containerCube.get());
	containerCube->GetCollider()->CalculateExtremDots(containerCube->GetModel()->GetPositions());
	containerCube->GetTransform()->setTranslation(vec3(0.5f, 3.0f, -3.5f));
	containerCube->GetTransform()->setScale(vec3(0.2f, 0.2f, 0.2f));
	m_objects.push_back(containerCube);

	shObject arrow = shObject(new eObject);
	arrow->SetModel(modelManager->Find("arrow").get());
	arrow->SetName("arrow");
	arrow->SetTransform(new Transform);
	arrow->SetCollider(new BoxCollider);
	arrow->SetRigidBody(new eRigidBody);
	arrow->GetRigidBody()->SetObject(arrow.get());
	arrow->GetCollider()->CalculateExtremDots(arrow->GetModel()->GetPositions());
	arrow->GetTransform()->setTranslation(vec3(1.0f, 1.0f, -1.0f));
	m_objects.push_back(arrow);

	shObject grassPlane = shObject(new eObject);
	grassPlane->SetModel(modelManager->Find("grass_plane").get());
	grassPlane->SetTransform(new Transform);
	grassPlane->SetCollider(new BoxCollider);
	grassPlane->SetRigidBody(new eRigidBody);
	grassPlane->GetRigidBody()->SetObject(grassPlane.get());
	grassPlane->GetCollider()->CalculateExtremDots(grassPlane->GetModel()->GetPositions());
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_objects.push_back(grassPlane);

	shObject nanosuit = shObject(new eObject);
	nanosuit->SetModel(modelManager->Find("nanosuit").get());
	nanosuit->SetTransform(new Transform);
	nanosuit->SetCollider(new BoxCollider);
	nanosuit->SetRigidBody(new eRigidBody);
	nanosuit->GetRigidBody()->SetObject(nanosuit.get());
	nanosuit->GetCollider()->CalculateExtremDots(nanosuit->GetModel()->GetPositions());
	nanosuit->GetTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	m_objects.push_back(nanosuit);

	shObject terrain = shObject(new eObject);
	terrain->SetModel((IModel*)m_TerrainModel.get());
	terrain->SetTransform(new Transform);
	terrain->SetCollider(new BoxCollider);
	terrain->SetRigidBody(new eRigidBody);
	terrain->GetRigidBody()->SetObject(terrain.get());
	terrain->GetCollider()->CalculateExtremDots(terrain->GetModel()->GetPositions());
	terrain->SetName("Terrain");
	terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	terrain->GetTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
	m_objects.push_back(terrain);

	shObject wolf = shObject(new eObject);
	wolf->SetModel(modelManager->Find("wolf").get());
	wolf->SetTransform(new Transform);
	wolf->SetCollider(new BoxCollider);
	wolf->SetRigidBody(new eRigidBody);
	wolf->GetRigidBody()->SetObject(wolf.get());
	wolf->GetCollider()->CalculateExtremDots(wolf->GetModel()->GetPositions());
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setTranslation(vec3(5.0f, 3.0f, 0.0f));
	wolf->SetRigger(new Rigger((Model*)modelManager->Find("wolf").get()));
	wolf->GetRigger()->ChangeName(std::string(), "Running");
	m_objects.push_back(wolf);

	shObject brickCube = shObject(new eObject);
	brickCube->SetModel(modelManager->Find("brick_cube").get());
	brickCube->SetTransform(new Transform);
	brickCube->SetCollider(new BoxCollider);
	brickCube->SetRigidBody(new eRigidBody);
	brickCube->GetRigidBody()->SetObject(brickCube.get());
	brickCube->GetCollider()->CalculateExtremDots(brickCube->GetModel()->GetPositions());
	brickCube->GetTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	m_objects.push_back(brickCube);

	shObject guard = shObject(new eObject);
	guard->SetModel(modelManager->Find("guard").get());
	guard->SetTransform(new Transform);
	guard->SetCollider(new BoxCollider);
	guard->SetRigidBody(new eRigidBody);
	guard->GetRigidBody()->SetObject(guard.get());
	guard->GetCollider()->CalculateExtremDots(guard->GetModel()->GetPositions());
	guard->GetTransform()->setTranslation(vec3(2.0f, 2.0f, 0.0f));
	guard->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	guard->GetTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
	guard->SetRigger(new Rigger((Model*)modelManager->Find("guard").get()));
	m_objects.push_back(guard);

	//light_cube
	lightObject = shObject(new eObject);
	lightObject->SetModel(modelManager->Find("white_sphere").get());
	lightObject->SetTransform(new Transform);
	lightObject->SetCollider(new BoxCollider);
	lightObject->SetRigidBody(new eRigidBody);
	lightObject->GetRigidBody()->SetObject(lightObject.get());
	lightObject->GetCollider()->CalculateExtremDots(lightObject->GetModel()->GetPositions());
	lightObject->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	lightObject->GetTransform()->setTranslation(m_light.light_position);
	m_objects.push_back(lightObject);

	/*shObject boat = shObject(new eObject(modelManager.Find("boat").get()));
	boat->GetTransform()->setScale(vec3(0.0001f, 0.0001f, 0.0001f));
	m_objects.push_back(boat);*/
}

//-------------------------------------------------------------------------
void eMainContext::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
	pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f), texManager->Find("Tatlas2"), soundManager->GetSound("shot_sound"), texManager->Find("Tatlas2")->numberofRows));
}

//#include "GlBufferContext.h" //temp need to get rid
//-------------------------------------------------------------------------------
void eMainContext::PaintGL()
{
	eMainContextBase::PaintGL();

	//just for test
	//if (!guis.empty())
	//{
	//	guis[0].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION));
	//	//8.2 Second quad
	//	guis[1].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION));
	//}

	std::vector<Flag> flags;
	for (auto &object : m_objects)
	{
		if (object->GetScript())
		{
			object->GetScript()->Update(m_objects);
			eShipScript* script = dynamic_cast<eShipScript*>(object->GetScript());
			if(script) flags.push_back(script->GetFlag(m_camera));
		}
	}

  lightObject->GetTransform()->setTranslation(m_light.light_position);
	//m_light.light_position = { lightObject->GetTransform()->getTranslation(), 1.0f };
	m_light.light_direction = { -m_light.light_position };
	
	std::vector<shObject> focused{ m_focused };
	pipeline.RanderFrame(m_camera, m_light, guis, m_focused ? focused : std::vector<shObject>{}, flags); //better design impl new standard
}