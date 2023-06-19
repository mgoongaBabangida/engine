#include "GameController.h"

#include <base/base.h>

#include <math/Camera.h>
#include <math/Random.h>

#include <opengl_assets/ModelManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/Sound.h>
#include <opengl_assets/openglrenderpipeline.h>

#include <game_assets/MainContextBase.h>
#include <game_assets/ShipScript.h>
#include <game_assets/ObjectFactory.h>

#include <deque>

//------------------------------------------------
GameController::GameController(eMainContextBase* _game,
                               eModelManager* _modelManager,
                               eTextureManager* _texManager,
                               eSoundManager* _soundManager,
                               eOpenGlRenderPipeline& _pipeline,
                               Camera& _camera)
  : m_game(_game)
  , m_modelManager(_modelManager)
  , m_texManager(_texManager)
  , m_soundManager(_soundManager)
  , m_pipeline(_pipeline)
  , m_camera(_camera)
  , m_has_moved{ false, false ,false ,false }
{
}

//------------------------------------------------
void GameController::Initialize()
{
  m_game->GetMainLight().light_position = glm::vec4{ 0.0f, 4.0f,0.0f, 1.0f };
  m_game->GetMainLight().intensity = glm::vec3{ 150.0f, 150.0f , 150.0f };
  m_game->GetMainCamera().setPosition(glm::vec3{-4.0f, 4.0f, 0.0f});
  m_game->GetMainCamera().setDirection(glm::normalize(glm::vec3{ 0.815, -0.588f, -0.018f }));

  m_game->ObjectPicked.Subscribe([this](shObject _picked)->void { this->OnObjectPicked(_picked); });

  _InitializeDiceLogicAndVisual();
  _InitializeShipIcons();
  _InitializeHexes();
  _InitializeShips();

  m_game->AddInputObserver(this, STRONG);
}

//----------------------------------------------------------------
bool GameController::OnMouseMove(int32_t _x, int32_t _y)
{
  m_cursor_pos = { _x , _y };
  return false;
}

//----------------------------------------------------------------
bool GameController::OnMousePress(int32_t _x, int32_t _y, bool _left)
{
  if (!_left)
  {
    if (!m_game->GetFocusedObject() || _CurrentShipHasMoved() || !m_dice_rolled || !m_current_path.empty())
      return true;
    else // player rolled dice and has not moved yet. so, lets calculate wheather and how they can move
    {
    auto focused = m_game->GetFocusedObject();
    
    // RIGHT CLICK ON WATER
      float waterHeight = m_pipeline.get().GetWaterHeight();
      dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f),
                    glm::vec3(0.0f, waterHeight, 0.0f),
                    glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane

      glm::vec3 target = dbb::intersection(pl, m_camera.get().getCameraRay().getLine());
      for (auto& hex : m_hexes)
      {
        if (hex.IsOn(target.x, target.z)  && hex.IsWater(m_terrain, waterHeight))
        {
          auto cur_hex = std::find_if(m_hexes.begin(), m_hexes.end(), [this , focused, waterHeight](const Hex& hex)
          {
            return glm::length(focused->GetTransform()->getTranslation() - glm::vec3{ hex.x(), waterHeight, hex.z() }) < (0.5f * 0.57f); //radius
          });

          if (cur_hex != m_hexes.end())
          {
            m_current_path = cur_hex->MakePath(&hex, m_terrain, waterHeight);
            if (m_current_path.size() > m_current_dice) // can move less or equal to dice
            {
              m_current_path.clear();
              return true;
            }
            if (!m_current_path.empty())
            {
              glm::vec3 destination = { m_current_path.front()->x(), waterHeight, m_current_path.front()->z() };
              m_current_path.pop_front();
              m_ships[m_focused_index]->SetDestination(destination);
              return true;
            }
          }
          else
          {
            assert("can not find current hex!");
          }
        }
      }
    }
  }
  return false;
}

//----------------------------------------------------------------
bool GameController::OnMouseRelease()
{
  for (int i = 0; i < m_ships.size(); i++)
  {
    if (m_ships[i]->GetScriptObject() == m_game->GetFocusedObject().get()
      && m_ships[i]->GetDestination() != NONE)
    {
      m_dice_rolled = false;
      m_has_moved[i] = true;
    }
  }
  return false;
}

