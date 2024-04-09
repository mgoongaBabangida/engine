#include "RigidBodyDBB.h"

#include "GeometryFunctions.h"

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>
#include <glm\glm\gtx\euler_angles.hpp>

namespace dbb
{
  glm::vec3 RigidBody::g_gravity = glm::vec3(0.0f, -9.82f, 0.0f);
  float RigidBody::g_friction = 0.95f;

  //--------------------------------------------------------------------------------
  RigidBody::RigidBody(dbb::ICollider* _collider)
    : m_collider(_collider)
  {
    if (m_collider != nullptr)
    {
      m_position = m_collider->GetCenter();
      m_orientation = m_collider->GetOrientation();
    }
  }

  //--------------------------------------------------------------------------------
  CollisionManifold RigidBody::FindCollisionFeatures(RigidBody& ra, RigidBody& rb)
  {
    if (ra.GetCollider() != nullptr && rb.GetCollider() != nullptr)
      return dbb::ICollider::FindCollisionFeatures(*ra.GetCollider(), *rb.GetCollider());
    else
      return CollisionManifold();
  }

  //-----------------------------------------------------------------------------------------
  void RigidBody::ApplyImpulse(RigidBody& _A, RigidBody& _B, const CollisionManifold& M, int c)
  {
    // Linear Velocity
    float invMass1 = _A.InvMass();
    float invMass2 = _B.InvMass();
    float invMassSum = invMass1 + invMass2;
    if (invMassSum == 0.0f)
      return;

    glm::vec3 r1 = M.contacts[c] - _A.m_position;
    glm::vec3 r2 = M.contacts[c] - _B.m_position;
    glm::mat4 i1 = _A.InvTensor();
    glm::mat4 i2 = _B.InvTensor();

    // Relative velocity (the difference between velocities)
    // The cross product of angular velocity and the relative contact point will give us the magnitude of rotational velocity
    glm::vec3 relativeVel = (_B.m_velocity + glm::cross(_B.m_angVel, r2)) - (_A.m_velocity + glm::cross(_A.m_angVel, r1));

    // Relative collision normal
    glm::vec3 relativeNorm = M.normal;
    glm::normalize(relativeNorm);

    // Moving away from each other? Do nothing!
    if (glm::dot(relativeVel, relativeNorm) > 0.0f)
      return;

    float e = fminf(_A.m_cor, _B.m_cor);
    float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));

    float d1 = invMassSum;
    glm::vec3 d2 = glm::cross(glm::cross(r1, relativeNorm) * glm::mat3(i1), r1); // mat3 ?
    glm::vec3 d3 = glm::cross(glm::cross(r2, relativeNorm) * glm::mat3(i2), r2);// mat3 ?
    float denominator = d1 + glm::dot(relativeNorm, d2 + d3);

    float j = (denominator == 0.0f) ? 0.0f : numerator / denominator; // j is magnitude of impulse needed to resolve the collision
    if (M.contacts.size() > 0.0f && j != 0.0f)
      j /= (float)M.contacts.size();

    glm::vec3 impulse = relativeNorm * j;
    _A.m_velocity = _A.m_velocity - impulse * invMass1;
    _B.m_velocity = _B.m_velocity + impulse * invMass2;

    _A.m_angVel = _A.m_angVel - (glm::cross(r1, impulse) * glm::mat3(i1));// mat3 ?
    _B.m_angVel = _B.m_angVel + (glm::cross(r2, impulse) * glm::mat3(i2));// mat3 ?

    glm::vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));

    if (glm::length2(t) == 0.0f)
      return;
    glm::normalize(t);

    numerator = -glm::dot(relativeVel, t);
    d1 = invMassSum;
    d2 = glm::cross(glm::cross(r1, t) * glm::mat3(i1), r1);
    d3 = glm::cross(glm::cross(r2, t) * glm::mat3(i2), r2);
    denominator = d1 + glm::dot(t, d2 + d3);

    if (denominator == 0.0f)
      return;

    float jt = numerator / denominator; // the magnitude of friction we are applying to the collision
    if (M.contacts.size() > 0.0f && jt != 0.0f)
      jt /= (float)M.contacts.size();

    if (jt == 0.0f)
      return;

    float friction = sqrtf(_A.m_friction * _B.m_friction);
    if (jt > j * friction)
      jt = j * friction;
    else if (jt < -j * friction)
      jt = -j * friction;

    glm::vec3 tangentImpuse = t * jt;

    _A.m_velocity = _A.m_velocity - tangentImpuse * invMass1;
    _B.m_velocity = _B.m_velocity + tangentImpuse * invMass2;

    _A.m_angVel = _A.m_angVel - (glm::cross(r1, tangentImpuse) * glm::mat3(i1));
    _B.m_angVel = _B.m_angVel + (glm::cross(r2, tangentImpuse) * glm::mat3(i2));
  }

  //-------------------------------------------------------------------------------------------
  void RigidBody::ApplyImpulseLinear(RigidBody& A, RigidBody& B, const CollisionManifold& M, int c)
  {
    // Linear Velocity
    float invMass1 = A.InvMass();
    float invMass2 = B.InvMass();
    float invMassSum = invMass1 + invMass2;
    if (invMassSum == 0.0f)
      return;

    // Relative velocity
    glm::vec3 relativeVel = B.m_velocity - A.m_velocity;
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
    _deltaTime /= 1'000.f;
    if(HasVolume())
    {
      m_oldPosition = m_position;

      glm::vec3 acceleration = m_forces * InvMass();
      m_velocity = m_velocity + acceleration * _deltaTime;
      m_velocity = m_velocity * m_damping;

      /*if (type == RIGIDBODY_TYPE_BOX) {*/
        glm::vec3 angAccel = glm::vec4(m_torques, 1.0f) * InvTensor();
        m_angVel = m_angVel + angAccel * _deltaTime;
        m_angVel = m_angVel * m_damping;

        if (m_gravity_applicable)
          m_position = m_position + m_velocity * _deltaTime;
       /* else
          m_velocity = {};*/

      /*if (type == RIGIDBODY_TYPE_BOX) {*/
      if(m_gravity_applicable)
        m_orientation = m_orientation + m_angVel * _deltaTime;

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
    if(m_gravity_applicable)
      m_forces = g_gravity * m_mass;
  }

  //---------------------------------------------------------------------
  void RigidBody::SolveConstraints(const std::vector<OBB>& _constraints)
  {
    if (HasVolume())
    {

    }
    else //for particles
    {
      size_t size = _constraints.size();
      for (size_t i = 0; i < size; ++i)
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
  }

  //---------------------------------------------------------------------
  bool RigidBody::HasVolume() const
  {
    return m_collider != nullptr;
  }  // no collider means particle, no volume

  //---------------------------------------------------------------------
  glm::vec3 RigidBody::GetVelocity() const
  {
    if (HasVolume())
      return m_oldPosition - m_position;
    else
      return m_velocity;
  }

  //---------------------------------------------------------------------
  void RigidBody::SetGravityApplicable(bool _gravity_app)
  {
    m_gravity_applicable = _gravity_app;
  }

  //---------------------------------------------------------------------
  bool RigidBody::GetGravityApplicable() const
  {
    return m_gravity_applicable;
  }

  //---------------------------------------------------------------------
  void RigidBody::SynchCollisionVolumes()
  {
    if (m_collider)
      m_collider->SynchCollisionVolumes(m_position, m_orientation);
  }

  //---------------------------------------------------------------------
  float RigidBody::InvMass()
  {
    if (m_mass == 0.0f)
      { return 0.0f; }
    return 1.0f / m_mass;
  }

  //---------------------------------------------------------------------
  void RigidBody::AddLinearImpulse(const glm::vec3& _impulse)
  {
    m_velocity = m_velocity + _impulse;
  }

  //--------------------------------------------------------------------------------------
  glm::mat4 RigidBody::InvTensor()
  {
    glm::vec4 i = m_collider->GetTensor(*this);
    return glm::inverse(glm::mat4(
                                  i.x, 0, 0, 0,
                                  0, i.y, 0, 0,
                                  0, 0, i.z, 0,
                                  0, 0, 0, i.w));
  }

  //--------------------------------------------------------------------------------------
  void RigidBody::AddRotationalImpulse(const glm::vec3& _point, const glm::vec3& _impulse)
  {
    glm::vec3 centerOfMass = m_position;
    glm::vec3 torque = glm::cross(_point - centerOfMass, _impulse);
    glm::vec3 angAccel = glm::vec4(torque, 1.0f) * InvTensor();
    m_angVel = m_angVel + angAccel;
  }

  //--------------------------------------------------------------------------------------
  dbb::ICollider* RigidBody::GetCollider() const
  {
    return m_collider;
  }

  //--------------------------------------------------------------------------------------
  void RigidBody::SetCollider(dbb::ICollider* _c)
  {
    dbb::ICollider* old = m_collider;
    if (_c)
    {
      m_collider = _c;
      m_position = m_collider->GetCenter();
      m_orientation = m_collider->GetOrientation();
    }
    if (old != nullptr)
      delete old;
  }

  //--------------------------------------------------------------------------------------
  void RigidBody::SetPosition(const glm::vec3& pos)
  {
    m_position = m_oldPosition = pos;
  }

  //--------------------------------------------------------------------------------------
  glm::vec3 RigidBody::GetPosition()
  {
    return m_position;
  }

  //--------------------------------------------------------------------------------------
  void RigidBody::SetBounce(float b)
  {
    m_bounce = b;
  }

  //--------------------------------------------------------------------------------------
  float RigidBody::GetBounce() const
  {
    return m_bounce;
  }

  //--------------------------------------------------------------------------------------
  void RigidBody::SetMass(float _mass)
  {
    m_mass = _mass;
  }

  //--------------------------------------------------------------------------------------
  float RigidBody::GetMass() const
  {
    return m_mass;
  }

  //---------------------------------------------------------------------------------------
  void RigidBody::SetCoefOfRestitution(float _cor)
  {
    m_cor = _cor;
  }

  //---------------------------------------------------------------------------------------
  float RigidBody::GetCoefOfRestitution() const
  {
    return m_cor;
  }

  //---------------------------------------------------------------------------------------
  void RigidBody::SetFriction(float _f)
  {
    m_friction = _f;
  }

  //---------------------------------------------------------------------------------------
  float RigidBody::GetFriction() const
  {
    return m_friction;
  }

  //---------------------------------------------------------------------------------------
  void OBBCollider::SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation)
  {
    box.origin = _pos;
    box.orientation = glm::eulerAngleYXZ(_orientation.y , _orientation.x, _orientation.z);
  }

  //--------------------------------------------------
  glm::vec4 OBBCollider::GetTensor(RigidBody& _rb) const
  {
    float ix = 0.0f;
    float iy = 0.0f;
    float iz = 0.0f;
    float iw = 0.0f;
    if(_rb.GetMass() != 0)
    {
      glm::vec3 size = box.size * 2.0f;
      float fraction = (1.0f / 12.0f);
      float x2 = size.x * size.x;
      float y2 = size.y * size.y;
      float z2 = size.z * size.z;
      ix = (y2 + z2) * _rb.GetMass() * fraction;
      iy = (x2 + z2) * _rb.GetMass() * fraction;
      iz = (x2 + y2) * _rb.GetMass() * fraction;
      iw = 1.0f;
    }
    return glm::vec4(ix, iy, iz, iw);
  }

  //---------------------------------------------------------------------------------------
  CollisionManifold OBBCollider::Dispatch(const ICollider& _other) const
  {
    return _other.CollidesWith(*this);
  }

  //---------------------------------------------------------------------------------------
  CollisionManifold SphereCollider::Dispatch(const ICollider& _other) const
  {
    return _other.CollidesWith(*this);
  }

  //---------------------------------------------------------------------------------------
  std::optional<dbb::OBB> OBBCollider::GetBox() const
  { return box; }

  //---------------------------------------------------------------------------------------
  dbb::point OBBCollider::GetCenter() const
  {
    return box.origin;
  }

  // -------------------------------------------------------------------------------------- -
  glm::vec3 OBBCollider::GetOrientation() const
  {
    return glm::eulerAngles(glm::toQuat(box.orientation)); // pitch(x), yaw(x), roll(x)
    //return { xyz.y, xyz.x, xyz.z }; /// yaw, pitch, roll
  }

  //---------------------------------------------------------------------------------------
  std::optional <dbb::sphere> SphereCollider::GetSphere() const
  { return sphere; }

  //---------------------------------------------------------------------------------------
  dbb::point SphereCollider::GetCenter() const
  {
    return sphere.position;
  }

  // -------------------------------------------------------------------------------------- -
  glm::vec3 SphereCollider::GetOrientation() const
  {
    return {1,1,1};
  }

  //---------------------------------------------------------------------------------------
  CollisionManifold OBBCollider::CollidesWith(const SphereCollider& _sphere) const
  {
    CollisionManifold cm = FindCollision(box, *_sphere.GetSphere()); //@todo check optional
    cm.normal = -cm.normal;
    return cm;
  }

  //---------------------------------------------------------------------------------------
  CollisionManifold OBBCollider::CollidesWith(const OBBCollider& _box) const
  {
    return FindCollision(*_box.GetBox(), box);
  }

  //--------------------------------------------------------------------------------
  CollisionManifold SphereCollider::CollidesWith(const SphereCollider& _sphere) const
  {
    return FindCollision(*_sphere.GetSphere(), sphere);
  }

  //--------------------------------------------------------------------------------
  CollisionManifold SphereCollider::CollidesWith(const OBBCollider& _box) const
  {
    return FindCollision(*_box.GetBox(), sphere);
  }

  //---------------------------------------------------------------------------------------
  void SphereCollider::SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation)
  {
    sphere.position = _pos;
  }

  //--------------------------------------------------
  glm::vec4 SphereCollider::GetTensor(RigidBody& _rb) const
  {
    float ix = 0.0f;
    float iy = 0.0f;
    float iz = 0.0f;
    float iw = 0.0f;
    if (_rb.GetMass() != 0)
    {
      float r2 = sphere.radius * sphere.radius;
      float fraction = (2.0f / 5.0f);
      ix = r2 * _rb.GetMass() * fraction;
      iy = r2 * _rb.GetMass() * fraction;
      iz = r2 * _rb.GetMass() * fraction;
      iw = 1.0f;
    }
    return glm::vec4(ix, iy, iz, iw);
  }

  CollisionManifold ICollider::FindCollisionFeatures(const ICollider& _A, const ICollider& _B)
  {
    return _A.Dispatch(_B);
  }
}