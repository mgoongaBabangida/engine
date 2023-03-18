#include "stdafx.h"
#include "AmericanTreasureGame.h"

#include <base/InputController.h>

#include <math/Random.h>
#include <math/Camera.h>
#include <math/CameraRay.h>

#include <opengl_assets/OpenGlRenderPipeline.h>
#include <opengl_assets/GUI.h>
#include <opengl_assets/Sound.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ObjectFactory.h>

#include "Hex.h"
#include "ATGameClasses.h"

#include <math.h>
#include <algorithm>

//-------------------------------------------------------------------------------------------
class eTurnController : public ICommand
{
public:
	eTurnController(eAmericanTreasureGame& gc) : game_context(gc) {}

	virtual void Execute() override
	{
		if (game_context.get().focused && !game_context.get().focused->HasMoved())
		{
			current_dice = math::Random::RandomInt(1, 6);
			dice_rolled = true;
		}
	}

	//-------------------------------------------------------------------------------------------
  void Update()
  {
    //hex_distance multiplied by shooting distance
    {
      if (glm::length(shooter->GetTransform()->getTranslation() - target->GetTransform()->getTranslation()) <= (radius))
      {
        //Calculate fight outcome
      }
    }
  }

	//-------------------------------------------------------------------------------------------
	Texture* GetDiceTexture()
	{
		switch (current_dice)
		{
		case 1: return game_context.get().texManager->Find("tex_dice1"); break;
		case 2: return game_context.get().texManager->Find("tex_dice2"); break;
		case 3: return game_context.get().texManager->Find("tex_dice3"); break;
		case 4: return game_context.get().texManager->Find("tex_dice4"); break;
		case 5: return game_context.get().texManager->Find("tex_dice5"); break;
		case 6: return game_context.get().texManager->Find("tex_dice6"); break;
		}
		return nullptr; //assert
	}

	//-------------------------------------------------------------------------------------------
	bool CanBeChosen(std::shared_ptr<eShip> _ship)
	{
		return (_ship->IsSpanish() && spanish_turn) || (!_ship->IsSpanish() && !spanish_turn)
				&& (!game_context.get().focused || game_context.get().focused->HasMoved());
	}

	//------------------------------------------------------------------------------------------
	int32_t CurrentDice() { return current_dice; }

	//-------------------------------------------------------------------------------------------
	void CheckAllMoved()
	{
		if (std::find_if(game_context.get().ships.begin(), game_context.get().ships.end(),
			[this](std::shared_ptr<eShip> ship) { return ship->IsSpanish() == spanish_turn && !ship->HasMoved(); })
			== game_context.get().ships.end())
		{
			spanish_turn = !spanish_turn;
			for (auto ship : game_context.get().ships)
				ship->SetHasMoved(false);
		}
	}

	//----------------------------------------------------------------------------------------------
	bool DiceRolled()		{ return dice_rolled; }
	//----------------------------------------------------------------------------------------------
	void ResetDiceRolled()	{ dice_rolled = false; }

	std::shared_ptr<eShip> shooter = nullptr; //make setters or friend?
	std::shared_ptr<eShip> target  = nullptr;

protected:
	std::reference_wrapper<eAmericanTreasureGame> game_context;
	int32_t	current_dice = 6;
	bool	spanish_turn = true;
	bool	dice_rolled = false;
};

