#include "GameController.h"

#include <base/base.h>

#include <math/Camera.h>
#include <math/Random.h>
#include <math/ParticleSystem.h>
#include <math/PlaneLine.h>

#include <tcp_lib/Network.h>
#include <tcp_lib/Server.h>
#include <tcp_lib/Client.h>

#include <opengl_assets/ModelManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/Sound.h>
#include <opengl_assets/openglrenderpipeline.h>
#include <opengl_assets/TerrainModel.h>
#include <opengl_assets/Sound.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/MainContextBase.h>
#include <game_assets/ObjectFactory.h>

#include <deque>

//------------------------------------------------
GameController::GameController(eMainContextBase* _game,
                               eModelManager* _modelManager,
                               eTextureManager* _texManager,
                               eSoundManager* _soundManager,
                               eOpenGlRenderPipeline& _pipeline,
                               Camera& _camera,
                               IWindowImGui* _debug_window)
  : m_game(_game)
  , m_modelManager(_modelManager)
  , m_texManager(_texManager)
  , m_soundManager(_soundManager)
  , m_pipeline(_pipeline)
  , m_camera(_camera)
  , m_debug_window(_debug_window)
  , m_has_moved{ false, false ,false ,false }
  , m_has_moved_pirate{ false, false ,false ,false, false, false }
  , m_texture_scales{ 10.0f, 7.0f , 4.0f}
{
  m_ship_height_level = m_pipeline.get().GetWaterHeight() - 0.05f;
}

//------------------------------------------------
GameController::~GameController()
{
  if(m_tcpAgent)
    dbb::NetWork::Shutdown();

  m_seagull_sound->Stop();
}

//------------------------------------------------
void GameController::Initialize()
{
  //Light
  m_game->GetMainLight().light_position = glm::vec4{ 0.0f, 1.0f, 0.0f, 1.0f };
  m_game->GetMainLight().intensity = glm::vec4{ 10.0f, 10.0f , 10.0f, 1.0f };

  ObjectFactoryBase factory;
  m_pipeline.get().SetUniformData("class ePhongRender", "emission_strength", 5.0f); //this does not work // This is common uniform
  shObject hdr_object = factory.CreateObject(m_modelManager->Find("white_sphere"), eObject::RenderType::PHONG, "LightObject"); // or "white_quad"
  if (hdr_object->GetModel()->GetName() == "white_sphere")
    hdr_object->GetTransform()->setScale(vec3(0.15f, 0.15f, 0.15f));
  hdr_object->GetTransform()->setTranslation(m_game->GetMainLight().light_position);
  std::array<glm::vec4, 4> points = { // for area light
                                      glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
                                      glm::vec4(1.0f, -1.0f, 0.0f, 1.0f),
                                      glm::vec4(-1.0f, -1.0f, 0.0f, 1.0f),
                                      glm::vec4(1.0f, 1.0f, 0.0f, 1.0f) };
  m_game->GetMainLight().points = points;

  Material m { vec3{}, 0.0f, 0.0f, 1.0f,
              Texture::GetTexture1x1(TColor::YELLOW).id, Texture::GetTexture1x1(TColor::WHITE).id,
              Texture::GetTexture1x1(TColor::BLUE).id,   Texture::GetTexture1x1(TColor::WHITE).id, Texture::GetTexture1x1(TColor::YELLOW).id,
              true, true, true, true };

  hdr_object->GetModel()->SetMaterial(m);
  m_game->AddObject(hdr_object);

  //Camera
  m_game->GetMainCamera().setPosition(glm::vec3{-3.193f, 5.45, -0.33f});
  m_game->GetMainCamera().setDirection(glm::normalize(glm::vec3{ 0.54f, -0.841f, -0.00f }));
  m_pipeline.get().GetOutlineFocusedRef() = false;
  m_pipeline.get().IBL() = false;

  m_game->EnableFrameChoice(false);

  m_game->ObjectPicked.Subscribe([this](shObject _picked, bool _left)->void
    {
      if(_left)
        OnObjectPickedWithLeft(_picked);
      else
        OnObjectPickedWithRight(_picked);
    });

  _InitializeDiceLogicAndVisual();
  _InitializeShipIcons();
  _InitializeGoldenFrame();
  _InitializeHexes();
  _InitializeShips();
  _InitializeBases();
  _InitializeSounds();

  m_choice_circle = new SimpleGeometryMesh({ NONE }, 0.15f, SimpleGeometryMesh::GeometryType::Circle, { m_hdr_strangth, m_hdr_strangth, 0.0f });
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_choice_circle), eObject::RenderType::GEOMETRY, "Choice circle"));
  m_path_mesh = new LineMesh({}, {}, glm::vec4{1.0f,1.0f,1.0f,1.0f});
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_path_mesh), eObject::RenderType::LINES, "Path mesh"));

  m_game->AddInputObserver(this, STRONG);

  m_destination_text = std::make_shared<Text>();
  m_destination_text->content = std::to_string(m_current_path.size());
  m_destination_text->font = "edwardian";
  m_destination_text->pos_x = 0.0f;
  m_destination_text->pos_y = 0.0f;
  m_destination_text->scale = 1.0f;
  m_destination_text->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_destination_text->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * UNIT_MATRIX;
  m_game->AddText(m_destination_text);

  m_warrining = std::make_shared<Text>();
  m_warrining->font = "ARIALN";
  m_warrining->pos_x = 200.0f;
  m_warrining->pos_y = 25.0f;
  m_warrining->scale = 1.0f;
  m_warrining->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_warrining->mvp = glm::ortho(0.0f, (float)m_game->Width(), 0.0f, (float)m_game->Height());
  m_game->AddText(m_warrining);

  m_game->ObjectBeingDeletedFromScene.Subscribe(std::bind(&GameController::_OnShipDeleted, this, std::placeholders::_1));

  //_InstallTcpServer();
  //_InstallTcpClient();

 /* m_side = Side::PIRATE;*/

  //DEBUG FIELDS
  std::function<void()> switch_mode__callback = [this]()
  {
    m_terrain->SetTextureBlending(!m_terrain->IsTextureBlending());
  };
  std::function<void()> debug_hexes = [this]()
  {
    this->_DebugHexes();
  };

  std::function<void()> switch_lod__callback = [this]()
  {
    if (TerrainModel* terrrain_model = dynamic_cast<TerrainModel*>(m_terrain->GetModel()); terrrain_model)
    {
      if (terrrain_model->getMeshes()[0]->LODInUse() == 1)
        terrrain_model->getMeshes()[0]->SwitchLOD(2);
      else if (terrrain_model->getMeshes()[0]->LODInUse() == 2)
        terrrain_model->getMeshes()[0]->SwitchLOD(3);
      else
        terrrain_model->getMeshes()[0]->SwitchLOD(1);
    }
  };

  std::function<void()> switch_rendering_mode_callback = [this]()
  {
    if (TerrainModel* terrrain_model = dynamic_cast<TerrainModel*>(m_terrain->GetModel()); terrrain_model)
    {
      if (terrrain_model->getMeshes()[0]->GetRenderMode() == MyMesh::RenderMode::DEFAULT)
        terrrain_model->getMeshes()[0]->SetRenderMode(MyMesh::RenderMode::WIREFRAME);
      else
        terrrain_model->getMeshes()[0]->SetRenderMode(MyMesh::RenderMode::DEFAULT);
    }
  };

  std::function<void()> send_msg_test__callback = [this]()
  {
    if(m_tcpAgent)
      m_tcpAgent->SendMsg(std::vector<uint32_t>{ 3, 1, 2, 3 });
  };

  m_debug_window->Add(BUTTON, "Terrain rendering", (void*)&switch_mode__callback);
  m_debug_window->Add(BUTTON, "Rendering Mode", (void*)&switch_rendering_mode_callback);
  m_debug_window->Add(SLIDER_FLOAT, "Texture Scale under watter", &m_texture_scales[0]);
  m_debug_window->Add(SLIDER_FLOAT, "Texture Scale grass", &m_texture_scales[1]);
  m_debug_window->Add(SLIDER_FLOAT, "Texture Scale grass 2", &m_texture_scales[2]);
  m_debug_window->Add(BUTTON, "Debug Hexes", (void*)&debug_hexes);
  m_debug_window->Add(SLIDER_FLOAT, "Height Scale", &m_height_scale);
  m_debug_window->Add(SLIDER_FLOAT, "Max Height", &m_max_height);
  m_debug_window->Add(SLIDER_FLOAT, "Water Level Height", &m_water_level);
  m_debug_window->Add(BUTTON, "Switch LOD", (void*)&switch_lod__callback);
  m_debug_window->Add(BUTTON, "Send Test Msg", (void*)&send_msg_test__callback);
  
  m_debug_window->Add(SLIDER_FLOAT, "HDR Strength", (void*)&m_hdr_strangth);

  m_debug_window->Add(TEXT, "Water debug", nullptr);
  m_debug_window->Add(SLIDER_FLOAT, "WaveSpeedFactor", (void*)&m_pipeline.get().WaveSpeedFactor());
  m_debug_window->Add(SLIDER_FLOAT, "Tiling", (void*)&m_pipeline.get().Tiling());
  m_debug_window->Add(SLIDER_FLOAT, "WaveStrength", (void*)&m_pipeline.get().WaveStrength());
  m_debug_window->Add(SLIDER_FLOAT, "ShineDumper", (void*)&m_pipeline.get().ShineDumper());
  m_debug_window->Add(SLIDER_FLOAT, "Reflactivity", (void*)&m_pipeline.get().Reflactivity());
  m_debug_window->Add(SLIDER_FLOAT_3, "WaterColor", (void*)&m_pipeline.get().WaterColor());
  m_debug_window->Add(SLIDER_FLOAT, "ColorMix", (void*)&m_pipeline.get().ColorMix());
  m_debug_window->Add(SLIDER_FLOAT, "RefrectionFactor", (void*)&m_pipeline.get().RefrectionFactor());
  m_debug_window->Add(SLIDER_FLOAT, "DistortionStrength", (void*)&m_pipeline.get().DistortionStrength());
}

