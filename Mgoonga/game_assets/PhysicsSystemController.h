#pragma once
#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Object.h>

#include <math/Timer.h>

class eMainContextBase;

namespace dbb
{
  class RigidBody;
  class PhysicsSystem;
  struct CollisionPair;
}

//-------------------------------------------
class DLL_GAME_ASSETS PhysicsSystemController : public IScript
{
public:
  PhysicsSystemController(eMainContextBase* _game);
  ~PhysicsSystemController();

  virtual void	Update(float _tick) override;
  virtual void  Initialize() override;

  void RunPhysics() { m_simulation_on = true; }
  void StopPhysics() { m_simulation_on = false; }

protected:
  eMainContextBase*             m_game = nullptr;
  dbb::PhysicsSystem*           m_physics_system = nullptr;
  std::unique_ptr<math::Timer>  m_timer;
  bool                          m_simulation_on = false;
};
