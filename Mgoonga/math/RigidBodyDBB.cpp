#include "RigidBodyDBB.h"

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

namespace dbb
{
  glm::vec3 RigidBody::g_gravity = glm::vec3(0.0f, -9.82f, 0.0f);
  float RigidBody::g_friction = 0.95f;

  //--------------------------------------------------------------------------------
  CollisionManifold RigidBody::FindCollisionFeatures(RigidBody& ra, RigidBody& rb)
  {
    if (ra.GetCollider() != nullptr && rb.GetCollider() != nullptr)
      return dbb::Collider::FindCollisionFeatures(*ra.GetCollider(), *rb.GetCollider());
    else
      return CollisionManifold();
  }

  //-----------------------------------------------------------------------------------------
  void RigidBody::ApplyImpulse(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c)
  {
    // Linear Velocity
    float invMass1 = A.InvMass();
    float invMass2 = B.InvMass();
    float invMassSum = invMass1 + invMass2;
    if (invMassSum == 0.0f) { return; }
    // Relative velocity
    glm::vec3 relativeVel = B.GetVelocity() - A.GetVelocity();
    // Relative collision normal
    glm::vec3 relativeNorm = M.normal;
    glm::normalize(relativeNorm);
    // Moving away from each other? Do nothing!
    if (glm::dot(relativeVel, relativeNorm) > 0.0f)
      return;

    float e = fminf(A.m_cor, B.m_cor);
    float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));
    float j = numerator / invMassSum;
    if (M.contacts.size() > 0.0f && j != 0.0f) {
      j /= (float)M.contacts.size();
    }
    glm::vec3 impulse = relativeNorm * j;
    A.m_velocity = A.GetVelocity() - impulse * invMass1; // modify old vel?
    B.m_velocity = B.GetVelocity() + impulse * invMass2;
    // Friction
    glm::vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));
    if (glm::length2(t) == 0.0f)
      return;

    glm::normalize(t);

    numerator = -glm::dot(relativeVel, t);
    float jt = numerator / invMassSum;
    if (M.contacts.size() > 0.0f && jt != 0.0f)
      jt /= (float)M.contacts.size();
    if (jt == 0.0f)
      return;
    //Coulomb's Law
    float friction = sqrtf(A.m_friction * B.m_friction);
    if (jt > j * friction)
      jt = j * friction;
    else if (jt < -j * friction)
      jt = -j * friction;
    glm::vec3 tangentImpuse = t * jt;
    A.m_velocity = A.m_velocity - tangentImpuse * invMass1;
    B.m_velocity = B.m_velocity + tangentImpuse * invMass2;
  }

  //--------------------------------------------
  void RigidBody::Update(float _deltaTime) //needs to be called at fixed fps
  {
    if(HasVolume())
    {
      m_oldPosition = m_position;
      const float damping = 0.98f;
      glm::vec3 acceleration = m_forces * InvMass();
      m_velocity = m_velocity + acceleration * _deltaTime;
      m_velocity = m_velocity * damping;
      m_position = m_position + m_velocity * _deltaTime;
      SynchCollisionVolumes();
    }
    else
    {
      glm::vec3 velocity = m_position - m_oldPosition;
      m_oldPosition = m_position;
      float deltaSquare = _deltaTime * _deltaTime;
      m_position = m_position + (velocity * g_friction + m_forces * deltaSquare);
    }
  }

  //---------------------------------------------
  void RigidBody::ApplyForces()
  {
    m_forces = g_gravity * m_mass;
  }

  //---------------------------------------------------------------------
  void RigidBody::SolveConstraints(const std::vector<OBB>& _constraints)
  {
    int size = _constraints.size();
    for (int i = 0; i < size; ++i)
    {
      dbb::lineSegment traveled(m_oldPosition, m_position);
      if (traveled.LineTest(_constraints[i]))
      {
        glm::vec3 velocity = m_position - m_oldPosition;
        glm::vec3 direction = glm::normalize(velocity);
        dbb::ray ray(m_oldPosition, direction);
        RaycastResult result;
        if (ray.Raycast(_constraints[i], result))
        {
          m_position = result.point + result.normal * 0.003f;
          glm::vec3 vn = result.normal * glm::dot(result.normal, velocity);
          glm::vec3 vt = velocity - vn;
          m_oldPosition = m_position - (vt - vn * m_bounce);
          break;
        }
      }
    }
  }

  //---------------------------------------------------------------------
  glm::vec3 RigidBody::GetVelocity() const
  {
    if (HasVolume())
      return m_oldPosition - m_position;
    else
      return m_velocity;
  }

  //---------------------------------------------------------------------
  void RigidBody::SynchCollisionVolumes()
  {
    if (m_collider)
      m_collider->SynchCollisionVolumes(m_position);
  }

  //---------------------------------------------------------------------
  float RigidBody::InvMass()
  {
    if (m_mass == 0.0f) { return 0.0f; }
    return 1.0f / m_mass;
  }

  //---------------------------------------------------------------------
  void RigidBody::AddLinearImpulse(const glm::vec3& _impulse)
  {
    m_velocity = m_velocity + _impulse;
  }
}