#include "stdafx.h"
#include "AmericanTreasureGame.h"
#include "Pipeline.h"
#include "ImGuiContext.h"
#include "Camera.h"
#include "CameraRay.h"
#include "GUI.h"

#include "ShipScript.h"
#include "Texture.h"
#include "Sound.h"

class eShip : public eObject
{
public:
	eShip() = delete;
	eShip(IModel* _m, eShipScript* _script, const std::string& _name = "empty")
		: eObject(_m, _name) 
	{
		script.reset(_script); 
		script->setObject(this);
	}
};

eAmericanTreasureGame::eAmericanTreasureGame(eInputController*  _input,
											 IWindowImGui*		_guiWnd,
											 const std::string& _modelsPath,
											 const std::string& _assetsPath,
											 const std::string& _shadersPath)
: eMainContextBase(_input, _guiWnd, _modelsPath, _assetsPath, _shadersPath)
, pipeline(new ePipeline(objects, width, height, nearPlane, farPlane, 2.0f)) //waterHeight is not initialized! @better design
, camera(new Camera())
, camRay(new dbb::CameraRay())
{
	_guiWnd->Add(SLIDER_FLOAT, "Ydir", light.light_position.y);
	_guiWnd->Add(SLIDER_FLOAT, "Zdir", light.light_position.z);
	_guiWnd->Add(SLIDER_FLOAT, "Xdir", light.light_position.x);

	//Light init!
	light.ambient = vec3(0.1f, 0.1f, 0.1f);
	light.diffuse = vec3(0.75f, 0.75f, 0.75f);
	light.specular = vec3(0.5f, 0.5f, 0.5f);
}

bool eAmericanTreasureGame::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	default: return false;
	}
}


bool eAmericanTreasureGame::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	camRay->Update(*camera, x, y, width, height);
	if (left)
	{
		camRay->press(x, y);
		focused = dynamic_pointer_cast<eShip>(camRay->calculateIntersaction(objects));
	}
	if (focused && focused->getScript())
	{
		focused->getScript()->OnMousePress(x, y, left);
	}
	return true;
}

void eAmericanTreasureGame::InitializePipline()
{
	pipeline->Initialize();
	pipeline->SwitchSkyBox(false);
}

void eAmericanTreasureGame::InitializeBuffers()
{
	//add posibility to init buffers separately
	pipeline->InitializeBuffers();
}

void eAmericanTreasureGame::InitializeModels()
{
	eMainContextBase::InitializeModels();

	//MODELS
	modelManager.Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());

	//TERRAIN MODEL
	terrainModel.swap(modelManager.CloneTerrain("simple"));
	terrainModel->initialize(texManager.Find("Tgrass0_d"),
							 texManager.Find("Tgrass0_d"),
							 texManager.Find("Tblue"),
							 texManager.Find("TOcean0_s"));

	//OBJECTS
	shObject wallCube = shObject(new eObject(modelManager.Find("wall_cube").get()));
	wallCube->getTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	objects.push_back(wallCube);

	shObject brickCube = shObject(new eObject(modelManager.Find("brick_cube").get()));
	brickCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	objects.push_back(brickCube);

	shObject terrain = shObject(new eObject((IModel*)terrainModel.get(), "Terrain"));
	terrain->getTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	terrain->getTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
	objects.push_back(terrain);

	//need to load listners for sounds somewhere, sounds should be copied
	auto* shipScript = new eShipScript(texManager.Find("TSpanishFlag0_s"),
										pipeline->ParticleRender(),
										texManager.Find("Tatlas2"),
										soundManager.GetSound("shot_sound"),
										camRay.get(),
										waterHeight);

	std::shared_ptr<eShip> nanosuit = std::make_shared<eShip>(modelManager.Find("nanosuit").get(), shipScript, "spanish_1");
	nanosuit->getTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->getTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	ships.push_back(nanosuit);
	objects.push_back(nanosuit);

	//Camera Ray
	camRay->init(width, height, nearPlane, farPlane);

	inputController->AddObserver(this, STRONG);
	/*inputController->AddObserver(&guis[0], MONOPOLY);
	inputController->AddObserver(&guis[1], MONOPOLY);*/
	inputController->AddObserver(camera.get(), WEAK);
	inputController->AddObserver(camRay.get(), WEAK);
}

void eAmericanTreasureGame::InitializeRenders()
{
	pipeline->InitializeRenders(modelManager, texManager, shadersFolderPath);
}

void eAmericanTreasureGame::PaintGL()
{
	angle += 0.01f;
	light.light_position = vec4(sin(angle) * 4.0f, cos(angle) * 4.0f, 0.0f, 1.0f);
	for (auto &ship : ships)
	{
		if (ship->getScript())
			ship->getScript()->Update(objects);
	}
	std::vector<shObject> focuseds { focused };
	pipeline->RanderFrame(*camera.get(), light, guis, focused ? focuseds : std::vector<shObject>{}); //better design
}