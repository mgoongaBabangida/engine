#include "stdafx.h"
#include "ObjectFactory.h"
#include <base/Object.h>
#include <math/Rigger.h>
#include <math/RigidBdy.h>
#include <math/BoxCollider.h>

std::unique_ptr<eObject> ObjectFactoryBase::CreateObject(std::shared_ptr<IModel> _model, const std::string& _name)
{
 auto obj = std::make_unique<eObject>();
  obj->SetModel(_model);
  obj->SetTransform(new Transform);
  obj->SetCollider(new BoxCollider);
  obj->SetRigidBody(new eRigidBody);
  obj->GetRigidBody()->SetObject(obj.get());
  obj->GetCollider()->CalculateExtremDots(obj->GetModel()->GetPositions());
  obj->SetName(_name);
  return obj;
}
