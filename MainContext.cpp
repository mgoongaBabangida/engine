#include "stdafx.h"
#include "MainContext.h"
#include "ImGuiContext.h"

#include "ShipScript.h"
#include "Sound.h"
#include "Texture.h"

#include "ParticleSystem.h"
#include "ShootingParticleSystem.h"


eMainContext::eMainContext(eInputController* _input,
						   IWindowImGui*	 _guiWnd,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath, 
						   const std::string& _shadersPath)
: eMainContextBase(_input, _guiWnd, _modelsPath, _assetsPath, _shadersPath)
, pipeline(m_Objects, width, height, nearPlane, farPlane, waterHeight)
{
	_guiWnd->Add(SLIDER_FLOAT, "Ydir", m_light.light_position.y);
	_guiWnd->Add(SLIDER_FLOAT, "Zdir", m_light.light_position.z);
	_guiWnd->Add(SLIDER_FLOAT, "Xdir", m_light.light_position.x);

	//Light init!
	m_light.ambient			= vec3(0.1f, 0.1f, 0.1f);
	m_light.diffuse			= vec3(0.75f, 0.75f, 0.75f);
	m_light.specular		= vec3(0.5f, 0.5f, 0.5f);
	m_light.light_position	= vec4(1.0f, 4.0f, 1.0f, 1.0f);
}

bool eMainContext::OnMouseMove(uint32_t x, uint32_t y)
{
	m_framed = camRay.onMove(m_camera, m_Objects, x, y); 	//to draw a frame
	return true;
}

bool eMainContext::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	case ASCII_J:	{ if (m_focused)	m_focused->MoveLeft(m_Objects);}				return true;
	case ASCII_L:	{ if (m_focused)	m_focused->MoveRight(m_Objects);}				return true;
	case ASCII_K:	{ if (m_focused)	m_focused->MoveBack(m_Objects);}				return true;
	case ASCII_I:	{ if (m_focused)	m_focused->MoveForward(m_Objects);}				return true;
	case ASCII_Z:	{ if (m_focused)	m_focused->MoveUp(m_Objects);}					return true;
	case ASCII_X:	{ if (m_focused)	m_focused->MoveDown(m_Objects);}				return true;
	case ASCII_C:	{ if (m_focused)	m_focused->TurnRight(m_Objects);}				return true;
	case ASCII_V:	{ if (m_focused)	m_focused->TurnLeft(m_Objects);}				return true;
	case ASCII_B:	{ if (m_focused)	m_focused->LeanRight(m_Objects);}				return true;
	case ASCII_N:	{ if (m_focused)	m_focused->LeanLeft(m_Objects);}				return true;
	case ASCII_U:	{ if (m_focused)	m_focused->LeanForward(m_Objects);}				return true;
	case ASCII_H:	{ if (m_focused)	m_focused->LeanBack(m_Objects);}				return true;
	case ASCII_G:	{ if (m_focused)	m_focused->getScript()->OnKeyPress(ASCII_G);}	return true;
	default: return false;
	}
}

bool eMainContext::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	camRay.Update(m_camera, x, y, width, height);
	if(left)
	{
		camRay.press(x, y);
		m_focused = camRay.calculateIntersaction(m_Objects);
	}
	if(m_focused && m_focused->getScript())
	{
		m_focused->getScript()->OnMousePress(x, y, left);
	}
	return true;
}

bool eMainContext::OnMouseRelease()
{
	camRay.release();
	return true;
}

void eMainContext::InitializeGL()
{
	eMainContextBase::InitializeGL();
	
	//Camera Ray
	camRay.init(width, height, nearPlane, farPlane);
	
	m_Objects[4]->setScript(new eShipScript(texManager.Find("TSpanishFlag0_s"),
											pipeline.ParticleRender(),
											texManager.Find("Tatlas2"),
											soundManager.GetSound("shot_sound"),
											&camRay,
											waterHeight));
	m_focused = m_Objects[4];

	guis.push_back(GUI(0, 0, width / 4, height / 4, width, height));
	guis[0].setCommand(std::shared_ptr<ICommand>(new AnimStart(m_Objects[6])));
	guis.push_back(GUI(width / 4 * 3, height / 4 * 3, width / 4, height / 4, width, height));
	guis[1].setCommand(std::shared_ptr<ICommand>(new AnimStop(m_Objects[6])));

	inputController->AddObserver(this, STRONG);
	inputController->AddObserver(&guis[0], MONOPOLY);
	inputController->AddObserver(&guis[1], MONOPOLY);
	inputController->AddObserver(&m_camera, WEAK);
	inputController->AddObserver(&camRay, WEAK);
}

