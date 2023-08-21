#include "GameController.h"

#include <base/base.h>

#include <math/Camera.h>
#include <math/Random.h>

#include <tcp_lib/Network.h>
#include <tcp_lib/Server.h>
#include <tcp_lib/Client.h>

#include <opengl_assets/ModelManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/Sound.h>
#include <opengl_assets/openglrenderpipeline.h>
#include <opengl_assets/TerrainModel.h>

#include <sdl_assets/ImGuiContext.h>

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
  , m_texture_scales{ 10.0f, 10.0f , 5.0f}
{
}

//------------------------------------------------
GameController::~GameController()
{
  if(m_tcpAgent)
    dbb::NetWork::Shutdown();
}

//------------------------------------------------
void GameController::Initialize()
{
  m_game->GetMainLight().light_position = glm::vec4{ 0.0f, 4.0f,0.0f, 1.0f };
  m_game->GetMainLight().intensity = glm::vec3{ 100.0f, 100.0f , 100.0f };
  m_game->GetMainCamera().setPosition(glm::vec3{-4.0f, 4.0f, 0.0f});
  m_game->GetMainCamera().setDirection(glm::normalize(glm::vec3{ 0.815, -0.588f, -0.018f }));
  m_pipeline.get().GetOutlineFocusedRef() = false;

  m_game->ObjectPicked.Subscribe([this](shObject _picked)->void { this->OnObjectPicked(_picked); });

  _InitializeDiceLogicAndVisual();
  _InitializeShipIcons();
  _InitializeHexes();
  _InitializeShips();
  _InitializeBases();
  _InitializeGoldenFrame();

  ObjectFactoryBase factory;
  m_choice_circle = new SimpleGeometryMesh({ NONE }, 0.15f, SimpleGeometryMesh::GeometryType::Circle, { 5.0f, 5.0f, 0.0f });
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_choice_circle), eObject::RenderType::GEOMETRY, "Choice circle"));
  m_path_mesh = new LineMesh({}, {}, glm::vec3{1.0f,1.0f,1.0f});
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

  //_InstallTcpServer();
  _InstallTcpClient();

  m_side = Side::PIRATE;

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

  std::function<void()> send_msg_test__callback = [this]()
  {
    if(m_tcpAgent)
      m_tcpAgent->SendMsg(std::vector<uint32_t>{ 3, 1, 2, 3 });
  };

  m_debug_window->Add(BUTTON, "Terrain rendering", (void*)&switch_mode__callback);
  m_debug_window->Add(SLIDER_FLOAT, "Texture Scale under watter", &m_texture_scales[0]);
  m_debug_window->Add(SLIDER_FLOAT, "Texture Scale grass", &m_texture_scales[1]);
  m_debug_window->Add(SLIDER_FLOAT, "Texture Scale grass 2", &m_texture_scales[2]);
  m_debug_window->Add(BUTTON, "Debug Hexes", (void*)&debug_hexes);
  m_debug_window->Add(SLIDER_FLOAT, "Height Scale", &m_height_scale);
  m_debug_window->Add(SLIDER_FLOAT, "Max Height", &m_max_height);
  m_debug_window->Add(SLIDER_FLOAT, "Water Level Height", &m_water_level);
  m_debug_window->Add(BUTTON, "Switch LOD", (void*)&switch_lod__callback);
  m_debug_window->Add(BUTTON, "Send Test Msg", (void*)&send_msg_test__callback);
}

