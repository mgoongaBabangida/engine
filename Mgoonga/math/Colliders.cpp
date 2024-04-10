
#include "Colliders.h"
#include "GeometryFunctions.h"

#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>
#include <glm\glm\gtx\euler_angles.hpp>

namespace dbb
{
  //---------------------------------------------------------------------------------------
  void OBBCollider::SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation)
  {
    box.origin = _pos;
    box.orientation = glm::eulerAngleYXZ(_orientation.y, _orientation.x, _orientation.z);
  }

  //--------------------------------------------------
  glm::vec4 OBBCollider::GetTensor(float _mass) const
  {
    float ix = 0.0f;
    float iy = 0.0f;
    float iz = 0.0f;
    float iw = 0.0f;
    if (_mass != 0)
    {
      glm::vec3 size = box.size * 2.0f;
      float fraction = (1.0f / 12.0f);
      float x2 = size.x * size.x;
      float y2 = size.y * size.y;
      float z2 = size.z * size.z;
      ix = (y2 + z2) * _mass * fraction;
      iy = (x2 + z2) * _mass * fraction;
      iz = (x2 + y2) * _mass * fraction;
      iw = 1.0f;
    }
    return glm::vec4(ix, iy, iz, iw);
  }

  //---------------------------------------------------------------------------------------
  OBBCollider::OBBCollider(const BoxCollider& _base, const ITransform& _trans)
    : BoxCollider(_base)
    , box(_base._GetOBB(_trans))
  {
  }

  //---------------------------------------------------------------------------------------
  OBBCollider* OBBCollider::CreateFrom(const BoxCollider& _base, const ITransform& _trans)
  {
    return new OBBCollider(_base, _trans);
  }

  //---------------------------------------------------------------------------------------
  dbb::OBB OBBCollider::GetBox() const
  {
    return box;
  }

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
  void OBBCollider::SetFrom(const ITransform& _trans)
  {
    box = _GetOBB(_trans);
  }

  //---------------------------------------------------------------------------------------
  void OBBCollider::SetTo(ITransform& _trans) const
  {
    //dbb::OBB c1 = cube1.second->GetCollider()->GetBox().value();
    //cube1.first->GetTransform()->setRotation(glm::toQuat(c1.orientation)); //works incorrectly!
    //cube1.first->GetTransform()->setTranslation(c1.origin - (cube1.first->GetTransform()->getRotation() * cube1.first->GetCollider()->GetCenter()));

    _trans.setRotation(glm::toQuat(box.orientation)); //@todo works incorrectly!!!!!!
    _trans.setTranslation(box.origin - (_trans.getRotation() * BoxCollider::GetCenter())); //!?
  }

  //---------------------------------------------------------------------------------------
  SphereCollider::SphereCollider(const BoxCollider& _base, const ITransform& _trans)
    : BoxCollider(_base)
    , sphere(_base._GetSphere(_trans))
  {
  }

  //---------------------------------------------------------------------------------------
  SphereCollider* SphereCollider::CreateFrom(const BoxCollider& _base, const ITransform& _trans)
  {
    return new SphereCollider(_base, _trans);
  }

  //---------------------------------------------------------------------------------------
  dbb::sphere SphereCollider::GetSphere() const
  {
    return sphere;
  }

  //---------------------------------------------------------------------------------------
  dbb::point SphereCollider::GetCenter() const
  {
    return sphere.position;
  }

  // -------------------------------------------------------------------------------------- -
  glm::vec3 SphereCollider::GetOrientation() const
  {
    return { 1,1,1 };
  }

  //---------------------------------------------------------------------------------------
  void SphereCollider::SetFrom(const ITransform& _trans)
  {
    sphere = _GetSphere(_trans);
  }

  //---------------------------------------------------------------------------------------
  void SphereCollider::SetTo(ITransform& _trans) const
  {
    //dbb::sphere s1 = sphere1.second->GetCollider()->GetSphere().value();
    //sphere1.first->GetTransform()->setTranslation(s1.position - sphere1.first->GetCollider()->GetCenter());
    _trans.setTranslation(sphere.position - BoxCollider::GetCenter());
  }

  //---------------------------------------------------------------------------------------
  CollisionManifold OBBCollider::CollidesWith(const SphereCollider& _sphere) const
  {
    CollisionManifold cm = FindCollision(box, _sphere.GetSphere()); //@todo check optional
    cm.normal = -cm.normal;
    return cm;
  }

  //---------------------------------------------------------------------------------------
  CollisionManifold OBBCollider::CollidesWith(const OBBCollider& _box) const
  {
    return FindCollision(_box.GetBox(), box);
  }

  //--------------------------------------------------------------------------------
  CollisionManifold SphereCollider::CollidesWith(const SphereCollider& _sphere) const
  {
    return FindCollision(_sphere.GetSphere(), sphere);
  }

  //--------------------------------------------------------------------------------
  CollisionManifold SphereCollider::CollidesWith(const OBBCollider& _box) const
  {
    return FindCollision(_box.GetBox(), sphere);
  }

  //---------------------------------------------------------------------------------------
  void SphereCollider::SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation)
  {
    sphere.position = _pos;
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

  //--------------------------------------------------
  glm::vec4 SphereCollider::GetTensor(float _mass) const
  {
    float ix = 0.0f;
    float iy = 0.0f;
    float iz = 0.0f;
    float iw = 0.0f;
    if (_mass != 0)
    {
      float r2 = sphere.radius * sphere.radius;
      float fraction = (2.0f / 5.0f);
      ix = r2 * _mass * fraction;
      iy = r2 * _mass * fraction;
      iz = r2 * _mass * fraction;
      iw = 1.0f;
    }
    return glm::vec4(ix, iy, iz, iw);
  }
}