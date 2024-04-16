#pragma once

#include "math.h"

#include <base/Event.h>
#include "RigidBody.h"
#include "Spring.h"
#include "Cloth.h"

#include <queue>

namespace dbb
{
  struct CollisionPair
  {
    CollisionPair(std::shared_ptr<dbb::RigidBody> _A, std::shared_ptr<dbb::RigidBody> _B, const CollisionManifold& _res)
      :m_A(_A), m_B(_B), m_result(_res) {}
    std::shared_ptr<dbb::RigidBody> m_A = nullptr;
    std::shared_ptr<dbb::RigidBody> m_B = nullptr;
    CollisionManifold m_result;
  };

  //------------------------------------------------------------------------
  class DLL_MATH PhysicsSystem
  {
  public:
    Event<std::function<void(const CollisionPair&)>>  CollisionOccured;

    void Update(float _deltaTime); // called in main thread
    void UpdateAsync(float _deltaTime); //needs to be called at fixed fps, should be separate thread(or separate clock) with fixed interval of calls like 30fps
    
    std::vector<std::shared_ptr<dbb::RigidBody>>  GetRigidBodies() const { return m_bodies; }

    void AddRigidbody(std::shared_ptr<dbb::RigidBody> _body);
    void AddConstraint(const OBB& _constraint);
    void AddSpring(const Spring& spring);
    void AddCloth(Cloth* cloth);

    void SetLinearProjectionPercent(float);
    float GetLinearProjectionPercent();

    void SetPenetrationSlack(float);
    float GetPenetrationSlack();

    void SetImpulseIteration(int32_t);
    int32_t GetImpulseIteration();

    void SetLinearImpulsesOnly(bool);
    bool GetLinearImpulsesOnly();

    void SetCorrectAllObjects(bool);
    bool GetCorrectAllObjects();

    void ClearRigidbodys();
    void ClearConstraints();
    void ClearSprings();
    void ClearCloths();

  protected:
    void ClearCollisions();

    std::vector<std::shared_ptr<dbb::RigidBody>>   m_bodies;
    std::vector<Spring>                            m_springs;
    std::vector<Cloth*>                            m_cloths;
    std::atomic<bool>								               body_container_flag = false;
    std::vector<dbb::OBB>                          m_constraints;
    std::vector<CollisionPair>                     m_collisions;
    std::queue<CollisionPair>                      m_callbacks;

    float m_linearProjectionPercent = 0.1f;
    float m_penetrationSlack = 0.01f;
    int32_t m_impulseIteration = 20;
    bool m_linear_impulses_only = false;
    bool m_correct_all_objects = false;
  };
}