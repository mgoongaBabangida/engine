#pragma once

#include "math.h"

#include <base/Event.h>
#include "RigidBodyDBB.h"

namespace dbb
{
  struct CollisionPair
  {
    CollisionPair(dbb::RigidBody* _A, dbb::RigidBody* _B, const CollisionManifold& _res)
      :m_A(_A), m_B(_B), m_result(_res) {}
    dbb::RigidBody* m_A = nullptr;
    dbb::RigidBody* m_B = nullptr;
    CollisionManifold m_result;
  };

  //------------------------------------------------------------------------
  class DLL_MATH PhysicsSystem
  {
  public:
    Event<std::function<void(const CollisionPair&)>>  CollisionOccured;

    void Update(float _deltaTime); //needs to be called at fixed fps, should be separate thread(or separate clock) with fixed interval of calls like 30fps
    void AddRigidbody(dbb::RigidBody* _body);
    void AddConstraint(const OBB& _constraint);

    void SetLinearProjectionPercent(float);
    float GetLinearProjectionPercent();

    void SetPenetrationSlack(float);
    float GetPenetrationSlack();

    void SetImpulseIteration(int32_t);
    int32_t GetImpulseIteration();

    void SetLinearImpulsesOnly(bool);
    bool GetLinearImpulsesOnly();

    void ClearRigidbodys();
    void ClearConstraints();
    void ClearCollisions();
  protected:
    std::vector<dbb::RigidBody*>   m_bodies;
    std::vector<dbb::OBB>          m_constraints;
    std::vector<CollisionPair>     m_collisions;

    float m_linearProjectionPercent = 0.6f;
    float m_penetrationSlack = 0.01f;
    int32_t m_impulseIteration = 20;
    bool m_linear_impulses_only = false;
  };
}