//-------------------------------------------------------------------------------------------
eAmericanTreasureGame::eAmericanTreasureGame(eInputController* _input,
                       std::vector<IWindowImGui*> _externalGui,
											 const std::string& _modelsPath,
											 const std::string& _assetsPath,
											 const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
, camRay(new dbb::CameraRay())
, pipeline(new eOpenGlRenderPipeline(width, height))
, camera(new Camera(width, height, nearPlane, farPlane))
{
	_externalGui[0]->Add(SLIDER_FLOAT, "Ydir", &light.light_position.y);
	_externalGui[0]->Add(SLIDER_FLOAT, "Zdir", &light.light_position.z);
	_externalGui[0]->Add(SLIDER_FLOAT, "Xdir", &light.light_position.x);

	_externalGui[0]->Add(SLIDER_FLOAT, "camera positoin X", &camera->ViewDirectionRef().x);
	_externalGui[0]->Add(SLIDER_FLOAT, "camera positoin Y", &camera->ViewDirectionRef().y);
	_externalGui[0]->Add(SLIDER_FLOAT, "camera positoin Z", &camera->ViewDirectionRef().z);
	
	//Light init!
	light.ambient = vec3(0.3f, 0.3f, 0.3f);
	light.diffuse = vec3(0.65f, 0.65f, 0.65f);
	light.specular = vec3(0.5f, 0.5f, 0.5f);
}

//-------------------------------------------------------------------------------------------
bool eAmericanTreasureGame::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
		//for debuf temp
	case ASCII_G: { if (focused)	focused->GetScript()->OnKeyPress(ASCII_G); }	return true;
	default: return false;
	}
}

//-------------------------------------------------------------------------------------------
bool eAmericanTreasureGame::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	turn_context->CheckAllMoved();

	camRay->Update(*camera, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
	camRay->press(static_cast<float>(x), static_cast<float>(y));
	auto clicked = camRay->calculateIntersaction(m_objects).first;
	
	if (clicked != nullptr && clicked->Name() == "Terrain")
		clicked = nullptr;

	for (auto ship : ships)
	{
		if (left)
		{
			if (ship == clicked && turn_context->CanBeChosen(ship))
			{
				focused = std::static_pointer_cast<eShip>(clicked);
				break;
			}
			else if (ship != clicked && ship == ships.back())// left click with no ship
			{
				focused = nullptr;
			}
		}
		else //right click
		{
			if (ship == clicked && focused && clicked != focused
				&& ship->IsSpanish() != focused->IsSpanish()
				&& !focused->HasMoved()
				&& turn_context->DiceRolled())// right click on other ship try to attack
			{
				//unite for loops
				auto target_hex = std::find_if(hexes.begin(), hexes.end(), [this, ship](const Hex& hex)
				{
					return glm::length(ship->GetTransform()->getTranslation() - glm::vec3{ hex.x(), waterHeight, hex.z() }) < (0.5f * 0.57f); //radius
				});
				auto focused_hex = std::find_if(hexes.begin(), hexes.end(), [this](const Hex& hex)
				{
					return glm::length(focused->GetTransform()->getTranslation() - glm::vec3{ hex.x(), waterHeight, hex.z() }) < (0.5f * 0.57f); //radius
				});
				if (target_hex != hexes.end() && focused_hex != hexes.end())
				{
					std::vector<Hex*> path = focused_hex->MakePath(&(*target_hex));
					if (path.size() > 1)
					{
						glm::vec3 destination = { path[path.size() - 2]->x(), waterHeight, path[path.size() - 2]->z() };
						focused->getShipScript()->SetDestination(destination);
						focused->SetHasMoved(true);
						focused->getShipScript()->SetShootAfterMove(true);
						turn_context->ResetDiceRolled();
					}
					else if (path.size() == 1)
					{
						focused->getShipScript()->Shoot();
					}
					turn_context->shooter = focused;
					turn_context->target  = ship;
				}
				else
				{
					assert("can not find current hex!");
				}
			}
		}
	}
		for (auto& base : bases)
		{
			if (base == clicked && focused && !left) // right click on a base
			{
				break;
			}
		}

		if (focused && !clicked && !left && focused->GetScript()) // right click on water
		{
			dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f),
				glm::vec3(0.0f, waterHeight, 0.0f),
				glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane
			glm::vec3 target = dbb::intersection(pl, camRay->getLine());
			for (auto& hex : hexes)
			{
				if (hex.IsOn(target.x, target.z) && hex.IsWater(terrain, waterHeight))
				{
					auto cur_hex = std::find_if(hexes.begin(), hexes.end(), [this](const Hex& hex) 
					{ 
						return glm::length(focused->GetTransform()->getTranslation() - glm::vec3{ hex.x(), waterHeight, hex.z() }) < (0.5f * 0.57f); //radius
					});
					if (cur_hex != hexes.end() && turn_context->DiceRolled())
					{
						std::vector<Hex*> path = cur_hex->MakePath(&hex);
						if (!path.empty())
						{
							glm::vec3 destination = { path.back()->x(), waterHeight, path.back()->z() };
							focused->getShipScript()->SetDestination(destination);
							focused->SetHasMoved(true);
							turn_context->ResetDiceRolled();
						}
					}
					else
					{
						assert("can not find current hex!");
					}
				}						
			}
		}

	if (focused)
	{
		externalGui[0]->Add(SLIDER_FLOAT, "focused positoin X", &focused->GetTransform()->getTranslationRef().x);
    externalGui[0]->Add(SLIDER_FLOAT, "focused positoin Y", &focused->GetTransform()->getTranslationRef().y);
    externalGui[0]->Add(SLIDER_FLOAT, "focused positoin Z", &focused->GetTransform()->getTranslationRef().z);
	}
	return true;
}

