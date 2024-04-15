#pragma once

#include "math.h"
#include "Geometry.h"

#include <base/interfaces.h>
#include <base/base.h>

#include <vector>
#include <optional>

namespace dbb
{
  class RigidBody;

  //---------------------------------------------------------------------
  class DLL_MATH RigidBody : public IRigidBody
  {
  public:
    explicit RigidBody(ICollider* _collider = nullptr);

    static glm::vec3  g_gravity; //@todo constexpr somewhere above
    static float      g_friction;

    float      g_angular_vel_damper = 1.0f; //temp

    static CollisionManifold FindCollisionFeatures(RigidBody& ra, RigidBody& rb);

    static void ApplyImpulse(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c);
    static void ApplyImpulseLinear(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c);

    virtual void Update(float _deltaTime);
    virtual void ApplyForces();
    virtual void SolveConstraints(const std::vector<OBB>& _constraints);
    virtual bool HasVolume() const;

    virtual void  AddLinearImpulse(const glm::vec3& impulse);
    virtual void  AddRotationalImpulse(const glm::vec3& point, const glm::vec3& impulse);

    ICollider*      GetCollider() const;
    void            SetCollider(ICollider*);

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
    ICollider*          m_collider = nullptr; // no collider means particle
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