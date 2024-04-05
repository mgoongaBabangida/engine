#pragma once

#include "math.h"
#include "Geometry.h"

#include <vector>
#include <optional>

namespace dbb
{
  class RigidBody;

  class SphereCollider;
  class OBBCollider;

  //---------------------------------------------------------------------
  class DLL_MATH ICollider //@todo move colliders to separate file
  {
  public:
    static CollisionManifold FindCollisionFeatures(const ICollider& _A, const ICollider& _B);

    virtual CollisionManifold Dispatch(const ICollider& other) const = 0;
    virtual void SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) = 0;
    virtual glm::vec4 GetTensor(RigidBody&) const = 0;
    virtual CollisionManifold CollidesWith(const SphereCollider& _other) const = 0;
    virtual CollisionManifold CollidesWith(const OBBCollider& _other) const = 0;

    virtual dbb::point GetCenter() const = 0;
    virtual glm::vec3 GetOrientation() const = 0;
    virtual std::optional<dbb::OBB> GetBox() const { return std::nullopt; }
    virtual std::optional<dbb::sphere> GetSphere() const { return std::nullopt; }
  };

  //---------------------------------------------------------------------
  class DLL_MATH OBBCollider : public ICollider
  {
  public:
    OBBCollider(const dbb::OBB& _obb) : box(_obb) {}

    virtual void                    SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4               GetTensor(RigidBody&) const override;
    virtual std::optional<dbb::OBB> GetBox() const override;
    virtual dbb::point              GetCenter() const override;
    virtual glm::vec3               GetOrientation() const override;

    virtual CollisionManifold       Dispatch(const ICollider& other) const override;
  protected:
    virtual CollisionManifold       CollidesWith(const SphereCollider& _other) const override;
    virtual CollisionManifold       CollidesWith(const OBBCollider& _other) const override;
    dbb::OBB box;
  };

  //---------------------------------------------------------------------
  class DLL_MATH SphereCollider : public ICollider
  {
  public:
    SphereCollider(const dbb::sphere& _sphere) : sphere(_sphere) {}
    virtual void SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4 GetTensor(RigidBody&) const override;
    virtual std::optional<dbb::sphere> GetSphere() const override;
    virtual dbb::point GetCenter() const override;
    virtual glm::vec3 GetOrientation() const override;

    virtual CollisionManifold Dispatch(const ICollider& other) const override;
  protected:
    virtual CollisionManifold CollidesWith(const SphereCollider& _other) const override;
    virtual CollisionManifold CollidesWith(const OBBCollider& _other) const override;
    dbb::sphere sphere;
  };

  //---------------------------------------------------------------------
  class DLL_MATH RigidBody
  {
  public:
    explicit RigidBody(dbb::ICollider* _collider = nullptr);

    static glm::vec3  g_gravity; //@todo constexpr somewhere above
    static float      g_friction;

    float      g_angular_vel_damper = 1.0f;

    static CollisionManifold FindCollisionFeatures(RigidBody& ra, RigidBody& rb);
    static void ApplyImpulse(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c);
    static void ApplyImpulseLinear(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c);

    virtual void Update(float _deltaTime);
    virtual void ApplyForces();
    virtual void SolveConstraints(const std::vector<OBB>& _constraints);
    virtual bool HasVolume() const;

    virtual void  AddLinearImpulse(const glm::vec3& impulse);
    virtual void  AddRotationalImpulse(const glm::vec3& point, const glm::vec3& impulse);

    dbb::ICollider* GetCollider() const;
    void            SetCollider(dbb::ICollider*);
    void            SetPosition(const glm::vec3& pos);
    glm::vec3       GetPosition();
    void            SetBounce(float b);
    float           GetBounce() const;
    void            SetMass(float);
    float           GetMass() const;
    void            SetCoefOfRestitution(float);
    float           GetCoefOfRestitution() const;
    void            SetFriction(float);
    float           GetFriction() const;
    glm::vec3       GetVelocity() const;
    void            SetGravityApplicable(bool);
    bool            GetGravityApplicable() const;

    //debug
    float& GetDamping() { return m_damping; }

    // protected
    void SynchCollisionVolumes();
    float InvMass();
    glm::mat4 InvTensor();

  protected:
    dbb::ICollider*     m_collider = nullptr; // delete !!! // no collider means particle
    glm::vec3           m_position = {};
    glm::vec3           m_oldPosition = {};
    glm::vec3           m_velocity = {};
    glm::vec3           m_forces = {}; // sum of all forces

    float               m_mass = 1.0f;
    float               m_cor = 0.5f;
    float               m_bounce = 0.7f;
    float               m_friction = 0.2f;
    bool                m_gravity_applicable = true;
    float               m_damping = 0.98f;

    //rotation info
    glm::vec3 m_orientation = {};
    glm::vec3 m_angVel = {};
    glm::vec3 m_torques = {}; // Sum torques
  };
}