//--------------------------------------------------------------------------
bool GameController::OnKeyPress(uint32_t _asci)
{
  switch (_asci)
  {
  case ASCII_J:
  {
    m_dice_gui->Perssed();
  }
  return true;
  default: return false;
  }
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
  if (!_left) // right click
  {
    if(m_has_gold_index == -1)
    {
      _UpdateWarrning("Choose a ship which will hold the treasures by moving golden frame");
      return true;
    }
    else if (!m_game->GetFocusedObject())
    {
      _UpdateWarrning("Choose a ship to move");
      return true;
    }
    else if (_CurrentShipHasMoved())
    {
      _UpdateWarrning("This ship has already moved");
      return true;
    }
    else if (!m_dice_rolled)
    {
      _UpdateWarrning("Roll the dice before moving");
      return true;
    }
    else if (!m_current_path.empty())
    {
      _UpdateWarrning("The ship can not move there");
      return true;
    }
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
          if (hex.IsTaken())
          {
            _UpdateWarrning("The ship can not move there");
            m_current_path.clear();
            return true;
          }

          auto cur_hex = std::find_if(m_hexes.begin(), m_hexes.end(), [this , focused, waterHeight](const Hex& hex)
          {
            return glm::length(focused->GetTransform()->getTranslation() - glm::vec3{ hex.x(), waterHeight, hex.z() }) < (0.5f * 0.57f); //radius
          });

          if (cur_hex != m_hexes.end())
          {
            m_current_path = cur_hex->MakePath(&hex, m_terrain, waterHeight);
            if (m_current_path.size() > m_current_dice) // can move less or equal to dice
            {
              _UpdateWarrning("The ship can not move there");
              m_current_path.clear();
              return true;
            }
            if (!m_current_path.empty())
            {
              glm::vec3 destination = { m_current_path.front()->x(), waterHeight, m_current_path.front()->z() };
              m_current_path.pop_front();

              if (m_game_state == GameState::SPANISH_TO_MOVE && m_side != Side::PIRATE)
                m_ships[m_focused_index]->SetDestination(destination);
              else if (m_game_state == GameState::PIRATE_TO_MOVE && m_side != Side::SPANISH)
                m_ships_pirate[m_focused_index]->SetDestination(destination);
              else
              {
                m_current_path.clear();
                return true;
              }

              std::vector<uint32_t> content { (uint32_t)(m_current_path.size() + 2),
                                             (uint32_t)MessageType::MOVE,
                                             m_focused_index };
              std::vector<uint32_t> path(_GetCurPathIndices());
              content.insert(content.end(), path.begin(), path.end());

              if (m_tcpAgent)
                m_tcpAgent->SendMsg(std::move(content));

              _UpdatePathVisual();
              hex.SetTaken(true);
              cur_hex->SetTaken(false);
              _UpdateWarrning("");
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
  if (m_game_state == GameState::SPANISH_TO_MOVE)
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
  }
  else if (m_game_state == GameState::PIRATE_TO_MOVE)
  {
    for (int i = 0; i < m_ships_pirate.size(); i++)
    {
      if (m_ships_pirate[i]->GetScriptObject() == m_game->GetFocusedObject().get()
          && m_ships_pirate[i]->GetDestination() != NONE)
      {
        m_dice_rolled = false;
        m_has_moved_pirate[i] = true;
      }
    }
  }
  return false;
}

//-------------------------------------------------------------
void GameController::Update(float _tick)
{
  for (int i = 0; i < m_ships.size(); ++i)
  {
    if (m_ships[i]->GetScriptObject() == m_game->GetFocusedObject().get()
        && m_ships[i]->GetDestination() == NONE
        && !m_current_path.empty())
    {
      glm::vec3 destination = { m_current_path.front()->x(), m_pipeline.get().GetWaterHeight(), m_current_path.front()->z() };
      m_current_path.pop_front();
      m_ships[m_focused_index]->SetDestination(destination);
    }

    if (m_ships[i]->GetScriptObject() == m_game->GetFocusedObject().get())
      m_choice_circle->SetDots({ m_ships[i]->GetScriptObject()->GetTransform()->getTranslation() + glm::vec3{0.0f,0.01f,0.0f} });
  }

  for (int i = 0; i < m_ships_pirate.size(); ++i)
  {
    if (m_ships_pirate[i]->GetScriptObject() == m_game->GetFocusedObject().get()
        && m_ships_pirate[i]->GetDestination() == NONE
        && !m_current_path.empty())
    {
      glm::vec3 destination = { m_current_path.front()->x(), m_pipeline.get().GetWaterHeight(), m_current_path.front()->z() };
      m_current_path.pop_front();
      m_ships_pirate[m_focused_index]->SetDestination(destination);
    }

    if (m_ships_pirate[i]->GetScriptObject() == m_game->GetFocusedObject().get())
      m_choice_circle->SetDots({ m_ships_pirate[i]->GetScriptObject()->GetTransform()->getTranslation() + glm::vec3{0.0f,0.01f,0.0f} });
  }

  if (m_game_state == GameState::SPANISH_TO_MOVE)
  {
    if (std::all_of(m_has_moved.begin(), m_has_moved.end(), [](bool _val) { return _val; }))
    {
      std::for_each(m_has_moved.begin(), m_has_moved.end(), [](bool& _val) { _val = false; });
      m_game_state = GameState::PIRATE_TO_MOVE;
    }
  }
  else if (m_game_state == GameState::PIRATE_TO_MOVE)
  {
    if (std::all_of(m_has_moved_pirate.begin(), m_has_moved_pirate.end(), [](bool _val) { return _val; }))
    {
      std::for_each(m_has_moved_pirate.begin(), m_has_moved_pirate.end(), [](bool& _val) { _val = false; });
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

  //update text
  if (m_current_path.size() != 0)
  {
    Transform trans;
    glm::vec2 offset{-0.2f, -0.25f};
    trans.setTranslation({ m_current_path.back()->x() + offset.x, Hex::common_height + 0.01f, m_current_path.back()->z() + offset.y });
    trans.setScale({ 0.013f, 0.013f, 0.013f });
    trans.setRotation(-PI / 2, -PI / 2, 0.0f);
    m_destination_text->mvp = m_camera.get().getProjectionMatrix() * m_camera.get().getWorldToViewMatrix() * trans.getModelMatrix();
  }
  else
  {
    m_destination_text->content = "";
    m_path_mesh->UpdateData({}, {}, glm::vec3{ 1.0f });
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
    m_pipeline.get().SetUniformData("class eMainRender",
      "textureScale[" + std::to_string(i) + "]",
      m_texture_scales[i]);
  }

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
      terrainModel->initialize(m_texManager->Find("Tgrass0_d"),
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
      m_pipeline.get().SetUniformData("class eMainRender",
        "base_start_heights[" + std::to_string(counter) + "]",
        type.threshold_start);

      m_pipeline.get().SetUniformData("class eMainRender",
        "textureScale[" + std::to_string(counter) + "]",
        m_texture_scales[counter]);
      ++counter;
    }
    m_pipeline.get().SetUniformData("class eMainRender",
      "base_start_heights[" + std::to_string(counter) + "]",
      1.0f);
    water_level = m_water_level;
  }
}

//---------------------------------------------------------------
void GameController::OnObjectPicked(std::shared_ptr<eObject> _picked)
{
  if (!m_dice_rolled)
  {
    if (m_game_state == GameState::SPANISH_TO_MOVE)
    {
      unsigned int index_picked = -1;
      for (unsigned int i = 0; i < m_ships.size(); ++i)
      {
        if (m_ships[i]->GetScriptObject() == _picked.get())
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
        if (m_ships_pirate[i]->GetScriptObject() == _picked.get())
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
      if (hovered->GetScript() == m_ships[i])
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
void GameController::_OnConnectionEstablished(const dbb::TCPConnection& _connection)
{
  _connection.IntArrayMessageRecieved.Subscribe([this](const std::vector<uint32_t>& _content, const std::string& _endpoint){ _OnTCPMessageRecieved(_content); });
}

//-----------------------------------------------------------
void GameController::_OnTCPMessageRecieved(const std::vector<uint32_t> _content)
{
  std::cout << "_OnTCPMessageRecieved" << std::endl;
  if(_content[0] == (uint32_t)MessageType::MOVE)
  {
    glm::vec3 destination = { m_current_path.front()->x(), m_pipeline.get().GetWaterHeight(), m_current_path.front()->z() };
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
            if (!m_dice_rolled)
            {
              m_game->SetFocused(m_ships_pirate[i]->GetScriptObject());
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
  terrainModel->initialize(m_texManager->Find("Tgrass0_d"),
                           m_texManager->Find("Tgrass0_d"),
                           &Texture::GetTexture1x1(BLUE),
                           m_texManager->Find("TOcean0_s"),
                           true,
                           m_height_scale,
                           m_max_height);
  terrainModel->setAlbedoTextureArray(m_game->GetTexture("terrain_albedo_array_2"));
  terrainModel->getMeshes()[0]->SwitchLOD(2);
  //terrainModel->getMeshes()[0]->SetRenderMode(MyMesh::RenderMode::WIREFRAME);

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
  m_pipeline.get().SetUniformData("class eMainRender", "min_height", 0.0f);
  m_pipeline.get().SetUniformData("class eMainRender", "max_height", 1.0f);
  m_pipeline.get().SetUniformData("class eMainRender", "color_count", 2);

  std::set<TerrainType> terrain_types;
  terrain_types.insert({ "mounten",		0.0f, m_water_level, {0.5f, 0.5f, 0.0f } });
  terrain_types.insert({ "grass",		m_water_level,	1.0f, {0.0f, 1.0f, 0.0f} });
  terrain_types.insert({ "grass",		1.0f,	1.0f, {0.0f, 1.0f, 0.0f} });

  int counter = 0;
  for (const auto& type : terrain_types)
  {
    m_pipeline.get().SetUniformData("class eMainRender",
      "base_start_heights[" + std::to_string(counter) + "]",
      type.threshold_start);

    m_pipeline.get().SetUniformData("class eMainRender",
      "textureScale[" + std::to_string(counter) + "]",
      m_texture_scales[counter]);
    ++counter;
  }
  m_pipeline.get().SetUniformData("class eMainRender",
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
    //shObject ship = factory.CreateObject(m_modelManager->Find("ship"), eObject::RenderType::PBR, "Ship" + std::to_string(i));
    shObject ship = factory.CreateObject(m_modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "Ship" + std::to_string(i));
    ship->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
    ship->GetTransform()->setTranslation(vec3(m_hexes[90 + i*2].x(), m_pipeline.get().GetWaterHeight(), m_hexes[90 + i * 2].z()));
    ship->GetTransform()->setUp(glm::vec3(0.0f, 0.0f, 1.0f));
    ship->GetTransform()->setForward(glm::vec3(-1.0f, 0.0f, 0.0f));
    m_hexes[90 + i * 2].SetTaken(true);

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
    ship->SetInstancingTag("spanish_ship");
    m_ships.push_back(script);

    m_game->AddInputObserver(ship->GetScript(), WEAK);
    m_game->AddObject(ship);
  }

  for (int i = 0; i < m_ship_quantity_pirate; ++i)
  {
    //shObject ship = factory.CreateObject(m_modelManager->Find("pirate_ship"), eObject::RenderType::PBR, "Pirate" + std::to_string(i));
    shObject ship = factory.CreateObject(m_modelManager->Find("brick_cube"), eObject::RenderType::PHONG, "Ship" + std::to_string(i));
    //ship->GetTransform()->setScale(vec3(0.04f, 0.04f, 0.04f));
    ship->GetTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
    ship->GetTransform()->setTranslation(vec3(m_hexes[26 + i * 20].x(), m_pipeline.get().GetWaterHeight(), m_hexes[26 + i * 20].z()));
    ship->GetTransform()->setUp(glm::vec3(0.0f, 0.0f, 1.0f));
    ship->GetTransform()->setForward(glm::vec3(-1.0f, 0.0f, 0.0f));

    if(i == 0)
    {
      for (auto& mesh : ship->GetModel()->Get3DMeshes())
      {
        Texture t;
        if (auto& textures = mesh->GetTextures(); !textures.empty())
        {
          if (textures[0].m_path.find("barrel_texture1k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/barrel_texture1k/DefaultMaterial_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;

            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/barrel_texture1k/DefaultMaterial_Normal_DirectX_1001.png");
            t.type = "texture_normal";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_normal = true;
          }
          else if (textures[0].m_path.find("cannon_texture1k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/cannon_texture1k/DefaultMaterial_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;
          }
          else if (textures[0].m_path.find("ship_texture4k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/ship_texture4k/ship_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;

            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/ship_texture4k/ship_Normal_DirectX_1001.png");
            t.type = "texture_normal";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_normal = true;
          }
          else if (textures[0].m_path.find("skull_texture1k") != std::string::npos)
          {
            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/skull_texture1k/Skull_Metallic_1001.png");
            t.type = "texture_specular";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_metalic = true;

            t.loadTextureFromFile("../game_assets/Resources/Pirate Ship/skull_texture1k/Skull_Normal_DirectX_1001.png");
            t.type = "texture_normal";
            const_cast<I3DMesh*>(mesh)->AddTexture(&t);
            mesh->GetMaterial()->use_normal = true;
          }
        }
      }
    }

    m_hexes[26 + i * 20].SetTaken(true);

    //for (auto& mesh : ship->GetModel()->Get3DMeshes())
    //  const_cast<I3DMesh*>(mesh)->SetMaterial(material); //@todo

    auto script = new eShipScript(m_game,
                                  m_texManager->Find("TPirate_flag0_s"),
                                  m_pipeline,
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
  shObject base1 = factory.CreateObject(m_modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "Base Veracruz");
  base1->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
  base1->GetTransform()->setTranslation(vec3(m_hexes[99].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[99].z()));
  //@todo set own script with flag and may be some logic
  m_game->AddObject(base1);
  
  m_base_labels.push_back(std::make_shared<Text>());
  m_base_labels[0]->content = "Veracruz";
  m_base_labels[0]->font = "edwardian";
  m_base_labels[0]->pos_x = 0.0f;
  m_base_labels[0]->pos_y = 0.0f;
  m_base_labels[0]->scale = 1.0f;
  m_base_labels[0]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_game->AddText(m_base_labels[0]);

  //2
  shObject base2 = factory.CreateObject(m_modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "Base Cartagena");
  base2->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
  base2->GetTransform()->setTranslation(vec3(m_hexes[38].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[38].z()));
  //@todo set own script with flag and may be some logic
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
  shObject base3 = factory.CreateObject(m_modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "Base Casablanca");
  base3->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
  base3->GetTransform()->setTranslation(vec3(m_hexes[83].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[83].z()));
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
  shObject base4 = factory.CreateObject(m_modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "Base Sevillia");
  base4->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
  base4->GetTransform()->setTranslation(vec3(m_hexes[141].x(), m_pipeline.get().GetWaterHeight() + 0.15f, m_hexes[141].z()));
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
  m_game->AddInputObserver(frame_gui.get(), MONOPOLY);
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
      if (m_ships[i]->GetScriptObject() == m_game->GetFocusedObject().get()
        && m_has_moved[i] == true)
        return true;
    }
  }
  else if (m_game_state == GameState::PIRATE_TO_MOVE)
  {
    for (int i = 0; i < m_ships_pirate.size(); ++i)
    {
      if (m_ships_pirate[i]->GetScriptObject() == m_game->GetFocusedObject().get()
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
void GameController::_UpdatePathVisual()
{
  std::vector<glm::vec3>	verices;
  std::vector<GLuint>			indices;
  glm::vec3 pos;
  if(m_game_state == GameState::SPANISH_TO_MOVE)
    pos = m_ships[m_focused_index]->GetScriptObject()->GetTransform()->getTranslation();
  else if(m_game_state == GameState::PIRATE_TO_MOVE)
    pos = m_ships_pirate[m_focused_index]->GetScriptObject()->GetTransform()->getTranslation();

  pos.y += 0.01f;
  verices.push_back(pos);
  for (auto& hex : m_current_path)
    verices.push_back({ hex->x(), Hex::common_height + 0.01f, hex->z() });

  for (unsigned int i = 0; i < m_current_path.size() + 1; ++i)
  {
    indices.push_back(i);
    if(i!=0)
      indices.push_back(i);
  }
  m_path_mesh->UpdateData(verices, indices, { 1.0f, 1.0f ,1.0f });

  //text destination
  m_destination_text->content = std::to_string(m_current_path.size());
}

//-------------------------------------------------------------
void GameController::_UpdateWarrning(const std::string& _message)
{
  m_warrining->content = _message;
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