//-------------------------------------------------------------
void GameController::Update(float _tick)
{
  for (int i = 0; i < m_ships.size(); i++)
  {
    if (m_ships[i]->GetScriptObject() == m_game->GetFocusedObject().get()
      && m_ships[i]->GetDestination() == NONE
      && !m_current_path.empty())
    {
      glm::vec3 destination = { m_current_path.front()->x(), m_pipeline.get().GetWaterHeight(), m_current_path.front()->z() };
      m_current_path.pop_front();
      m_ships[m_focused_index]->SetDestination(destination);
    }
  }

  if (std::all_of(m_has_moved.begin(), m_has_moved.end(), [](bool _val) { return _val; }))
  {
    std::for_each(m_has_moved.begin(), m_has_moved.end(), [](bool& _val) { _val = false; });
  }
}

//---------------------------------------------------------------
void GameController::OnObjectPicked(std::shared_ptr<eObject> _picked)
{
  if (!m_dice_rolled)
  {
    m_game->SetFocused(_picked);
    for (int i = 0; i < m_ships.size(); i++)
    {
      if (m_ships[i]->GetScriptObject() == _picked.get())
      {
        m_ship_icons[i]->SetRenderingFunc(GUI::RenderFunc::Default);
        m_focused_index = i;
      }
      else
        m_ship_icons[i]->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
    }
  }
}

//------------------------------------------------------------
void GameController::_InitializeDiceLogicAndVisual()
{
  float pos_x = 10.0f;
  float pos_y = m_game->Height() - 125.0f - 10.0f;
  glm::vec2 icon_size = { 125.0f , 125.0f };
  std::shared_ptr<GUI> dice_gui = std::make_shared<GUI>(pos_x, pos_y, icon_size.x, icon_size.y, m_game->Width(), m_game->Height());
  auto dice_tex = _GetDiceTexture();
  dice_gui->SetTexture(*dice_tex, { 0,0 }, { dice_tex->mTextureWidth, dice_tex->mTextureHeight });
  dice_gui->setCommand(std::make_shared<GUICommand>([this, dice_gui]()
    {
      if (!m_dice_rolled && m_game->GetFocusedObject() && !_CurrentShipHasMoved())
      {
        m_current_dice = math::Random::RandomInt(1, 6);
        m_dice_rolled = true;
        auto dice_tex = _GetDiceTexture();
        dice_gui->SetTexture(*dice_tex, { 0,0 }, { dice_tex->mTextureWidth, dice_tex->mTextureHeight });
      }
    }));
  m_game->AddGUI(dice_gui);
  m_game->AddInputObserver(dice_gui.get(), MONOPOLY);
}

//------------------------------------------------------------
void GameController::_InitializeShipIcons()
{
  glm::vec2 icon_size = { 50 , 50 };
  for (int i = 0; i < m_ship_quantity; i++)
  {
    float pos_x = (10 + icon_size.x) * i;
    float pos_y = 10;
    const Texture* ship_tex = m_game->GetTexture("ship1");
    std::shared_ptr<GUI> ship_gui = std::make_shared<GUI>(pos_x, pos_y, icon_size.x, icon_size.y, m_game->Width(), m_game->Height());
    ship_gui->SetTexture(*ship_tex, { 0,0 }, { ship_tex->mTextureWidth, ship_tex->mTextureHeight });
    ship_gui->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
    ship_gui->setCommand(std::make_shared<GUICommand>([this, ship_gui]()
    {
        for (int i = 0; i < m_ship_icons.size(); i++)
        {
          if (m_ship_icons[i]->isPressed(m_cursor_pos.x, m_cursor_pos.y))
          {
            if (!m_dice_rolled)
            {
              m_game->SetFocused(m_ships[i]->GetScriptObject());
              m_ship_icons[i]->SetRenderingFunc(GUI::RenderFunc::Default);
              m_focused_index = i;
            }
          }
          else
          {
            m_ship_icons[i]->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
          }
        }
      }));
    m_game->AddGUI(ship_gui);
    m_game->AddInputObserver(ship_gui.get(), MONOPOLY);
    m_ship_icons.push_back(ship_gui);
  }
}