void eMainContext::InitializeSounds()
{
	//sound->loadListner(m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);
}

void eMainContext::InitializePipline()
{
	pipeline.Initialize();
	// call all the enable pipeline functions
}

void eMainContext::InitializeBuffers()
{
	pipeline.InitializeBuffers();
}

void eMainContext::InitializeModels()
{
	eMainContextBase::InitializeModels();

	//MODELS
	modelManager.Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());
	modelManager.Add("boat", (GLchar*)std::string(modelFolderPath + "Medieval Boat/Medieval Boat.obj").c_str());
	//modelManager.Add("spider", (GLchar*)std::string(ModelFolderPath + "ogldev-master/Content/spider.obj").c_str());
	modelManager.Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager.Add("guard", (GLchar*)std::string(modelFolderPath + "ogldev-master/Content/guard/boblampclean.md5mesh").c_str());
	//modelManager.Add("stairs", (GLchar*)std::string(modelFolderPath + "stairs.blend").c_str());

	//TERRAIN
	m_TerrainModel.swap(modelManager.CloneTerrain("simple"));
	m_TerrainModel->initialize(texManager.Find("Tgrass0_d"),
		texManager.Find("Tgrass0_d"),
		texManager.Find("Tblue"),
		texManager.Find("TOcean0_s"));

	//OBJECTS
	shObject wallCube = shObject(new eObject(modelManager.Find("wall_cube").get()));
	wallCube->getTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	m_Objects.push_back(wallCube);

	shObject containerCube = shObject(new eObject(modelManager.Find("container_cube").get()));
	containerCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, -3.5f));
	containerCube->getTransform()->setScale(vec3(0.2f, 0.2f, 0.2f));
	m_Objects.push_back(containerCube);

	shObject arrow = shObject(new eObject(modelManager.Find("arrow").get()));
	arrow->getTransform()->setTranslation(vec3(1.0f, 1.0f, -1.0f));
	m_Objects.push_back(arrow);

	shObject grassPlane = shObject(new eObject(modelManager.Find("grass_plane").get()));
	grassPlane->getTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_Objects.push_back(grassPlane);

	shObject nanosuit = shObject(new eObject(modelManager.Find("nanosuit").get()));
	nanosuit->getTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->getTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	m_Objects.push_back(nanosuit);

	shObject terrain = shObject(new eObject((IModel*)m_TerrainModel.get(), "Terrain"));
	terrain->getTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	terrain->getTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
	m_Objects.push_back(terrain);

	shObject wolf = shObject(new eObject(modelManager.Find("wolf").get()));
	wolf->getTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->getTransform()->setTranslation(vec3(5.0f, 3.0f, 0.0f));
	wolf->setRigger(new Rigger((Model*)modelManager.Find("wolf").get()));
	wolf->getRigger()->ChangeName(std::string(), "Running");
	m_Objects.push_back(wolf);

	shObject brickCube = shObject(new eObject(modelManager.Find("brick_cube").get()));
	brickCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	m_Objects.push_back(brickCube);

	shObject guard = shObject(new eObject(modelManager.Find("guard").get()));
	guard->getTransform()->setTranslation(vec3(2.0f, 2.0f, 0.0f));
	guard->getTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	guard->getTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
	guard->setRigger(new Rigger((Model*)modelManager.Find("guard").get()));
	m_Objects.push_back(guard);

	//light_cube
	lightObject = shObject(new eObject(modelManager.Find("white_sphere").get()));
	lightObject->getTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	lightObject->getTransform()->setTranslation(m_light.light_position);
	m_Objects.push_back(lightObject);

	/*shObject boat = shObject(new eObject(modelManager.Find("boat").get()));
	boat->getTransform()->setScale(vec3(0.0001f, 0.0001f, 0.0001f));
	m_Objects.push_back(boat);*/
}

void eMainContext::InitializeRenders()
{
	pipeline.InitializeRenders(modelManager, texManager, shadersFolderPath);
	pipeline.ParticleRender()->AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f), texManager.Find("Tatlas2"), soundManager.GetSound("shot_sound")));
}

void eMainContext::PaintGL()
{
	for (auto &object : m_Objects)
	{
		if (object->getScript())
			object->getScript()->Update(m_Objects);
	}
	lightObject->getTransform()->setTranslation(m_light.light_position);
	std::vector<shObject> focused{ m_focused };
	pipeline.RanderFrame(m_camera, m_light, guis, m_focused ? focused : std::vector<shObject>{} ); //better design
}