//--------------------------------------------------------------------------
bool GameController::OnKeyPress(uint32_t _asci, KeyModifiers _modifier)
{
  shObject focused_obj;
  if(m_focused_index == -1)
    return false;

  if(m_game_state == GameState::SPANISH_TO_MOVE && m_focused_index < m_ships.size() && m_ships[m_focused_index]->GetScriptObject().lock())
    focused_obj = m_ships[m_focused_index]->GetScriptObject().lock();
  else if(m_game_state == GameState::PIRATE_TO_MOVE && m_focused_index < m_ships.size() && m_ships_pirate[m_focused_index]->GetScriptObject().lock())
    focused_obj = m_ships_pirate[m_focused_index]->GetScriptObject().lock();

  if (focused_obj && focused_obj.get())
  {
    switch (_asci)
    {
    case ASCII_J:
    {
      m_dice_gui->Perssed();
      break;
    }
    case ASCII_K:
    {
      OnShoot(focused_obj.get());
      break;
    }
    case ASCII_H:
    {
      OnGetHit(focused_obj.get());
      break;
    }
    return true;
    default: return false;
    }
  }
  return false;
}

//----------------------------------------------------------------
bool GameController::OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
  m_cursor_pos = { _x , _y };

  if (m_right_button_pressed)
  {
    _UpdateCurrentPath();
    if (!m_current_path.empty())
    {
      _UpdatePathVisual();
      _UpdateTextPath();
    }
  }
  return false;
}

