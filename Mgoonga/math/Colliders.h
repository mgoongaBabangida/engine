#pragma once

#include "math.h"
#include "Geometry.h"
#include "BoxCollider.h"

#include <base/base.h>

#include <optional>

namespace dbb
{
  //---------------------------------------------------------------------
  static CollisionManifold FindCollisionFeatures(const ICollider& _A, const ICollider& _B)
  {
    return _A.Dispatch(_B);
  }

  //@todo reimplement some functions from BoxCollider like Radius()
  //---------------------------------------------------------------------
  class DLL_MATH OBBCollider : public BoxCollider
  {
  public:
    OBBCollider(const dbb::OBB& _obb) : box(_obb) {}
    OBBCollider(const BoxCollider&, const ITransform& _trans);

    static OBBCollider* CreateFrom(const BoxCollider&, const ITransform&);

    dbb::OBB GetBox() const;

    virtual CollisionManifold       Dispatch(const ICollider& other) const override;
    virtual void                    SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4               GetTensor(float mass) const override;

    virtual dbb::point              GetCenter() const override;
    virtual glm::vec3               GetOrientation() const override;

    virtual void										SetFrom(const ITransform& trans) override;
    virtual void										SetTo(ITransform& trans) const override;

  protected:
    virtual CollisionManifold       CollidesWith(const SphereCollider& _other) const override;
    virtual CollisionManifold       CollidesWith(const OBBCollider& _other) const override;
    dbb::OBB box;
  };

  //---------------------------------------------------------------------
  class DLL_MATH SphereCollider : public BoxCollider
  {
  public:
    SphereCollider(const dbb::sphere& _sphere) : sphere(_sphere) {}
    SphereCollider(const BoxCollider&, const ITransform& _trans);

    static SphereCollider* CreateFrom(const BoxCollider&, const ITransform&);

    dbb::sphere GetSphere() const;

    virtual void                        SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4                   GetTensor(float mass) const override;

    virtual dbb::point                  GetCenter() const override;
    virtual glm::vec3                   GetOrientation() const override;

    virtual void										    SetFrom(const ITransform& trans) override;
    virtual void										    SetTo(ITransform& trans) const override;

    virtual CollisionManifold Dispatch(const ICollider& other) const override;
  protected:
    virtual CollisionManifold CollidesWith(const SphereCollider& _other) const override;
    virtual CollisionManifold CollidesWith(const OBBCollider& _other) const override;
    dbb::sphere sphere;
  };

  //---------------------------------------------------------------------
  class DLL_MATH EllipseCollider : public BoxCollider
  {
    EllipseCollider(const dbb::ellipse& _ellipse) : ellipse(_ellipse) {}
    EllipseCollider(const BoxCollider&, const ITransform& _trans);

    static EllipseCollider* CreateFrom(const BoxCollider&, const ITransform&);

    dbb::ellipse GetEllipse() const;

    virtual void                        SynchCollisionVolumes(const glm::vec3& _pos, const glm::vec3& _orientation) override;
    virtual glm::vec4                   GetTensor(float mass) const override;

    virtual dbb::point                  GetCenter() const override;
    virtual glm::vec3                   GetOrientation() const override;

    virtual void										    SetFrom(const ITransform& trans) override;
    virtual void										    SetTo(ITransform& trans) const override;

    virtual CollisionManifold Dispatch(const ICollider& other) const override;
  protected:
    virtual CollisionManifold CollidesWith(const SphereCollider& _other) const override;
    virtual CollisionManifold CollidesWith(const OBBCollider& _other) const override;
    virtual CollisionManifold CollidesWith(const EllipseCollider& _other) const override;

    dbb::ellipse ellipse;
  };
}