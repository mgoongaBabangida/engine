#pragma once

#include <base/interfaces.h>

#include <opengl_assets/GUI.h>

#include <tcp_lib/TCPConnection.h>

#include "Hex.h"

class eObject;
class Camera;
class eMainContextBase;
class eModelManager;
class eTextureManager;
class eSoundManager;
class eOpenGlRenderPipeline;
class eShipScript;
class SimpleGeometryMesh;
class LineMesh;
class IWindowImGui;
class ITcpAgent;

//------------------------------------------------
class GameController : public IScript
{
public:
  enum class GameState
  {
    SPANISH_TO_MOVE,
    PIRATE_TO_MOVE
  };

  enum class Mode
  {
    SERVER,
    CLIENT,
    LOCAL
  };

  enum class Side
  {
    BOTH,
    SPANISH,
    PIRATE
  };

  enum class MessageType : uint32_t
  {
    MOVE,
  };

  GameController(eMainContextBase* _game,
                 eModelManager* _modelManager,
                 eTextureManager* _texManager,
                 eSoundManager* _soundManager,
                 eOpenGlRenderPipeline& _pipeline,
                 Camera& _camera,
                 IWindowImGui* = nullptr);

  virtual ~GameController();

  virtual void Initialize() override;
  virtual void Update(float _tick) override;

  virtual bool OnKeyPress(uint32_t _asci);
  virtual bool OnMouseMove(int32_t _x, int32_t _y) override;
  virtual bool OnMousePress(int32_t x, int32_t y, bool left) override;
  virtual bool OnMouseRelease() override;

protected:
  void OnObjectPicked(std::shared_ptr<eObject> _picked);
  void OnFrameMoved(std::shared_ptr<GUI> _frame);

  void _OnConnectionEstablished(const dbb::TCPConnection& _connection);
  void _OnTCPMessageRecieved(const std::vector<uint32_t>);

  void _InitializeDiceLogicAndVisual();
  void _InitializeShipIcons();
  void _InitializeHexes();
  void _InitializeShips();
  void _InitializeBases();
  void _InitializeGoldenFrame();

  void _InstallTcpServer();
  void _InstallTcpClient();

  bool            _CurrentShipHasMoved() const;
  const Texture*  _GetDiceTexture() const;
  void            _UpdatePathVisual();
  void            _UpdateWarrning(const std::string& _message);

  std::vector<uint32_t> _GetCurPathIndices();
  void _DebugHexes();

  eMainContextBase* m_game = nullptr;
  eModelManager*    m_modelManager = nullptr;
  eTextureManager*  m_texManager = nullptr;
  eSoundManager*    m_soundManager = nullptr;

  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  std::reference_wrapper<Camera> m_camera;

  GameState                          m_game_state = GameState::SPANISH_TO_MOVE;
  Side                               m_side = Side::BOTH;
  shObject                           m_terrain;

  static const uint32_t              m_ship_quantity = 4;
  static const uint32_t              m_ship_quantity_pirate = 6;

  std::vector<Hex>					         m_hexes;
  std::deque<Hex*>                   m_current_path;

  std::vector<eShipScript*>          m_ships;
  std::vector<eShipScript*>          m_ships_pirate;
  std::vector<std::shared_ptr<GUI>>  m_ship_icons;
  std::vector<std::shared_ptr<GUI>>  m_ship_icons_pirate;
  std::vector<std::shared_ptr<GUI>>  m_status_icons;
  std::vector<std::shared_ptr<GUI>>  m_status_icons_pirate;

  std::array<bool, m_ship_quantity>         m_has_moved;
  std::array<bool, m_ship_quantity_pirate>  m_has_moved_pirate;

  glm::vec2                          m_cursor_pos;
  uint32_t                           m_focused_index = -1;
  uint32_t                           m_has_gold_index = -1;

  std::shared_ptr<GUI>               m_dice_gui;
  uint32_t                           m_current_dice = 1;
  bool                               m_dice_rolled = false;

  SimpleGeometryMesh*                m_choice_circle;
  LineMesh*                          m_path_mesh;
  std::shared_ptr<Text>              m_warrining;
  std::shared_ptr<Text>              m_destination_text;
  std::vector<std::shared_ptr<Text>> m_base_labels;

  //tcp
  Mode                                m_mode = Mode::LOCAL;
  std::unique_ptr <ITcpAgent>				  m_tcpAgent;
  std::unique_ptr<math::Timer>			  m_tcpTimer;

  //debug
  IWindowImGui*                      m_debug_window = nullptr;
  std::vector<float>                 m_texture_scales;
  std::vector<std::shared_ptr<Text>> texts; //debug
  float                              m_height_scale = 2.0f;
  float                              m_max_height = 1.0f;
  float                              m_water_level = 0.5f;
};