//----------------------------------------------------------------
bool GameController::OnMousePress(int32_t _x, int32_t _y, bool _left, KeyModifiers _modifier)
{
  if (!_left && !m_target) // right click and no target
  {
    m_right_button_pressed = true;
    m_game->EnableHovered(true);

    _UpdateCurrentPath();
    if (!m_current_path.empty())
    {
      _UpdatePathVisual();
      _UpdateTextPath();
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------
bool GameController::OnMouseRelease(KeyModifiers _modifier)
{
  if (m_right_button_pressed)
  {
    if (!m_current_path.empty())
    {
      m_current_path.back()->SetTaken(true);
      if(auto cur_hex = _GetCurHex(); cur_hex)
        cur_hex->SetTaken(false);
      _UpdateWarrning("");

      m_destination_text->content = "";
      m_path_mesh->UpdateData({}, {}, glm::vec4{ 1.0f });
    }
    _SetDestinationFromCurrentPath();
  }
  
  m_right_button_pressed = false; // ? check if right
  m_game->EnableHovered(false);

  if (m_game_state == GameState::SPANISH_TO_MOVE)
  {
    for (int i = 0; i < m_ships.size(); ++i)
    {
      if (m_ships[i] == nullptr)
        continue;

      if (m_ships[i]->GetScriptObject().lock().get() == m_game->GetFocusedObject().get()
        && m_ships[i]->GetDestination() != NONE)
      {
        m_dice_rolled = false;
        m_has_moved[i] = true;
      }
    }
  }
  else if (m_game_state == GameState::PIRATE_TO_MOVE)
  {
    for (int i = 0; i < m_ships_pirate.size(); ++i)
    {
      if (m_ships_pirate[i] == nullptr)
        continue;

      if (m_ships_pirate[i]->GetScriptObject().lock().get() == m_game->GetFocusedObject().get()
          && m_ships_pirate[i]->GetDestination() != NONE)
      {
        m_dice_rolled = false;
        m_has_moved_pirate[i] = true;
      }
    }
  }
  m_target = nullptr;
  return false;
}

//-------------------------------------------------------------
void GameController::Update(float _tick)
{
  _UpdateLight(_tick);

  if (!m_right_button_pressed)
  {
    for (int i = 0; i < m_ships.size(); ++i)
    {
      if (m_ships[i] == nullptr)
        continue;

      if (shObject obj = m_ships[i]->GetScriptObject().lock())
      {
        if (obj && obj.get() == m_game->GetFocusedObject().get()
          && m_ships[i]->GetDestination() == NONE
          && !m_current_path.empty())
        {
          glm::vec3 destination = { m_current_path.front()->x(), m_ship_height_level, m_current_path.front()->z() };
          m_current_path.pop_front();
          m_ships[m_focused_index]->SetDestination(destination);
        }

        if (obj && obj.get() == m_game->GetFocusedObject().get())
          m_choice_circle->SetDots({ obj->GetTransform()->getTranslation() + glm::vec3{0.0f,0.06f,0.0f} }); //@todo
      }
    }

    for (int i = 0; i < m_ships_pirate.size(); ++i)
    {
      if (m_ships_pirate[i] == nullptr)
        continue;

      if (shObject obj = m_ships_pirate[i]->GetScriptObject().lock())
      {
        if (obj && obj.get() == m_game->GetFocusedObject().get()
          && m_ships_pirate[i]->GetDestination() == NONE
          && !m_current_path.empty())
        {
          glm::vec3 destination = { m_current_path.front()->x(), m_ship_height_level, m_current_path.front()->z() };
          m_current_path.pop_front();
          m_ships_pirate[m_focused_index]->SetDestination(destination);
        }

        if (obj && obj.get() == m_game->GetFocusedObject().get())
          m_choice_circle->SetDots({ obj->GetTransform()->getTranslation() + glm::vec3{0.0f,0.06f,0.0f} });//@todo
      }
    }
  }

  if (m_game_state == GameState::SPANISH_TO_MOVE)
  {
    if (std::all_of(m_has_moved.begin(), m_has_moved.end(), [](bool _val) { return _val; }))
    {
      std::for_each(m_has_moved.begin(), m_has_moved.end(), [](bool& _val) { _val = false; });
      for (int i = 0; i < m_ships.size(); ++i)
      {
        if (m_ships[i] == nullptr || m_ships[i]->GetDrowned())
          m_has_moved[i] = true;
      }
      m_game_state = GameState::PIRATE_TO_MOVE;
    }
  }
  else if (m_game_state == GameState::PIRATE_TO_MOVE)
  {
    if (std::all_of(m_has_moved_pirate.begin(), m_has_moved_pirate.end(), [](bool _val) { return _val; }))
    {
      std::for_each(m_has_moved_pirate.begin(), m_has_moved_pirate.end(), [](bool& _val) { _val = false; });
      for (int i = 0; i < m_ships.size(); ++i)
      {
        if (m_ships_pirate[i] == nullptr || m_ships_pirate[i]->GetDrowned())
          m_has_moved_pirate[i] = true;
      }
      m_game_state = GameState::SPANISH_TO_MOVE;
    }
  }

  //update icons move status
  for (int i = 0; i < m_status_icons.size(); ++i)
  {
    const Texture* red_tex = m_game->GetTexture("red_circle");
    const Texture* green_tex = m_game->GetTexture("green_circle");
    if (m_has_moved[i])
      m_status_icons[i]->SetTexture(*red_tex, { 0,0 }, { red_tex->mTextureWidth, red_tex->mTextureHeight });
    else
      m_status_icons[i]->SetTexture(*green_tex, { 0,0 }, { green_tex->mTextureWidth, green_tex->mTextureHeight });
  }

  //update icons move status pirate
  for (int i = 0; i < m_status_icons_pirate.size(); ++i)
  {
    const Texture* red_tex = m_game->GetTexture("red_circle");
    const Texture* green_tex = m_game->GetTexture("green_circle");
    if (m_has_moved_pirate[i])
      m_status_icons_pirate[i]->SetTexture(*red_tex, { 0,0 }, { red_tex->mTextureWidth, red_tex->mTextureHeight });
    else
      m_status_icons_pirate[i]->SetTexture(*green_tex, { 0,0 }, { green_tex->mTextureWidth, green_tex->mTextureHeight });
  }

  // update base labels
  Transform trans;
  glm::vec2 offset{ -0.3f, -0.75f };
  trans.setScale({ 0.008f, 0.008f, 0.008f });
  //trans.setRotation(-PI / 2, -PI / 2, 0.0f);

  trans.setTranslation({ m_hexes[99].x() + offset.x, m_pipeline.get().GetWaterHeight() + 0.2f, m_hexes[99].z() + offset.y });
  trans.billboard(-m_camera.get().getDirection());
  m_base_labels[0]->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();

  trans.setTranslation({ m_hexes[38].x() + offset.x, m_pipeline.get().GetWaterHeight() + 0.2f, m_hexes[38].z() + offset.y });
  trans.billboard(-m_camera.get().getDirection());
  m_base_labels[1]->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();

  trans.setTranslation({ m_hexes[83].x() + offset.x, m_pipeline.get().GetWaterHeight() + 0.2f, m_hexes[83].z() + offset.y });
  trans.billboard(-m_camera.get().getDirection());
  m_base_labels[2]->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();

  trans.setTranslation({ m_hexes[141].x() + offset.x, m_pipeline.get().GetWaterHeight() + 0.2f, m_hexes[141].z() + offset.y });
  trans.billboard(-m_camera.get().getDirection());
  m_base_labels[3]->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();

  //update debug
  for (int i = 0; i < m_texture_scales.size(); ++i)
  {
    m_pipeline.get().SetUniformData("class ePhongRender",
      "textureScale[" + std::to_string(i) + "]",
      m_texture_scales[i]);
  }

  m_choice_circle->SetColor({ m_hdr_strangth, m_hdr_strangth, 0.f, 1.f});

  //update texts debug
  for (int i = 0; i < texts.size() ; ++i)
  {
    Transform trans;
    glm::vec2 offset{ -0.2f, -0.25f };
    trans.setTranslation({ m_hexes[i].x() + offset.x, Hex::common_height + 0.01f, m_hexes[i].z() + offset.y });
    trans.setScale({ 0.010f, 0.010f, 0.010f });
    trans.setRotation(-PI / 2, -PI / 2, 0.0f);
    texts[i]->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();
  }

  // update heights debug
  static float height_scale = m_height_scale;
  static float max_height = m_max_height;
  if (height_scale != m_height_scale || max_height != m_max_height)
  {
    if (auto* terrainModel = dynamic_cast<TerrainModel*>(m_terrain->GetModel()))
    {
      terrainModel->Initialize(m_texManager->Find("Tgrass0_d"),
                               m_texManager->Find("Tgrass0_d"),
                               &Texture::GetTexture1x1(BLUE),
                               m_texManager->Find("TOcean0_s"),
                               true,
                               m_height_scale,
                               m_max_height);
      height_scale = m_height_scale;
      max_height = m_max_height;
    }
  }
  static float water_level = m_water_level;
  if (water_level != m_water_level)
  {
    std::set<TerrainType> terrain_types;
    terrain_types.insert({ "mounten",		0.0f, m_water_level, {0.5f, 0.5f, 0.0f } });
    terrain_types.insert({ "grass",		m_water_level,	1.0f, {0.0f, 1.0f, 0.0f} });
    terrain_types.insert({ "grass",		1.0f,	1.0f, {0.0f, 1.0f, 0.0f} });

    int counter = 0;
    for (const auto& type : terrain_types)
    {
      m_pipeline.get().SetUniformData("class ePhongRender",
        "base_start_heights[" + std::to_string(counter) + "]",
        type.threshold_start);

      m_pipeline.get().SetUniformData("class ePhongRender",
        "textureScale[" + std::to_string(counter) + "]",
        m_texture_scales[counter]);
      ++counter;
    }
    m_pipeline.get().SetUniformData("class ePhongRender",
      "base_start_heights[" + std::to_string(counter) + "]",
      1.0f);
    water_level = m_water_level;
  }
}

//---------------------------------------------------------------
void GameController::OnObjectPickedWithLeft(std::shared_ptr<eObject> _picked)
{
  if (!m_current_path.empty())
    return;

  if (!m_dice_rolled)
  {
    if (m_game_state == GameState::SPANISH_TO_MOVE)
    {
      unsigned int index_picked = -1;
      for (unsigned int i = 0; i < m_ships.size(); ++i)
      {
        if (m_ships[i] == nullptr)
          continue;
        if (m_ships[i]->GetScriptObject().lock().get() == _picked.get())
        {
          index_picked = i;
          m_ship_icons[index_picked]->SetRenderingFunc(GUI::RenderFunc::Default);
          m_focused_index = index_picked;
        }
        else
          m_ship_icons[i]->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
      }
      if(index_picked != -1)
        m_game->SetFocused(_picked);
    }
    else if (m_game_state == GameState::PIRATE_TO_MOVE)
    {
      unsigned int index_picked = -1;
      for (unsigned int i = 0; i < m_ships_pirate.size(); ++i)
      {
        if (m_ships_pirate[i] == nullptr)
          continue;
        if (m_ships_pirate[i]->GetScriptObject().lock().get() == _picked.get())
        {
          index_picked = i;
          m_ship_icons_pirate[index_picked]->SetRenderingFunc(GUI::RenderFunc::Default);
          m_focused_index = index_picked;
        }
        else
          m_ship_icons_pirate[i]->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
      }
      if (index_picked != -1)
        m_game->SetFocused(_picked);
    }
  }
  else
  {
    _UpdateWarrning("You should move current ship before choosing next one");
  }
}

//------------------------------------------------------------
void GameController::OnObjectPickedWithRight(std::shared_ptr<eObject> _picked)
{
  if (_picked)
  {
    if (size_t index = _GetPirateShipIndex(_picked); m_game_state == GameState::SPANISH_TO_MOVE && index < m_ships_pirate.size())
    {
      for (int i = 0; i < m_ships.size(); ++i)
      {
        if (m_ships[i] == nullptr)
          continue;

        if (m_ships[i]->GetScriptObject().lock().get() == m_game->GetFocusedObject().get()
            && m_has_moved[i] == true)
        {
          float distance = glm::length(m_ships[i]->GetScriptObject().lock()->GetTransform()->getTranslation() -
            _picked->GetTransform()->getTranslation());
          if (dbb::round(distance, 2) <= Hex::radius * 2.0f)
          {
            m_target = _picked.get();
            //calculate turn to point
            glm::vec3 direction_shooter_to_targrt = glm::normalize(m_target->GetTransform()->getTranslation() - m_ships[i]->GetScriptObject().lock()->GetTransform()->getTranslation());
            glm::vec3 direction_turened = glm::cross(direction_shooter_to_targrt, YAXIS);
            m_ships[i]->GetScriptObject().lock()->GetTransform()->turnTo(m_ships[i]->GetScriptObject().lock()->GetTransform()->getTranslation() + direction_turened, 0.0f);
            OnShoot(m_ships[i]->GetScriptObject().lock().get());
            if (int32_t outcome = math::Random::RandomInt(1, 6); outcome <= 3)
            {
              OnGetHit(_picked.get());
              m_has_moved_pirate[index] = true;
            }
          }
        }
      }
    }
    else if (size_t index = _GetSpanishShipIndex(_picked); m_game_state == GameState::PIRATE_TO_MOVE && index < m_ships.size())
    {
      for (int i = 0; i < m_ships_pirate.size(); ++i)
      {
        if (m_ships_pirate[i]->GetScriptObject().lock().get() == m_game->GetFocusedObject().get()
          && m_has_moved_pirate[i] == true)
        {
          float distance = glm::length(m_ships_pirate[i]->GetScriptObject().lock()->GetTransform()->getTranslation() -
                                      _picked->GetTransform()->getTranslation());
          if (dbb::round(distance, 2) <= Hex::radius * 1.0f) // one hex
          {
            m_target = _picked.get();
            glm::vec3 direction_shooter_to_targrt = glm::normalize(m_target->GetTransform()->getTranslation() - m_ships_pirate[i]->GetScriptObject().lock()->GetTransform()->getTranslation());
            glm::vec3 direction_turened = glm::cross(direction_shooter_to_targrt, YAXIS);
            m_ships_pirate[i]->GetScriptObject().lock()->GetTransform()->turnTo(m_ships_pirate[i]->GetScriptObject().lock()->GetTransform()->getTranslation() + direction_turened, 0.0f);
            OnShoot(m_ships_pirate[i]->GetScriptObject().lock().get());
            if (int32_t outcome = math::Random::RandomInt(1, 6); outcome <= 2)
            {
              OnGetHit(_picked.get());
              m_has_moved[index] = true;
            }
          }
        }
      }
    }
  }
}

//------------------------------------------------------------
void GameController::OnFrameMoved(std::shared_ptr<GUI> _frame)
{
  auto x = _frame->getTopLeft().x + ((_frame->getBottomRight().x - _frame->getTopLeft().x) / 2);
  auto y = _frame->getTopLeft().y + ((_frame->getBottomRight().y - _frame->getTopLeft().y) / 2);
  glm::vec2 offset{ 10,7 };
  for (unsigned int i = 0; i < m_ship_icons.size(); ++i)
  {
    if (m_ship_icons[i]->isHover(x,y))
    {
      if (m_has_gold_index == -1)
        m_has_gold_index = i;
      _frame->Move({ m_ship_icons[m_has_gold_index]->getTopLeft().x - offset.x, m_ship_icons[m_has_gold_index]->getTopLeft().y - offset.y });
      return;
    }
  }
  if (m_has_gold_index != -1)
    _frame->Move({ m_ship_icons[m_has_gold_index]->getTopLeft().x - offset.x, m_ship_icons[m_has_gold_index]->getTopLeft().y - offset.y });
  else
    _frame->Move({ 50 * 5, 5 });
  return;

  //@todo add this later ?
  if (auto hovered = m_game->GetHoveredObject(); hovered)
  {
    for (unsigned int i = 0; i < m_ships.size(); ++i)
    {
      if (hovered->GetScript() == m_ships[i] && m_ships[i] != nullptr)
      {
        m_has_gold_index = i;
        glm::vec2 offset{ 10,7 };
        _frame->Move({ m_ship_icons[i]->getTopLeft().x - offset.x, m_ship_icons[i]->getTopLeft().y - offset.y });
        return;
      }
    }
  }
}

//------------------------------------------------------------
void GameController::OnShipCameToBase(std::shared_ptr<eObject> _ship, const std::string& _base_name)
{
  _ship->SetVisible(false);
  if (m_ships[0] != _ship->GetScript())
    m_game->SetFocused(m_ships[0]->GetScriptObject().lock());
  else
    m_game->SetFocused(m_ships[1]->GetScriptObject().lock()); //@todo check is visible or is alive

  if(_ship->GetScript() == m_ships[m_has_gold_index] && (_base_name == "Base Casablanca" || _base_name == "Base Sevillia"))
    _UpdateWarrning("Game Over! Spanish Won!");
}

//------------------------------------------------------------
void GameController::OnShoot(const eObject* _shooter)
{
  glm::vec4 modelCenterShooter = _shooter->GetTransform()->getModelMatrix() * glm::vec4(_shooter->GetCollider()->GetCenter(), 1.0f);
  glm::vec4 modelCenterTarget = m_target->GetTransform()->getModelMatrix() * glm::vec4(m_target->GetCollider()->GetCenter(), 1.0f);
  glm::vec4 direction = modelCenterTarget - modelCenterShooter;
  std::shared_ptr<IParticleSystem> system = std::make_shared<ParticleSystem>(10.0f, 0.1f, 0.0f, 10'000.f,
    modelCenterShooter + glm::normalize(direction) * Hex::radius / 2.0f,
    m_texManager->Find("Tatlas1"),
    m_soundManager->GetSound("shot_sound"),
    4,
    2'500.f);

  //@todo no connection to rendering. Add pt sys in different way
  system->Loop() = false;
  system->ConeAngle() = 0.3f;
  system->Speed() = 0.003f;
  system->LifeLength() = 1.5f;
  system->Scale() = { 0.04f,0.04f, 0.04f };
  system->SystemType() = IParticleSystem::SPHERE;
  system->BaseRadius() = 0.0f;
  system->ParticlesPerSecond() = 20;
  system->RandomizeMagnitude() = 0.7f;
  system->SetSizeBezier({glm::vec3{ -0.88, 0.72, 0 }, glm::vec3{-0.51, 0.72, 0}, glm::vec3{-0.2, 0.58,0 }, glm::vec3{0.1, -0.69, 0}});
  system->Start();
  m_pipeline.get().AddParticleSystem(system);
}

//------------------------------------------------------------
void GameController::OnGetHit(const eObject* _target)
{
  std::shared_ptr<IParticleSystem> system = std::make_shared<ParticleSystem>(40.0f, 0.1f, 0.0f, 4'000.0f,
    _target->GetTransform()->getTranslation(),
    m_texManager->Find("Tatlas2"),
    m_soundManager->GetSound("fire_sound"),
    4,
    7'000.f);

  //@todo no connection to rendering. Add pt sys in different way
  system->Loop() = false;
  system->ConeAngle() = 0.3f;
  system->Speed() = 0.01f;
  system->LifeLength() = 1.0f;
  system->BaseRadius() = 0.1f;
  system->Scale() = { 0.055f,0.055f, 0.055f };
  system->Start();
  m_pipeline.get().AddParticleSystem(system);

  if (auto* target_script = _GetShipScript(_target); target_script)
    target_script->SetDrowned(true);

  if(_target->GetScript() == m_ships[m_has_gold_index])
    _UpdateWarrning("Game Over! Pirate Won!");
}

//------------------------------------------------------------
void GameController::_OnShipDeleted(shObject _deleted)
{
  for (int i = 0; i < m_ships.size(); ++i)
  {
    if (m_ships[i] == _deleted->GetScript())
      m_ships[i] = nullptr;
  }

  for (int i = 0; i < m_ships_pirate.size(); ++i)
  {
    if (m_ships_pirate[i] == _deleted->GetScript())
      m_ships_pirate[i] = nullptr;
  }
}

//----------------------------------------------------------------------------------
void GameController::_OnConnectionEstablished(const dbb::TCPConnection& _connection)
{
  _connection.IntArrayMessageRecieved.Subscribe([this](const std::vector<uint32_t>& _content, const std::string& _endpoint){ _OnTCPMessageRecieved(_content); });
}

//-------------------------------------------------------------------------------
void GameController::_OnTCPMessageRecieved(const std::vector<uint32_t> _content)
{
  std::cout << "_OnTCPMessageRecieved" << std::endl;
  if(_content[0] == (uint32_t)MessageType::MOVE)
  {
    glm::vec3 destination = { m_current_path.front()->x(), m_ship_height_level, m_current_path.front()->z() };
    m_focused_index = _content[1];
    m_current_path.clear();
    for (int i = 2; i < _content.size(); ++i)
      m_current_path.push_back(&m_hexes[_content[i]]);

    if (m_game_state == GameState::SPANISH_TO_MOVE && m_side == Side::PIRATE)
      m_ships[m_focused_index]->SetDestination(destination);
    else if (m_game_state == GameState::PIRATE_TO_MOVE && m_side == Side::SPANISH)
      m_ships_pirate[m_focused_index]->SetDestination(destination);
    else
    {
      m_current_path.clear();
      return;
    }

    _UpdatePathVisual();
    m_current_path.back()->SetTaken(true);
    auto cur_hex = std::find_if(m_hexes.begin(), m_hexes.end(), [this](const Hex& hex)
      {
        return glm::length(m_game->GetFocusedObject()->GetTransform()->getTranslation() - glm::vec3{ hex.x(), m_pipeline.get().GetWaterHeight(), hex.z() }) < (0.5f * 0.57f); //radius
      });
    cur_hex->SetTaken(false);
    _UpdateWarrning("");
  }
}

//------------------------------------------------------------
void GameController::_InitializeDiceLogicAndVisual()
{
  float pos_x = 10.0f;
  float pos_y = m_game->Height() - 125.0f - 10.0f;
  glm::vec2 icon_size = { 125.0f , 125.0f };
  m_dice_gui = std::make_shared<GUI>(pos_x, pos_y, (float)icon_size.x, (float)icon_size.y, m_game->Width(), m_game->Height());
  auto dice_tex = _GetDiceTexture();
  m_dice_gui->SetTexture(*dice_tex, { 0,0 }, { dice_tex->mTextureWidth, dice_tex->mTextureHeight });
  m_dice_gui->setCommand(std::make_shared<GUICommand>([this]()
    {
      if (!m_dice_rolled && m_game->GetFocusedObject() && !_CurrentShipHasMoved())
      {
        m_current_dice = math::Random::RandomInt(1, 6);
        m_dice_rolled = true;
        auto dice_tex = _GetDiceTexture();
        m_dice_gui->SetTexture(*dice_tex, { 0,0 }, { dice_tex->mTextureWidth, dice_tex->mTextureHeight });
      }
    }));
  m_game->AddGUI(m_dice_gui);
  m_game->AddInputObserver(m_dice_gui.get(), MONOPOLY);
}

//------------------------------------------------------------
void GameController::_InitializeShipIcons()
{
  // spanish ship icons
  glm::vec2 icon_size = { 50.0f , 50.0f };
  for (int i = 0; i < m_ship_quantity; ++i)
  {
    float pos_x = (10.0f + icon_size.x) * i;
    float pos_y = 10.0f;
    const Texture* ship_tex = m_game->GetTexture("ship1");
    std::shared_ptr<GUI> ship_gui = std::make_shared<GUI>(pos_x, pos_y, icon_size.x, icon_size.y, m_game->Width(), m_game->Height());
    ship_gui->SetTexture(*ship_tex, { 0,0 }, { ship_tex->mTextureWidth, ship_tex->mTextureHeight });
    ship_gui->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
    ship_gui->setCommand(std::make_shared<GUICommand>([this, ship_gui]()
    {
        for (int i = 0; i < m_ship_icons.size(); i++)
        {
          if (m_ship_icons[i]->isPressed((int)m_cursor_pos.x, (int)m_cursor_pos.y))
          {
            if (!m_dice_rolled && m_game_state == GameState::SPANISH_TO_MOVE)
            {
              m_game->SetFocused(m_ships[i]->GetScriptObject().lock());
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

  //pirate ship icons
  for (int i = 0; i < m_ship_quantity_pirate; ++i)
  {
    float pos_x = m_game->Width() - ((icon_size.x + 10.0f) * (i+1));
    float pos_y = 10.0f;
    const Texture* ship_tex = m_game->GetTexture("ship1");
    std::shared_ptr<GUI> ship_gui = std::make_shared<GUI>(pos_x, pos_y, icon_size.x, icon_size.y, m_game->Width(), m_game->Height());
    ship_gui->SetTexture(*ship_tex, { 0,0 }, { ship_tex->mTextureWidth, ship_tex->mTextureHeight });
    ship_gui->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
    ship_gui->setCommand(std::make_shared<GUICommand>([this, ship_gui]()
      {
        for (int i = 0; i < m_ship_icons_pirate.size(); i++)
        {
          if (m_ship_icons_pirate[i]->isPressed((int)m_cursor_pos.x, (int)m_cursor_pos.y))
          {
            if (!m_dice_rolled && m_game_state == GameState::PIRATE_TO_MOVE)
            {
              m_game->SetFocused(m_ships_pirate[i]->GetScriptObject().lock());
              m_ship_icons_pirate[i]->SetRenderingFunc(GUI::RenderFunc::Default);
              m_focused_index = i;
            }
          }
          else
          {
            m_ship_icons_pirate[i]->SetRenderingFunc(GUI::RenderFunc::GreyKernel);
          }
        }
      }));
    m_game->AddGUI(ship_gui);
    m_game->AddInputObserver(ship_gui.get(), MONOPOLY);
    m_ship_icons_pirate.push_back(ship_gui);
  }

  //icons move status
  icon_size = { 20.0f , 20.0f };
  for (int i = 0; i < m_ship_quantity; ++i)
  {
    float pos_x = 35.0f + 60.0f * i;
    float pos_y = 50.0f;
    std::shared_ptr<GUI> status_gui = std::make_shared<GUIWithAlpha>(pos_x, pos_y, icon_size.x, icon_size.y, m_game->Width(), m_game->Height());
    status_gui->SetRenderingFunc(GUI::RenderFunc::Default);
    status_gui->SetTransparent(true);
    m_game->AddGUI(status_gui);
    m_status_icons.push_back(status_gui);
  }
  // icons move status pirate
  for (int i = 0; i < m_ship_quantity_pirate; ++i)
  {
    float pos_x = m_game->Width() - 35.0f - (60.0f * i);
    float pos_y = 50.0f;
    std::shared_ptr<GUI> status_gui = std::make_shared<GUIWithAlpha>(pos_x, pos_y, icon_size.x, icon_size.y, m_game->Width(), m_game->Height());
    status_gui->SetRenderingFunc(GUI::RenderFunc::Default);
    status_gui->SetTransparent(true);
    m_game->AddGUI(status_gui);
    m_status_icons_pirate.push_back(status_gui);
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
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(new SimpleGeometryMesh(dots, Hex::radius * 0.57f, SimpleGeometryMesh::GeometryType::Hex, {1.0f, 1.0f, 0.0f})),
                                                                                              eObject::RenderType::GEOMETRY));
}

//------------------------------------------------------------
void GameController::_InitializeShips()
{
  //TERRAIN
  std::unique_ptr<TerrainModel> terrainModel = m_modelManager->CloneTerrain("simple");
  terrainModel->Initialize(m_texManager->Find("Tgrass0_d"),
                           m_texManager->Find("Tgrass0_d"),
                           &Texture::GetTexture1x1(BLUE),
                           m_texManager->Find("TOcean0_s"),
                           true,
                           m_height_scale,
                           m_max_height);
  terrainModel->setAlbedoTextureArray(m_game->GetTexture("terrain_albedo_array_2"));
  terrainModel->getMeshes()[0]->SwitchLOD(2);

  //OBJECTS
  ObjectFactoryBase factory;

  m_terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()), eObject::RenderType::PHONG, "Terrain");
  m_terrain->SetName("Terrain");
  m_terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
  m_terrain->GetTransform()->setTranslation(vec3(0.0f, 1.85f, 0.0f));
  m_terrain->SetTextureBlending(true);
  m_terrain->SetPickable(false);
  m_game->AddObject(m_terrain);

  //SET UNIFORMS
  m_pipeline.get().SetUniformData("class ePhongRender", "min_height", 0.0f);
  m_pipeline.get().SetUniformData("class ePhongRender", "max_height", 1.0f);
  m_pipeline.get().SetUniformData("class ePhongRender", "color_count", 2);

  std::set<TerrainType> terrain_types;
  terrain_types.insert({ "mounten",		0.0f, m_water_level, {0.5f, 0.5f, 0.0f } });
  terrain_types.insert({ "grass",		m_water_level,	1.0f, {0.0f, 1.0f, 0.0f} });
  terrain_types.insert({ "grass",		1.0f,	1.0f, {0.0f, 1.0f, 0.0f} });

  int counter = 0;
  for (const auto& type : terrain_types)
  {
    m_pipeline.get().SetUniformData("class ePhongRender",
      "base_start_heights[" + std::to_string(counter) + "]",
      type.threshold_start);

    m_pipeline.get().SetUniformData("class ePhongRender",
      "textureScale[" + std::to_string(counter) + "]",
      m_texture_scales[counter]);
    ++counter;
  }
  m_pipeline.get().SetUniformData("class ePhongRender",
    "base_start_heights[" + std::to_string(counter) + "]",
    1.0f);

  //SHIPS
  Material material;
  material.albedo = glm::vec3(0.8f, 0.0f, 0.0f);
  material.ao = 1.0f;
  material.roughness = 0.5;
  material.metallic = 0.5;

  //@todo color change all objects that share the model, can not change colors for every ship for now
  std::vector<glm::vec3> colors{ glm::vec3(0.9f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 0.9f, 0.0f),
                                 glm::vec3(0.0f, 0.0f, 0.9f),
                                 glm::vec3(0.9f, 0.9f, 0.0f)};

  for (int i = 0; i < m_ship_quantity; ++i)
  {
    //material.albedo = colors[i];
    shObject ship = factory.CreateObject(m_modelManager->Find("ship"), eObject::RenderType::PBR, "Ship" + std::to_string(i));
    //shObject ship = factory.CreateObject(m_modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "Ship" + std::to_string(i));
    ship->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
    ship->GetTransform()->setTranslation(vec3(m_hexes[55 + i*40].x(), m_ship_height_level, m_hexes[55 + i * 40].z()));
    ship->GetTransform()->setUp(glm::vec3(0.0f, 0.0f, 1.0f));
    ship->GetTransform()->setForward(glm::vec3(-1.0f, 0.0f, 0.0f));
    ship->GetTransform()->turnTo(vec3{ m_hexes[110].x(), m_ship_height_level, m_hexes[110].z() }, 0.05f);

    m_hexes[55 + i * 40].SetTaken(true);

    for (auto& mesh : ship->GetModel()->Get3DMeshes())
      const_cast<I3DMesh*>(mesh)->SetMaterial(material); //@todo

    auto script = new eShipScript(m_game,
                                  m_texManager->Find("TSpanishFlag0_s"),
                                  m_camera,
                                  m_texManager->Find("Tatlas2"),
                                  m_soundManager->GetSound("shot_sound"),
                                  m_pipeline.get().GetWaterHeight());
    ship->SetScript(script);
    ship->SetInstancingTag("spanish_ship");
    m_ships.push_back(script);

    m_game->AddInputObserver(ship->GetScript(), WEAK);
    m_game->AddObject(ship);
  }

  for (int i = 0; i < m_ship_quantity_pirate; ++i)
  {
    shObject ship = factory.CreateObject(m_modelManager->Find("pirate_ship"), eObject::RenderType::PBR, "Pirate" + std::to_string(i));
    ship->GetTransform()->setScale(vec3(0.04f, 0.04f, 0.04f));
    ship->GetTransform()->setTranslation(vec3(m_hexes[26 + i * 40].x(), m_ship_height_level, m_hexes[26 + i * 40].z()));
    ship->GetTransform()->setUp(glm::vec3(0.0f, 0.0f, 1.0f));
    ship->GetTransform()->setRotation(0.0f, PI/2, 0.0f);
    ship->GetTransform()->setForward(glm::vec3(1.0f, 0.0f, 0.0f));

    ship->SetInstancingTag("pirate_ship");
    
    m_hexes[26 + i * 40].SetTaken(true);

    if(i == 0)
    {
      for (auto& mesh : ship->GetModel()->Get3DMeshes())
      {
        Texture t;
        if (auto& textures = mesh->GetTextures(); !textures.empty())
        {
          if (textures[0].m_path.find("barrel_texture1k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/PirateShip/barrel_texture1k/DefaultMaterial_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;

            t.loadTextureFromFile("../game_assets/Resources/PirateShip/barrel_texture1k/DefaultMaterial_Normal_OpenGL_1001.png");
            t.type = "texture_normal";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_normal = true;
          }
          else if (textures[0].m_path.find("cannon_texture1k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/PirateShip/cannon_texture1k/DefaultMaterial_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;
          }
          else if (textures[0].m_path.find("ship_texture4k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/PirateShip/ship_texture4k/ship_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;

            t.loadTextureFromFile("../game_assets/Resources/PirateShip/ship_texture4k/ship_Normal_OpenGL_1001.png");
            t.type = "texture_normal";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_normal = true;
          }
          else if (textures[0].m_path.find("skull_texture1k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/PirateShip/skull_texture1k/Skull_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;

            t.loadTextureFromFile("../game_assets/Resources/PirateShip/skull_texture1k/Skull_Normal_OpenGL_1001.png");
            t.type = "texture_normal";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_normal = true;
          }
        }
      }
    }

    auto script = new eShipScript(m_game,
                                  m_texManager->Find("TPirate_flag0_s"),
                                  m_camera,
                                  m_texManager->Find("Tatlas2"),
                                  m_soundManager->GetSound("shot_sound"),
                                  m_pipeline.get().GetWaterHeight());
    ship->SetScript(script);
    ship->SetInstancingTag("pirate_ship");
    m_ships_pirate.push_back(script);

    m_game->AddInputObserver(ship->GetScript(), WEAK);
    m_game->AddObject(ship);
  }
}

//------------------------------------------------------------
void GameController::_InitializeBases()
{
  ObjectFactoryBase factory; //@todo make factory for bases
  //1
  float scale = 0.02f;
  shObject base1 = factory.CreateObject(m_modelManager->Find("castle"), eObject::RenderType::PHONG, "Base Veracruz");
  base1->GetTransform()->setScale(vec3(scale, scale, scale));
  base1->GetTransform()->setTranslation(vec3(m_hexes[99].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[99].z()));
  m_hexes[99].SetBase(true);
  auto* base_script1 = new eBaseScript(m_game, m_texManager->Find("TSpanishFlag0_s"));
  base_script1->ObjectCameToBase.Subscribe(
    [this](std::shared_ptr<eObject> _object, const std::string& _base_name) {OnShipCameToBase(_object, _base_name);});
  base1->SetScript(base_script1);
  m_game->AddObject(base1);
  
  m_base_labels.push_back(std::make_shared<Text>());
  m_base_labels[0]->content = "Veracruz";
  m_base_labels[0]->font = "edwardian";
  m_base_labels[0]->pos_x = 0.0f;
  m_base_labels[0]->pos_y = 0.0f;
  m_base_labels[0]->scale = 1.0f;
  m_base_labels[0]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_game->AddText(m_base_labels[0]);

  //load textures manualy
  Texture t;
  for (auto& mesh : base1->GetModel()->Get3DMeshes())
  {
    t.loadTextureFromFile("../game_assets/assets/brickwall.jpg");
    t.type = "texture_diffuse";
    const_cast<I3DMesh*>(mesh)->AddTexture(&t);
    t.loadTextureFromFile("../game_assets/assets/brickwall_normal.jpg");
    t.type = "texture_normal";
    const_cast<I3DMesh*>(mesh)->AddTexture(&t);
    mesh->GetMaterial()->use_normal = true;
    /*const_cast<I3DMesh*>(mesh)->calculatedTangent();
    const_cast<I3DMesh*>(mesh)->ReloadVertexBuffer();*/
  }

  //2
  shObject base2 = factory.CreateObject(m_modelManager->Find("castle"), eObject::RenderType::PHONG, "Base Cartagena");
  base2->GetTransform()->setScale(vec3(scale, scale, scale));
  base2->GetTransform()->setTranslation(vec3(m_hexes[38].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[38].z()));
  m_hexes[38].SetBase(true);
  auto* base_script2 = new eBaseScript(m_game, m_texManager->Find("TSpanishFlag0_s"));
  base_script2->ObjectCameToBase.Subscribe(
    [this](std::shared_ptr<eObject> _object, const std::string& _base_name) {OnShipCameToBase(_object, _base_name); });
  base2->SetScript(base_script2);
  m_game->AddObject(base2);

  m_base_labels.push_back(std::make_shared<Text>());
  m_base_labels[1]->content = "Cartagena";
  m_base_labels[1]->font = "edwardian";
  m_base_labels[1]->pos_x = 0.0f;
  m_base_labels[1]->pos_y = 0.0f;
  m_base_labels[1]->scale = 1.0f;
  m_base_labels[1]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_game->AddText(m_base_labels[1]);

  //3
  shObject base3 = factory.CreateObject(m_modelManager->Find("castle"), eObject::RenderType::PHONG, "Base Casablanca");
  base3->GetTransform()->setScale(vec3(scale, scale, scale));
  base3->GetTransform()->setTranslation(vec3(m_hexes[83].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[83].z()));
  m_hexes[83].SetBase(true);
  auto* base_script3 = new eBaseScript(m_game, m_texManager->Find("TSpanishFlag0_s"));
  base_script3->ObjectCameToBase.Subscribe(
    [this](std::shared_ptr<eObject> _object, const std::string& _base_name) {OnShipCameToBase(_object, _base_name); });
  base3->SetScript(base_script3);
  m_game->AddObject(base3);

  m_base_labels.push_back(std::make_shared<Text>());
  m_base_labels[2]->content = "Casablanca";
  m_base_labels[2]->font = "edwardian";
  m_base_labels[2]->pos_x = 0.0f;
  m_base_labels[2]->pos_y = 0.0f;
  m_base_labels[2]->scale = 1.0f;
  m_base_labels[2]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_game->AddText(m_base_labels[2]);

  //4
  shObject base4 = factory.CreateObject(m_modelManager->Find("castle"), eObject::RenderType::PHONG, "Base Sevillia");
  base4->GetTransform()->setScale(vec3(scale, scale, scale));
  base4->GetTransform()->setTranslation(vec3(m_hexes[141].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[141].z()));
  m_hexes[141].SetBase(true);
  auto* base_script4 = new eBaseScript(m_game, m_texManager->Find("TSpanishFlag0_s"));
  base_script4->ObjectCameToBase.Subscribe(
    [this](std::shared_ptr<eObject> _object, const std::string& _base_name) {OnShipCameToBase(_object, _base_name); });
  base4->SetScript(base_script4);
  m_game->AddObject(base4);

  m_base_labels.push_back(std::make_shared<Text>());
  m_base_labels[3]->content = "Sevillia";
  m_base_labels[3]->font = "edwardian";
  m_base_labels[3]->pos_x = 0.0f;
  m_base_labels[3]->pos_y = 0.0f;
  m_base_labels[3]->scale = 1.0f;
  m_base_labels[3]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_game->AddText(m_base_labels[3]);
}

//------------------------------------------------------------
void GameController::_InitializeGoldenFrame()
{
  float pos_x = 50 * 5;
  float pos_y = 5.0f;
  const Texture* tex = m_game->GetTexture("golden_frame");
  std::shared_ptr<GUI> frame_gui = std::make_shared<GUI>(pos_x, pos_y, 70, 70, m_game->Width(), m_game->Height());
  frame_gui->SetTexture(*tex, { 0,0 }, { tex->mTextureWidth, tex->mTextureHeight });
  frame_gui->SetTakeMouseEvents(true);
  frame_gui->SetMovable2D(true);
  frame_gui->SetExecuteOnRelease(true);
  frame_gui->SetTransparent(true);
  frame_gui->setCommand(std::make_shared<GUICommand>([this, frame_gui]()
   {
     this->OnFrameMoved(frame_gui);
   }));
  m_game->AddGUI(frame_gui);
  m_game->AddInputObserver(frame_gui.get(), ALWAYS);
}

//------------------------------------------------------------
void GameController::_InstallTcpServer()
{
  if (!m_tcpAgent)
  {
    if (dbb::NetWork::Initialize())
    {
      m_tcpAgent = std::make_unique<Server>();
      m_tcpAgent->ConnectionEstablished.Subscribe([this](const dbb::TCPConnection& _connection) {_OnConnectionEstablished(_connection); });
      if (m_tcpAgent->Initialize(dbb::IPEndPoint{ "0.0.0.0", 8080 }))//134.238.94.205 //208.67.222.222
      {
        m_tcpTimer.reset(new math::Timer([this]()->bool
          {
            if (m_tcpAgent->IsConnected())
              m_tcpAgent->Frame();
            return true;
          }));
        m_tcpTimer->start(15); //~70 fps
      }
    }
  }
}

//------------------------------------------------------------
void GameController::_InstallTcpClient()
{
  if (!m_tcpAgent)
  {
    if (dbb::NetWork::Initialize())
    {
      m_tcpAgent = std::make_unique<Client>();
      m_tcpAgent->ConnectionEstablished.Subscribe([this](const dbb::TCPConnection _connection) {_OnConnectionEstablished(_connection); });
      if (m_tcpAgent->Initialize(dbb::IPEndPoint{ "127.0.0.1", 8080 })) //"109.95.50.27 // "192.168.2.102 //134.238.94.205 /208.67.222.222
      {
        m_tcpTimer.reset(new math::Timer([this]()->bool
          {
            if (m_tcpAgent->IsConnected())
              m_tcpAgent->Frame();
            return true;
          }));
        m_tcpTimer->start(15); //~70 fps
      }
    }
  }
}

//------------------------------------------------------------
bool GameController::_CurrentShipHasMoved() const
{
  if (m_game_state == GameState::SPANISH_TO_MOVE)
  {
    for (int i = 0; i < m_ships.size(); ++i)
    {
      if (m_ships[i] == nullptr)
        continue;
      if (m_ships[i]->GetScriptObject().lock().get() == m_game->GetFocusedObject().get()
        && m_has_moved[i] == true)
        return true;
    }
  }
  else if (m_game_state == GameState::PIRATE_TO_MOVE)
  {
    for (int i = 0; i < m_ships_pirate.size(); ++i)
    {
      if (m_ships_pirate[i] == nullptr)
        continue;
      if (m_ships_pirate[i]->GetScriptObject().lock().get() == m_game->GetFocusedObject().get()
          && m_has_moved_pirate[i] == true)
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

//-------------------------------------------------------------
void GameController::_UpdateLight(float _tick)
{
  static float radians_rotation = 0.0f;
  radians_rotation += (_tick / 1'000.f);
  glm::vec3 rotation_axis = { glm::cos(glm::radians(45.0f)), 0, glm::sin(glm::radians(45.0f)) };
  glm::mat4 rotation_matrix = glm::rotate(UNIT_MATRIX, glm::radians(radians_rotation), rotation_axis);
  m_game->GetMainLight().light_position = rotation_matrix * glm::vec4{0.f, 1.f, 0.f, 1.f} ;
  m_game->GetMainLight().light_position += glm::vec4{ 0.f, 3.f, 0.f, 1.f };
}

//-------------------------------------------------------------
void GameController::_UpdatePathVisual()
{
  std::vector<glm::vec3>	verices;
  std::vector<GLuint>			indices;
  glm::vec3 pos;
  if(m_game_state == GameState::SPANISH_TO_MOVE)
    pos = m_ships[m_focused_index]->GetScriptObject().lock()->GetTransform()->getTranslation();
  else if(m_game_state == GameState::PIRATE_TO_MOVE)
    pos = m_ships_pirate[m_focused_index]->GetScriptObject().lock()->GetTransform()->getTranslation();

  pos.y += 0.01f;
  verices.push_back(pos);
  for (auto& hex : m_current_path)
    verices.push_back({ hex->x(), Hex::common_height + 0.01f, hex->z() });

  // last should be half long
  glm::vec3 last = (verices[verices.size() - 1] - verices[verices.size() - 2]) / 2.0f;
  verices[verices.size() - 1] = verices[verices.size() - 2] + last;

  for (unsigned int i = 0; i < verices.size(); ++i)
  {
    indices.push_back(i);
    if(i!=0)
      indices.push_back(i);
  }
  m_path_mesh->UpdateData(verices, indices, { 1.0f, 1.0f ,1.0f, 1.0f });

  //text destination
  m_destination_text->content = std::to_string(m_current_path.size());
}

//-------------------------------------------------------------
void GameController::_UpdateWarrning(const std::string& _message)
{
  m_warrining->content = _message;
}

//-------------------------------------------------------------
void GameController::_InitializeSounds()
{
  m_seagull_sound = m_soundManager->GetSound("seagull_sound");
  m_seagull_sound->Play();
}

//-------------------------------------------------------------
void GameController::_SetDestinationFromCurrentPath()
{
  if (!m_current_path.empty())
  {
    glm::vec3 destination = { m_current_path.front()->x(), m_ship_height_level, m_current_path.front()->z() };
    m_current_path.pop_front();

    if (m_game_state == GameState::SPANISH_TO_MOVE && m_side != Side::PIRATE)
      m_ships[m_focused_index]->SetDestination(destination);
    else if (m_game_state == GameState::PIRATE_TO_MOVE && m_side != Side::SPANISH)
      m_ships_pirate[m_focused_index]->SetDestination(destination);
    else
    {
      m_current_path.clear();
    }
    _SendMoveMsg();
  }
}

//-------------------------------------------------------------
std::vector<uint32_t> GameController::_GetCurPathIndices()
{
  std::vector<uint32_t> ret(m_current_path.size());
  for (int i = 0; i < m_hexes.size(); ++i)
  {
    for (int h = 0; i < m_current_path.size(); ++h)
    {
      if (&m_hexes[i] == m_current_path[h])
        ret[h] = i;
    }
  }
  return ret;
}

//-------------------------------------------------------------
void GameController::_DebugHexes()
{
  int counter = 0;
  for (auto& hex : m_hexes)
  {
    auto text = std::make_shared<Text>();
    texts.push_back(text);
    text->content = std::to_string(counter);
    text->font = "edwardian";
    text->pos_x = 0.0f;
    text->pos_y = 0.0f;
    text->scale = 1.0f;
    text->color = glm::vec3(1.0f, 1.0f, 1.0f);

    Transform trans;
    glm::vec2 offset{ -0.2f, -0.25f };
    trans.setTranslation({ hex.x() + offset.x, Hex::common_height + 0.01f, hex.z() + offset.y });
    trans.setScale({ 0.010f, 0.010f, 0.010f });
    trans.setRotation(-PI / 2, -PI / 2, 0.0f);
    text->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();

    m_game->AddText(text);
    ++counter;
  }
}

//-------------------------------------------------------------
void GameController::_SendMoveMsg()
{
  // should be sent when we actually move
  if (m_side != Side::BOTH) // send tcp event
  {
    std::vector<uint32_t> content{ (uint32_t)(m_current_path.size() + 2),
                                   (uint32_t)MessageType::MOVE,
                                   m_focused_index };
    std::vector<uint32_t> path(_GetCurPathIndices());
    content.insert(content.end(), path.begin(), path.end());

    if (m_tcpAgent)
      m_tcpAgent->SendMsg(std::move(content));
  }
}

//-------------------------------------------------------------
void GameController::_UpdateTextPath()
{
  if (!m_current_path.empty())
  {
    if (Hex* dest = m_current_path.back(); dest)
    {
      Transform trans;
      glm::vec2 offset{ -0.2f, -0.25f };
      trans.setTranslation({ dest->x() + offset.x, Hex::common_height + 0.01f, dest->z() + offset.y });
      trans.setScale({ 0.013f, 0.013f, 0.013f });
      trans.setRotation(-PI / 2, -PI / 2, 0.0f);
      m_destination_text->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();
    }
  }
  else
  {
    m_destination_text->content = "";
    m_path_mesh->UpdateData({}, {}, glm::vec4{ 1.0f });
  }
}

//-----------------------------------------------------
Hex* GameController::_GetCurHex()
{
  auto cur_hex = std::find_if(m_hexes.begin(), m_hexes.end(), [this](const Hex& hex)
    {
      return glm::length(m_game->GetFocusedObject()->GetTransform()->getTranslation() - glm::vec3{ hex.x(), m_pipeline.get().GetWaterHeight(), hex.z() }) < (0.5f * 0.57f); //radius
    });
  if (cur_hex != m_hexes.end())
    return &*cur_hex;
  else
    return nullptr;
}

//-------------------------------------------------------------
eShipScript* GameController::_GetShipScript(const eObject* _ship) const
{
  for (int i = 0; i < m_ships.size(); ++i)
  {
    if (m_ships[i]->GetScriptObject().lock().get() == _ship)
      return m_ships[i];
  }
  for (int i = 0; i < m_ships_pirate.size(); ++i)
  {
    if (m_ships_pirate[i]->GetScriptObject().lock().get() == _ship)
      return m_ships_pirate[i];
  }
  return nullptr;
}

//-------------------------------------------------------------
void   GameController::_UpdateCurrentPath()
{
  if (m_has_gold_index == -1)
  {
    _UpdateWarrning("Choose a ship which will hold the treasures by moving golden frame");
    return;
  }
  else if (!m_game->GetFocusedObject())
  {
    _UpdateWarrning("Choose a ship to move");
    return;
  }
  else if (_CurrentShipHasMoved())
  {
    _UpdateWarrning("This ship has already moved");
    return;
  }
  else if (!m_dice_rolled)
  {
    _UpdateWarrning("Roll the dice before moving");
    return;
  }
  else // player rolled dice and has not moved yet. so, lets calculate wheather and how they can move
  {
    // RIGHT CLICK ON WATER
    float waterHeight = m_pipeline.get().GetWaterHeight();
    dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f),
      glm::vec3(0.0f, waterHeight, 0.0f),
      glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane

    glm::vec3 target = dbb::intersection(pl, m_camera.get().getCameraRay().getLine());
    for (auto& hex : m_hexes)
    {
      if (hex.IsOn(target.x, target.z) && (hex.IsWater(m_terrain, waterHeight) || hex.IsBase()))
      {
        if (hex.IsTaken())
        {
          _UpdateWarrning("The ship can not move there(hex is taken)");
          m_current_path.clear();
          return;
        }
        if (auto cur_hex = _GetCurHex(); cur_hex)
        {
          m_current_path = cur_hex->MakePath(&hex, m_terrain, waterHeight, m_game_state == GameState::SPANISH_TO_MOVE);
          if (m_current_path.size() > m_current_dice) // can move less or equal to dice
          {
            _UpdateWarrning("The ship can not move there");
            m_current_path.clear();
            return;
          }
          else
          {
            _UpdateWarrning("");
            return;
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

//-----------------------------------------------------------------------------
size_t GameController::_GetPirateShipIndex(std::shared_ptr<eObject> _ship)
{
  for (size_t i = 0; i < m_ships_pirate.size(); ++i)
  {
    if (_ship->GetScript() == m_ships_pirate[i])
      return i;
  }
  return -1;
}

//-----------------------------------------------------------------------------
size_t GameController::_GetSpanishShipIndex(std::shared_ptr<eObject> _ship)
{
  for (size_t i = 0; i < m_ships.size(); ++i)
  {
    if (_ship->GetScript() == m_ships[i])
      return i;
  }
  return -1;
}