//-------------------------------------------------------------------------------------------
void eAmericanTreasureGame::InitializePipline()
{
	pipeline->Initialize();
	pipeline->SwitchSkyBox(false);
}

//-------------------------------------------------------------------------------------------
void eAmericanTreasureGame::InitializeBuffers()
{
	//add posibility to init buffers separately
	pipeline->InitializeBuffers();
}

//-------------------------------------------------------------------------------------------
void eAmericanTreasureGame::InitializeModels()
{
	turn_context = std::make_shared<eTurnController>(*this);

	eMainContextBase::InitializeModels();

	//MODELS
	modelManager->Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());

	//TERRAIN MODEL
	terrainModel = modelManager->CloneTerrain("simple");
	terrainModel->initialize(texManager->Find("Tgrass0_d"),
							 texManager->Find("Tgrass0_d"),
							 texManager->Find("Tblue"),
							 texManager->Find("TOcean0_s"));
	terrain = std::make_shared<eTerrain>(terrainModel.get());

	//BASES
	std::shared_ptr<eBase> wallCube = std::shared_ptr<eBase>(new eBase(modelManager->Find("wall_cube").get(), new eBaseScript(texManager->Find("TPirate_flag0_s"))));
	wallCube->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	wallCube->GetTransform()->setTranslation(vec3(1.3f, 2.2f, 0.7f));
	m_objects.push_back(wallCube);
	bases.push_back(wallCube);

	std::shared_ptr<eBase> wallCube2 = std::shared_ptr<eBase>(new eBase(modelManager->Find("wall_cube").get(), new eBaseScript(texManager->Find("TPirate_flag0_s"))));
	wallCube2->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	wallCube2->GetTransform()->setTranslation(vec3(-0.5f, 2.2f, -0.5f));
	m_objects.push_back(wallCube2);
	bases.push_back(wallCube2);

	std::shared_ptr<eBase> brickCube = std::shared_ptr<eBase>(new eBase(modelManager->Find("brick_cube").get(), new eBaseScript(texManager->Find("TSpanishFlag0_s"))));
	brickCube->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	brickCube->GetTransform()->setTranslation(vec3(-1.3f, 2.2f, 3.5f));
	m_objects.push_back(brickCube);
	bases.push_back(brickCube);

	std::shared_ptr<eBase> brickCube2 = std::shared_ptr<eBase>(new eBase(modelManager->Find("brick_cube").get(), new eBaseScript(texManager->Find("TSpanishFlag0_s"))));
	brickCube2->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	brickCube2->GetTransform()->setTranslation(vec3(1.5f, 2.2f, 3.5f));
	m_objects.push_back(brickCube2);
	bases.push_back(brickCube2);

	std::shared_ptr<eBase> brickCube3 = std::shared_ptr<eBase>(new eBase(modelManager->Find("brick_cube").get(), new eBaseScript(texManager->Find("TSpanishFlag0_s"))));
	brickCube3->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	brickCube3->GetTransform()->setTranslation(vec3(1.5f, 2.2f, -3.0f));
	m_objects.push_back(brickCube3);
	bases.push_back(brickCube3);

	std::shared_ptr<eBase> brickCube4 = std::shared_ptr<eBase>(new eBase(modelManager->Find("brick_cube").get(), new eBaseScript(texManager->Find("TSpanishFlag0_s"))));
	brickCube4->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	brickCube4->GetTransform()->setTranslation(vec3(-1.3f, 2.2f, -3.5f));
	m_objects.push_back(brickCube4);
	bases.push_back(brickCube4);

	std::shared_ptr<eTerrain> terrain = std::shared_ptr<eTerrain>(new eTerrain(terrainModel.get(), "Terrain"));
	terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	terrain->GetTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
	m_objects.push_back(terrain);

	//SHIPS
	//1
	//need to load listeners for sounds somewhere, sounds should be copied
	auto* shipScript = new eShipScript(texManager->Find("TSpanishFlag0_s"),
										*pipeline,
		                *camera.get(),
										texManager->Find("Tatlas2"),
										soundManager->GetSound("shot_sound"),
										camRay.get(),
										waterHeight);

	std::shared_ptr<eShip> nanosuit = std::make_shared<eShip>(modelManager->Find("nanosuit").get(), shipScript, true, "spanish_1");
	nanosuit->GetTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->GetTransform()->setScale(vec3(0.03f, 0.03f, 0.03f));
	ships.push_back(nanosuit);
	m_objects.push_back(nanosuit);
	
	//2
	//need to load listners for sounds somewhere, sounds should be copied
	auto* shipScript2 = new eShipScript(texManager->Find("TPirate_flag0_s"),
		*pipeline,
		*camera.get(),
		texManager->Find("Tatlas2"),
		soundManager->GetSound("shot_sound"),
		camRay.get(),
		waterHeight);

	std::shared_ptr<eShip> nanosuit2 = std::make_shared<eShip>(modelManager->Find("nanosuit").get(), shipScript2, false, "pirat_1");
	nanosuit2->GetTransform()->setTranslation(vec3(1.0f, 2.0f, 1.0f));
	nanosuit2->GetTransform()->setScale(vec3(0.03f, 0.03f, 0.03f));
	ships.push_back(nanosuit2);
	m_objects.push_back(nanosuit2);

	//3
	//need to load listeners for sounds somewhere, sounds should be copied
	auto* shipScript3 = new eShipScript(texManager->Find("TSpanishFlag0_s"),
		*pipeline,
		*camera.get(),
		texManager->Find("Tatlas2"),
		soundManager->GetSound("shot_sound"),
		camRay.get(),
		waterHeight);

	std::shared_ptr<eShip> nanosuit3 = std::make_shared<eShip>(modelManager->Find("nanosuit").get(), shipScript3, true, "spanish_2");
	nanosuit3->GetTransform()->setTranslation(vec3(1.0f, 2.0f, -1.0f));
	nanosuit3->GetTransform()->setScale(vec3(0.03f, 0.03f, 0.03f));
	ships.push_back(nanosuit3);
	m_objects.push_back(nanosuit3);

	//4
	//need to load listners for sounds somewhere, sounds should be copied
	auto* shipScript4 = new eShipScript(texManager->Find("TPirate_flag0_s"),
		*pipeline,
		*camera.get(),
		texManager->Find("Tatlas2"),
		soundManager->GetSound("shot_sound"),
		camRay.get(),
		waterHeight);

	std::shared_ptr<eShip> nanosuit4 = std::make_shared<eShip>(modelManager->Find("nanosuit").get(), shipScript4, false, "pirat_2");
	nanosuit4->GetTransform()->setTranslation(vec3(1.0f, 2.0f, 2.0f));
	nanosuit4->GetTransform()->setScale(vec3(0.03f, 0.03f, 0.03f));
	ships.push_back(nanosuit4);
	m_objects.push_back(nanosuit4);

	//Camera Ray
	camRay->init(static_cast<float>(width), static_cast<float>(height), nearPlane, farPlane);

	inputController->AddObserver(this, STRONG);
	inputController->AddObserver(camera.get(), WEAK);
	inputController->AddObserver(camRay.get(), WEAK);

	guis.emplace_back(new GUI(width / 4 * 3, height / 4 * 3, width / 4, height / 4, width, height));
	guis[0]->setCommand(turn_context);
	inputController->AddObserver(guis[0].get(), MONOPOLY);
}

