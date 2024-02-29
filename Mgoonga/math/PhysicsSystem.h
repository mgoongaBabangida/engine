#pragma once

#include "math.h"

#include "RigidBodyDBB.h"

namespace dbb
{
  //------------------------------------------------------------------------
  class PhysicsSystem
  {
  public:
    void Update(float _deltaTime); //needs to be called at fixed fps, should be separate thread(or separate clock) with fixed interval of calls like 30fps
    void AddRigidbody(dbb::RigidBody* _body);
    void AddConstraint(const OBB& _constraint);
    void ClearRigidbodys();
    void ClearConstraints();

  protected:
    std::vector<dbb::RigidBody*> m_bodies;
    std::vector<OBB> m_constraints;
  };
}