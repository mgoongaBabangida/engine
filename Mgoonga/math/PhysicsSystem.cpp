#include "PhysicsSystem.h"

namespace dbb
{
  //-------------------------------------------------------
  void PhysicsSystem::Update(float _deltaTime)
  {
    for (int i = 0, size = m_bodies.size(); i < size; ++i)
      m_bodies[i]->ApplyForces();

    for (int i = 0, size = m_bodies.size(); i < size; ++i)
      m_bodies[i]->Update(_deltaTime);

    for (int i = 0, size = m_bodies.size(); i < size; ++i)
      m_bodies[i]->SolveConstraints(m_constraints);

  }

  //-------------------------------------------------------
  void PhysicsSystem::AddRigidbody(dbb::RigidBody* _body)
  {
    m_bodies.push_back(_body);
  }

  //-------------------------------------------------------
  void PhysicsSystem::AddConstraint(const OBB& _obb)
  {
    m_constraints.push_back(_obb);
  }

  //-------------------------------------------------------
  void PhysicsSystem::ClearRigidbodys()
  {
    m_bodies.clear();
  }

  //-------------------------------------------------------
  void PhysicsSystem::ClearConstraints()
  {
    m_constraints.clear();
  }
}