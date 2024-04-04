#include "PhysicsSystem.h"

namespace dbb
{
  //-------------------------------------------------------
  void PhysicsSystem::Update(float _deltaTime)
  {
    ClearCollisions();
    for (size_t i = 0, size = m_bodies.size(); i < size; ++i)
    {
      for (size_t j = i; j < size; ++j)
      {
        if (i == j)
          continue;
        CollisionManifold result;
        CollisionManifold::ResetCollisionManifold(result);
        if (m_bodies[i]->HasVolume() && m_bodies[j]->HasVolume())
        {
          result = ICollider::FindCollisionFeatures(*m_bodies[i]->GetCollider(), *m_bodies[j]->GetCollider());
          if (result.colliding)
          {
            m_collisions.emplace_back(m_bodies[i], m_bodies[j], result);
            CollisionOccured.Occur(m_collisions.back());
          }
        }
      }
    }

    for (size_t i = 0, size = m_bodies.size(); i < size; ++i)
      m_bodies[i]->ApplyForces();

    for (size_t k = 0; k < m_impulseIteration; ++k)
    {
      for (size_t i = 0; i < m_collisions.size(); ++i)
      {
        size_t jSize = m_collisions[i].m_result.contacts.size();
        for (int j = 0; j < jSize; ++j)
        {
          if (m_collisions[i].m_A->HasVolume() && m_collisions[i].m_B->HasVolume())
          {
            if (m_linear_impulses_only)
              RigidBody::ApplyImpulseLinear(*m_collisions[i].m_A, *m_collisions[i].m_B, m_collisions[i].m_result, j);
            else
              RigidBody::ApplyImpulse(*m_collisions[i].m_A, *m_collisions[i].m_B, m_collisions[i].m_result, j);
          }
        }
      }
    }

    for (size_t i = 0, size = m_bodies.size(); i < size; ++i)
      m_bodies[i]->Update(_deltaTime);

    //sync
    for (size_t i = 0, size = m_collisions.size(); i < size; ++i)
    {
      float totalMass = m_collisions[i].m_A->InvMass() + m_collisions[i].m_B->InvMass();
      if (totalMass == 0.0f)
        continue;

      /*if (!m_collisions[i].m_A->GetGravityApplicable() || !m_collisions[i].m_B->GetGravityApplicable())
        m_collisions[i].m_result.depth *= 2;*/

      float depth = fmaxf(m_collisions[i].m_result.depth - m_penetrationSlack, 0.0f);
      float scalar = depth / totalMass;
      glm::vec3 correction = m_collisions[i].m_result.normal * scalar * m_linearProjectionPercent;

      if (m_collisions[i].m_A->GetGravityApplicable())
      {
        correction = glm::dot(m_collisions[i].m_A->GetVelocity(), m_collisions[i].m_result.normal) > 0.0f ? correction : -correction;
        m_collisions[i].m_A->SetPosition(m_collisions[i].m_A->GetPosition() + correction * m_collisions[i].m_A->InvMass());
      }
      if (m_collisions[i].m_B->GetGravityApplicable())
      {
        correction = glm::dot(m_collisions[i].m_B->GetVelocity(), m_collisions[i].m_result.normal) > 0.0f ? correction : -correction;
        m_collisions[i].m_B->SetPosition(m_collisions[i].m_B->GetPosition() + correction * m_collisions[i].m_B->InvMass());
      }

      m_collisions[i].m_A->SynchCollisionVolumes();
      m_collisions[i].m_A->SynchCollisionVolumes();
    }

    for (size_t i = 0, size = m_bodies.size(); i < size; ++i)
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
  void PhysicsSystem::SetLinearProjectionPercent(float _linearProjectionPercent)
  {
    m_linearProjectionPercent = _linearProjectionPercent;
  }
  //-------------------------------------------------------
  float PhysicsSystem::GetLinearProjectionPercent()
  {
    return m_linearProjectionPercent;
  }
  //-------------------------------------------------------
  void PhysicsSystem::SetPenetrationSlack(float _penetrationSlack)
  {
    m_penetrationSlack = _penetrationSlack;
  }
  //-------------------------------------------------------
  float PhysicsSystem::GetPenetrationSlack()
  {
    return m_penetrationSlack;
  }
  //-------------------------------------------------------
  void PhysicsSystem::SetImpulseIteration(int32_t _impulseIteration)
  {
    m_impulseIteration = _impulseIteration;
  }

  //-------------------------------------------------------
  int32_t PhysicsSystem::GetImpulseIteration()
  {
    return m_impulseIteration;
  }

  //-------------------------------------------------------
  void PhysicsSystem::SetLinearImpulsesOnly(bool _l)
  {
    m_linear_impulses_only = _l;
  }

  //-------------------------------------------------------
  bool PhysicsSystem::GetLinearImpulsesOnly()
  {
    return m_linear_impulses_only;
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

  //-------------------------------------------------------
  void PhysicsSystem::ClearCollisions()
  {
    m_collisions.clear();
  }
}