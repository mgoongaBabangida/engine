#pragma once

#include "math.h"
#include "Geometry.h"

#include <vector>

namespace dbb
{
  //---------------------------------------------------------------------
  class Collider
  {
  public:
    static CollisionManifold FindCollisionFeatures(const Collider& _A, const Collider& _B) { return CollisionManifold(); }

    void SynchCollisionVolumes(const glm::vec3& _pos) { box.origin = _pos; sphere.position = _pos; }
  protected:
    dbb::OBB box;
    dbb::sphere sphere;
  };

  //---------------------------------------------------------------------
  class RigidBody
  {
  public:
    static glm::vec3 g_gravity; //@todo constexpr somewhere above
    static float g_friction;

    static CollisionManifold FindCollisionFeatures(RigidBody& ra, RigidBody& rb);
    static void ApplyImpulse(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c);

    virtual void Update(float _deltaTime);
    virtual void ApplyForces();
    virtual void SolveConstraints(const std::vector<OBB>& _constraints);
    virtual bool HasVolume() const { return m_collider == nullptr; }  // no collider means particle, no volume

    dbb::Collider* GetCollider() const { return m_collider; }

    void      SetPosition(const glm::vec3& pos) { m_position = m_oldPosition = pos; }
    glm::vec3 GetPosition() { return m_position; }
    void      SetBounce(float b) { m_bounce = b; }
    float     GetBounce() { return m_bounce; }
    glm::vec3 GetVelocity() const;

    void SynchCollisionVolumes();
    float InvMass();
    void AddLinearImpulse(const glm::vec3& impulse);

  protected:
    dbb::Collider* m_collider = nullptr; // no collider means particle
    glm::vec3 m_position;
    glm::vec3 m_oldPosition;
    glm::vec3 m_velocity;
    glm::vec3 m_forces; // sum of all forces
    float m_mass = 1.0f;
    float m_cor; // coef of restitution
    float m_bounce = 0.7f;
    float m_friction;
  };
}