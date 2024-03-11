#include "RigidBodyDBB.h"

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>
#include <glm\glm\gtx\euler_angles.hpp>

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

    glm::vec3 r1 = M.contacts[c] - A.m_position;
    glm::vec3 r2 = M.contacts[c] - B.m_position;
    glm::mat4 i1 = A.InvTensor();
    glm::mat4 i2 = B.InvTensor();
    // Relative velocity
    glm::vec3 relativeVel = (B.m_velocity + glm::cross(B.m_angVel, r2)) - (A.m_velocity + glm::cross(A.m_angVel, r1));

    // Relative collision normal
    glm::vec3 relativeNorm = M.normal;
    glm::normalize(relativeNorm);

    // Moving away from each other? Do nothing!
    if (glm::dot(relativeVel, relativeNorm) > 0.0f)
      return;

    float e = fminf(A.m_cor, B.m_cor);
    float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));
    float d1 = invMassSum;
    glm::vec3 d2 = glm::cross(glm::cross(r1, relativeNorm) * glm::mat3(i1), r1); // mat3 ?
    glm::vec3 d3 = glm::cross(glm::cross(r2, relativeNorm) * glm::mat3(i2), r2);// mat3 ?
    float denominator = d1 + glm::dot(relativeNorm, d2 + d3);
    float j = (denominator == 0.0f) ? 0.0f :
      numerator / denominator;
    if (M.contacts.size() > 0.0f && j != 0.0f) {
      j /= (float)M.contacts.size();
    }

    glm::vec3 impulse = relativeNorm * j;
    A.m_velocity = A.m_velocity - impulse * invMass1;
    B.m_velocity = B.m_velocity + impulse * invMass2;
    A.m_angVel = A.m_angVel - (glm::cross(r1, impulse) * glm::mat3(i1));// mat3 ?
    B.m_angVel = B.m_angVel + (glm::cross(r2, impulse) * glm::mat3(i2));// mat3 ?

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

    float jt = numerator / denominator;
    if (M.contacts.size() > 0.0f && jt != 0.0f) {
      jt /= (float)M.contacts.size();
    }

    if (jt == 0.0f)
      return;

    float friction = sqrtf(A.m_friction * B.m_friction);
    if (jt > j * friction) {
      jt = j * friction;
    }
    else if (jt < -j * friction) {
      jt = -j * friction;
    }

    glm::vec3 tangentImpuse = t * jt;
    A.m_velocity = A.m_velocity - tangentImpuse * invMass1;
    B.m_velocity = B.m_velocity + tangentImpuse * invMass2;
    A.m_angVel = A.m_angVel - (glm::cross(r1, tangentImpuse) * glm::mat3(i1));
    B.m_angVel = B.m_angVel + (glm::cross(r2, tangentImpuse) * glm::mat3(i2));

    //float e = fminf(A.m_cor, B.m_cor);
    //float numerator = (-(1.0f + e) * glm::dot(relativeVel, relativeNorm));
    //float j = numerator / invMassSum;
    //if (M.contacts.size() > 0.0f && j != 0.0f) {
    //  j /= (float)M.contacts.size();
    //}
    //glm::vec3 impulse = relativeNorm * j;
    //A.m_velocity = A.GetVelocity() - impulse * invMass1; // modify old vel?
    //B.m_velocity = B.GetVelocity() + impulse * invMass2;
    //// Friction
    //glm::vec3 t = relativeVel - (relativeNorm * glm::dot(relativeVel, relativeNorm));
    //if (glm::length2(t) == 0.0f)
    //  return;

    //glm::normalize(t);

    //numerator = -glm::dot(relativeVel, t);
    //float jt = numerator / invMassSum;
    //if (M.contacts.size() > 0.0f && jt != 0.0f)
    //  jt /= (float)M.contacts.size();
    //if (jt == 0.0f)
    //  return;
    ////Coulomb's Law
    //float friction = sqrtf(A.m_friction * B.m_friction);
    //if (jt > j * friction)
    //  jt = j * friction;
    //else if (jt < -j * friction)
    //  jt = -j * friction;
    //glm::vec3 tangentImpuse = t * jt;
    //A.m_velocity = A.m_velocity - tangentImpuse * invMass1;
    //B.m_velocity = B.m_velocity + tangentImpuse * invMass2;
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
      /*if (type == RIGIDBODY_TYPE_BOX) {*/
        glm::vec3 angAccel = glm::vec4(m_torques,1.0f) * InvTensor();
        m_angVel = m_angVel + angAccel * _deltaTime;
        m_angVel = m_angVel * damping;

      m_position = m_position + m_velocity * _deltaTime;
      /*if (type == RIGIDBODY_TYPE_BOX) {*/
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
    m_forces = g_gravity * m_mass;
  }

  //---------------------------------------------------------------------
  void RigidBody::SolveConstraints(const std::vector<OBB>& _constraints)
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
      m_collider->SynchCollisionVolumes(m_position, m_orientation);
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
    glm::vec3 angAccel = glm::vec4(torque,1.0f) * InvTensor();
    m_angVel = m_angVel + angAccel;
  }

  //---------------------------------------------------------------------------------------
  void Collider::SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation)
  {
    box.origin = _pos;
    sphere.position = _pos;
    box.orientation = glm::eulerAngleYXZ(_orientation.y , _orientation.x, _orientation.z);
  }

  //--------------------------------------------------
  glm::vec4 Collider::GetTensor(RigidBody& _rb) const
  {
    float ix = 0.0f;
    float iy = 0.0f;
    float iz = 0.0f;
    float iw = 0.0f;
    if (_rb.GetMass() != 0 /*&& type == RIGIDBODY_TYPE_SPHERE*/)
    {
      float r2 = sphere.radius * sphere.radius;
      float fraction = (2.0f / 5.0f);
      ix = r2 * _rb.GetMass() * fraction;
      iy = r2 * _rb.GetMass() * fraction;
      iz = r2 * _rb.GetMass() * fraction;
      iw = 1.0f;
    }
    else if (_rb.GetMass() != 0 /*&& type == RIGIDBODY_TYPE_BOX*/) {
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
}