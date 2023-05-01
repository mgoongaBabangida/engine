#include "stdafx.h"
#include "ObjectFactory.h"
#include <base/Object.h>
#include <math/Rigger.h>
#include <math/RigidBdy.h>
#include <math/BoxColliderDynamic.h>

std::unique_ptr<eObject> ObjectFactoryBase::CreateObject(std::shared_ptr<IModel> _model, const std::string& _name, bool _dynamic_collider)
{
  auto obj = std::make_unique<eObject>();
  obj->SetModel(_model);
  obj->SetTransform(new Transform);
  if(_dynamic_collider)
    obj->SetCollider(new BoxColliderDynamic);
  else
    obj->SetCollider(new BoxCollider);
  obj->SetRigidBody(new eRigidBody);
  obj->GetRigidBody()->SetObject(obj.get());
  obj->GetCollider()->CalculateExtremDots(obj.get());
  obj->SetName(_name);
  return obj;
}
