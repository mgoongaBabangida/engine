#pragma once
#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Object.h>

#include <math/Timer.h>

class eMainContextBase;
class LineMesh;
class IWindowImGui;

namespace dbb
{
  class RigidBody;
  class PhysicsSystem;
  struct CollisionPair;
}

//-------------------------------------------
class DLL_GAME_ASSETS PhysicsEngineTestScript : public IScript
{
public:
  PhysicsEngineTestScript(eMainContextBase* _game, IWindowImGui* _imgui);
  virtual ~PhysicsEngineTestScript();

  virtual bool  OnKeyPress(uint32_t _asci, KeyModifiers _modifier) override;
  virtual void	Update(float _tick) override;
  virtual void  Initialize() override;

  void ClearVisulaDebugInfo();
  void Reset();
  void OnCollisionOccured(const dbb::CollisionPair&);

protected:
  eMainContextBase*                   m_game = nullptr;
  std::unique_ptr<dbb::PhysicsSystem> m_physics_system;
  std::unique_ptr<math::Timer>        m_timer;

  std::pair<shObject, std::shared_ptr<dbb::RigidBody>> sphere1;
  std::pair<shObject, std::shared_ptr<dbb::RigidBody>> cube1;
  std::pair<shObject, std::shared_ptr<dbb::RigidBody>> sphere2;
  std::pair<shObject, std::shared_ptr<dbb::RigidBody>> cube2;
  std::pair<shObject, std::shared_ptr<dbb::RigidBody>> grassPlane;

  std::vector<shObject> m_visual;
  LineMesh*             m_normal_mesh = nullptr;
  bool                  m_simulation_on = false;
  bool                  m_reset = false;
  IWindowImGui*         m_imgui = nullptr;

  // parameters
  bool                  m_add_sphere = false;
  bool                  m_add_cube1 = true;
  bool                  m_add_cube2 = false;
  bool                  m_linear_impulses_only = false;
  float                 m_restitution = 0.5f;
  float                 m_friction = 0.1f;
  float                 m_terrain_mass = 1'000'000.f;

  float                 m_projection_percent = 0.6f;
  float                 m_penetration_slack = 0.01f;
  uint32_t              m_impulse_iterations = 20;
};