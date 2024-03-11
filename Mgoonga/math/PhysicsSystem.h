#pragma once

#include "math.h"

#include "RigidBodyDBB.h"

namespace dbb
{
  //------------------------------------------------------------------------
  class PhysicsSystem
  {
    struct CollisionPair
    {
      CollisionPair(dbb::RigidBody* _A, dbb::RigidBody* _B, const CollisionManifold& _res):m_A(_A), m_B(_B), m_result(_res) {}
      dbb::RigidBody* m_A = nullptr;
      dbb::RigidBody* m_B = nullptr;
      CollisionManifold m_result;
    };
  public:
    void Update(float _deltaTime); //needs to be called at fixed fps, should be separate thread(or separate clock) with fixed interval of calls like 30fps
    void AddRigidbody(dbb::RigidBody* _body);
    void AddConstraint(const OBB& _constraint);
    void ClearRigidbodys();
    void ClearConstraints();
    void ClearCollisions();
  protected:
    std::vector<dbb::RigidBody*> m_bodies;
    std::vector<OBB> m_constraints;
    std::vector<PhysicsSystem::CollisionPair> m_collisions;

    float m_linearProjectionPercent = 0.45f;
    float m_penetrationSlack = 0.01f;
    int m_impulseIteration = 6;
  };
}