//------------------------------------------------------------
void GameController::_InitializeHexes()
{
  float z_move = glm::sin(glm::radians(240.0f)) * Hex::radius;
  for (int i = -6; i < 6; ++i)
    for (int j = -5; j < 5; ++j)
    {
      m_hexes.push_back(glm::vec2(glm::cos(glm::radians(0.0f)) * Hex::radius * i, z_move * 2 * j));
      m_hexes.push_back(glm::vec2(glm::cos(glm::radians(60.0f)) * Hex::radius * i * 2 + Hex::radius / 2, z_move + z_move * 2 * j));
    }
  std::vector<glm::vec3> dots;
  for (auto& hex : m_hexes)
  {
    hex.SetNeighbour(m_hexes);
    dots.emplace_back(glm::vec3{ hex.x(), Hex::common_height,  hex.z() });
    //hex.Debug();
  }
  ObjectFactoryBase factory;
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(new SimpleGeometryMesh(dots, Hex::radius)), eObject::RenderType::GEOMETRY));
}

//------------------------------------------------------------
void GameController::_InitializeShips()
{
  //TERRAIN
  std::unique_ptr<TerrainModel> terrainModel = m_modelManager->CloneTerrain("simple");
  terrainModel->initialize(m_texManager->Find("Tgrass0_d"),
    m_texManager->Find("Tgrass0_d"),
    m_texManager->Find("Tblue"),
    m_texManager->Find("TOcean0_s"),
    true);

  //OBJECTS
  ObjectFactoryBase factory;

  m_terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()), eObject::RenderType::PHONG, "Terrain");
  m_terrain->SetName("Terrain");
  m_terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
  m_terrain->GetTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
  m_terrain->SetPickable(false);
  m_game->AddObject(m_terrain);

  Material material;
  material.albedo = glm::vec3(0.9f, 0.0f, 0.0f);
  material.ao = 1.0f;
  material.roughness = 0.5;
  material.metallic = 0.5;

  //@todo color change all objects that share the model, can not change colors for every ship for now
  std::vector<glm::vec3> colors{ glm::vec3(0.9f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 0.9f, 0.0f),
                                 glm::vec3(0.0f, 0.0f, 0.9f),
                                 glm::vec3(0.9f, 0.9f, 0.0f)};

  for (int i = 0; i < colors.size(); ++i)
  {
    //material.albedo = colors[i];
    shObject ship = factory.CreateObject(m_modelManager->Find("ship"), eObject::RenderType::PBR, "Ship" + std::to_string(i));
    ship->GetTransform()->setScale(vec3(0.0005f, 0.0005f, 0.0005f));
    ship->GetTransform()->setTranslation(vec3(m_hexes[90 + i*2].x(), m_pipeline.get().GetWaterHeight(), m_hexes[90 + i * 2].z()));
    
    ship->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
    ship->GetTransform()->setUp(glm::vec3(0.0f, 0.0f, 1.0f));
    ship->GetTransform()->setForward(glm::vec3(1.0f, 0.0f, 0.0f));

    for (auto& mesh : ship->GetModel()->Get3DMeshes())
      const_cast<I3DMesh*>(mesh)->SetMaterial(material); //@todo

    auto script = new eShipScript(m_game,
      m_texManager->Find("TSpanishFlag0_s"),
      m_pipeline,
      m_camera,
      m_texManager->Find("Tatlas2"),
      m_soundManager->GetSound("shot_sound"),
      m_pipeline.get().GetWaterHeight());
    ship->SetScript(script);
    m_ships.push_back(script);

    m_game->AddInputObserver(ship->GetScript(), WEAK);
    m_game->AddObject(ship);
  }
}

//------------------------------------------------------------
bool GameController::_CurrentShipHasMoved() const
{
  for (int i = 0; i < m_ships.size(); i++)
  {
    if (m_ships[i]->GetScriptObject() == m_game->GetFocusedObject().get()
      && m_has_moved[i] == true)
    {
      return true;
    }
  }
  return false;
}

//-------------------------------------------------------------
const Texture* GameController::_GetDiceTexture() const
{
  switch (m_current_dice)
  {
  case 1: return m_texManager->Find("tex_dice1"); break;
  case 2: return m_texManager->Find("tex_dice2"); break;
  case 3: return m_texManager->Find("tex_dice3"); break;
  case 4: return m_texManager->Find("tex_dice4"); break;
  case 5: return m_texManager->Find("tex_dice5"); break;
  case 6: return m_texManager->Find("tex_dice6"); break;
  }
  return nullptr; //assert
}