//-------------------------------------------------------------------------------------------
void eAmericanTreasureGame::InitializeRenders()
{
	_InitializeHexes();

	pipeline->InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
	//texManager.AddTextureBox(pipeline->GetSkyNoiseTexture(), "noise_skybox"); //test
	pipeline->SetSkyBoxTexture(texManager->Find("TcubeSkyWater2")); //TcubeSkyWater2
}

//-------------------------------------------------------------------------------------------
void eAmericanTreasureGame::_InitializeHexes()
{
	float z_move = glm::sin(glm::radians(240.0f)) * radius;
	for (int i = -6; i < 6; ++i)
		for (int j = -5; j < 5; ++j)
		{
			hexes.push_back(glm::vec2(glm::cos(glm::radians(0.0f)) * radius * i,					z_move * 2 * j));
			hexes.push_back(glm::vec2(glm::cos(glm::radians(60.0f)) * radius * i * 2 + radius / 2,  z_move + z_move * 2 * j));
		}
	std::vector<glm::vec3> dots;
	for (auto& hex : hexes)
	{
		hex.SetNeighbour(hexes);
		dots.emplace_back(glm::vec3{ hex.x(), common_height,  hex.z() });
		//hex.Debug();
	}
	auto* mesh = new SimpleGeometryMesh(dots, radius);//move
	ObjectFactoryBase factory;
	hex_model = factory.CreateObject(std::make_shared<SimpleModel>(mesh));
}

