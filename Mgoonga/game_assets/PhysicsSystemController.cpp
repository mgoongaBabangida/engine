#include "stdafx.h"

#include "PhysicsSystemController.h"
#include "MainContextBase.h"

#include <math/PhysicsSystem.h>

//------------------------------------------------------------------------
PhysicsSystemController::PhysicsSystemController(eMainContextBase* _game)
  : m_game(_game)
  , m_physics_system(m_game->GetPhysicsSystem())
{
}

//------------------------------------------------------------------------
void PhysicsSystemController::Update(float _tick)
{
  if (m_simulation_on)
  {
    m_physics_system->Update(_tick); //Syncronous update to send events in main thread

    std::vector<shObject> objects = m_game->GetObjects();
    for (auto body : m_physics_system->GetRigidBodies()) // Set Transform position and orientation from colliders
    {
      auto it = std::find_if(objects.begin(), objects.end(), [body](const shObject& _obj) { return _obj->GetRigidBody() == body; });
      if(it!= objects.end())
        body->GetCollider()->SetTo(*((*it)->GetTransform()));
    }
  }
  else
  {
    std::vector<shObject> objects = m_game->GetObjects();
    for (auto body : m_physics_system->GetRigidBodies()) // Set colliders from tramsform
    {
      auto it = std::find_if(objects.begin(), objects.end(), [body](const shObject& _obj) { return _obj->GetRigidBody() == body; });
      if (it != objects.end())
        body->GetCollider()->SetFrom(*((*it)->GetTransform()));
    }
  }
}

//----------------------------------------------------
void PhysicsSystemController::Initialize()
{
  static math::eClock s_clock;
  s_clock.start();

  m_timer.reset(new math::Timer([this]()->bool
    {
      float tick = s_clock.newFrame();
      if (m_simulation_on)
        m_physics_system->UpdateAsync(tick);
      return true;
    }));
  m_timer->start(33); //~30 fps
}

//----------------------------------------------------
PhysicsSystemController::~PhysicsSystemController()
{
  m_timer->stop();
}
