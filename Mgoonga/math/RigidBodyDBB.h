#pragma once

#include "math.h"
#include "Geometry.h"

#include <vector>

namespace dbb
{
  class RigidBody;

  //---------------------------------------------------------------------
  class ICollider //@todo move colliders to separate file
  {
  public:
    static CollisionManifold FindCollisionFeatures(const ICollider& _A, const ICollider& _B) { return _A.CollidesWith(_B); }

    virtual void SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) = 0;
    virtual glm::vec4 GetTensor(RigidBody&) const = 0;
    virtual CollisionManifold CollidesWith(const ICollider& _other) const = 0;
  };

  class SphereCollider;

  class OBBCollider : public ICollider
  {
  public:
    virtual void SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4 GetTensor(RigidBody&) const override;
    virtual CollisionManifold CollidesWith(const ICollider& _other) const override { return _other.CollidesWith(*this); }
    const dbb::OBB& GetBox() const { return box; }
  protected:
    CollisionManifold CollidesWith(const SphereCollider& _other) const;
    CollisionManifold CollidesWith(const OBBCollider& _other) const;
    dbb::OBB box;
  };

  class SphereCollider : public ICollider
  {
  public:
    virtual void SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4 GetTensor(RigidBody&) const override;
    virtual CollisionManifold CollidesWith(const ICollider& _other) const override { return _other.CollidesWith(*this); }

    const dbb::sphere& GetSphere() const { return sphere; }
  protected:
    CollisionManifold CollidesWith(const SphereCollider& _other) const;
    CollisionManifold CollidesWith(const OBBCollider& _other) const;
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

    dbb::ICollider* GetCollider() const { return m_collider; }

    void      SetPosition(const glm::vec3& pos) { m_position = m_oldPosition = pos; }
    glm::vec3 GetPosition() { return m_position; }
    void      SetBounce(float b) { m_bounce = b; }
    float     GetBounce() const{ return m_bounce; }
    float     GetMass() const { return m_mass; }
    glm::vec3 GetVelocity() const;

    void SynchCollisionVolumes();
    float InvMass();
    void AddLinearImpulse(const glm::vec3& impulse);

    glm::mat4 InvTensor();
    virtual void AddRotationalImpulse(const glm::vec3& point, const glm::vec3& impulse);

  protected:
    dbb::ICollider* m_collider = nullptr; // no collider means particle
    glm::vec3 m_position;
    glm::vec3 m_oldPosition;
    glm::vec3 m_velocity;
    glm::vec3 m_forces; // sum of all forces
    float m_mass = 1.0f;
    float m_cor; // coef of restitution
    float m_bounce = 0.7f;
    float m_friction;
    //rotation info
    glm::vec3 m_orientation;
    glm::vec3 m_angVel;
    glm::vec3 m_torques; // Sum torques
  };
}