//-------------------------------------------------------------------------------------------
void eAmericanTreasureGame::PaintGL()
{
	eMainContextBase::PaintGL();

	angle += 0.003f;
	light.light_position = vec4(sin(angle) * 4.0f, cos(angle) * 4.0f, 0.0f, 1.0f);
	
	turn_context->Update();

	guis[0]->SetTexture(*turn_context->GetDiceTexture(), { 0,0 },
		{ turn_context->GetDiceTexture()->mTextureWidth, turn_context->GetDiceTexture()->mTextureHeight });

	std::vector<shObject> flags;
	for (auto &ship : ships)
	{
		if (ship->GetScript())
		{
			ship->GetScript()->Update(m_objects);
			flags.push_back(ship->getShipScript()->GetChildrenObjects()[0]);
		}
	}

	for(auto &base : bases)
		flags.push_back(base->getBaseScript()->GetChildrenObjects()[0]);
	
	std::map<eOpenGlRenderPipeline::RenderType, std::vector<shObject>> objects;
	objects.insert({ eOpenGlRenderPipeline::RenderType::MAIN, m_objects });
	objects.insert({ eOpenGlRenderPipeline::RenderType::GEOMETRY, {hex_model} });
	if(focused)
		objects.insert({ eOpenGlRenderPipeline::RenderType::OUTLINED, std::vector<shObject>{ focused } });
	else
		objects.insert({ eOpenGlRenderPipeline::RenderType::OUTLINED, std::vector<shObject>{} });
	objects.insert({ eOpenGlRenderPipeline::RenderType::FLAG, flags });
	pipeline->RenderFrame(objects, *camera.get(), light, guis);
}
