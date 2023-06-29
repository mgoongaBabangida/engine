#pragma once

#include <base/interfaces.h>

#include <opengl_assets/GUI.h>

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

//------------------------------------------------
class GameController : public IScript
{
public:
  GameController::GameController(eMainContextBase* _game,
                                 eModelManager* _modelManager,
                                 eTextureManager* _texManager,
                                 eSoundManager* _soundManager,
                                 eOpenGlRenderPipeline& _pipeline,
                                 Camera& _camera);

  virtual void Initialize() override;
  bool OnKeyPress(uint32_t _asci);
  virtual void Update(float _tick) override;
  virtual bool OnMouseMove(int32_t _x, int32_t _y) override;
  virtual bool OnMousePress(int32_t x, int32_t y, bool left) override;
  virtual bool OnMouseRelease() override;

protected:
  void OnObjectPicked(std::shared_ptr<eObject> _picked);

  void _InitializeDiceLogicAndVisual();
  void _InitializeShipIcons();
  void _InitializeHexes();
  void _InitializeShips();

  bool _CurrentShipHasMoved() const;
  const Texture* _GetDiceTexture() const;

  eMainContextBase* m_game = nullptr;
  eModelManager* m_modelManager = nullptr;
  eTextureManager* m_texManager = nullptr;
  eSoundManager* m_soundManager = nullptr;

  std::reference_wrapper<eOpenGlRenderPipeline> m_pipeline;
  std::reference_wrapper<Camera> m_camera;

  shObject                           m_terrain;

  static const uint32_t              m_ship_quantity = 4;

  std::vector<Hex>					         m_hexes;
  std::deque<Hex*>                   m_current_path;
  std::vector<eShipScript*>          m_ships;
  std::vector<std::shared_ptr<GUI>>  m_ship_icons;
  std::vector<std::shared_ptr<GUI>>  m_status_icons;

  glm::vec2                          m_cursor_pos;
  uint32_t                           m_focused_index = -1;

  std::shared_ptr<GUI>               m_dice_gui;
  uint32_t                           m_current_dice = 1;
  bool                               m_dice_rolled = false;
  std::array<bool, m_ship_quantity>  m_has_moved;

  SimpleGeometryMesh*                m_choice_circle;
  std::shared_ptr<Text>              m_warrining;
};