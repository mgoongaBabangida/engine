#include "stdafx.h"
#include "ObjectFactory.h"

#include <math/Rigger.h>
#include <math/RigidBody.h>
#include <math/BoxColliderDynamic.h>
#include <math/AnimatedModel.h>

#include <game_assets/AnimationManagerYAML.h>
#include <game_assets/DynamicBoundingBoxColliderSerializerYAML.h>

//----------------------------------------------------------------------------------
std::unique_ptr<eObject> ObjectFactoryBase::CreateObject(std::shared_ptr<IModel> _model,
                                                         eObject::RenderType _render_type,
                                                         const std::string& _name)
{
  auto obj = std::make_unique<eObject>();
  obj->SetModel(_model);
  obj->SetRenderType(_render_type);
  obj->SetTransform(new Transform);
  if (!_model->Get3DMeshes().empty())
  {
    obj->SetCollider(new BoxCollider);
    obj->SetRigidBody(new eRigidBody);
    obj->GetRigidBody()->SetObject(obj.get());
    obj->GetCollider()->CalculateExtremDots(obj.get());
  }
  obj->SetName(_name);
  return obj;
}

//----------------------------------------------------------------------------------
std::unique_ptr<eObject> ObjectFactoryBase::CreateObject(std::shared_ptr<IModel> _model,
                                                         eObject::RenderType _render_type,
                                                         const std::string& _name,
                                                         const std::string& _rigger_path,
                                                         const std::string& _collider_path,
                                                         bool _dynamic_collider)
{
  std::unique_ptr<eObject> obj = CreateObject(_model, _render_type, _name);
  if (m_animationManager)
  {
    if (_rigger_path == "Default")
    {
      obj->SetRigger(new Rigger(dynamic_cast<eAnimatedModel*>(_model.get())));
      //obj->GetRigger()->ChangeName(std::string(), "Running");//@todo improve
    }
    else
    {
      IRigger* rigger = m_animationManager->DeserializeRigger(_rigger_path);
      obj->SetRigger(rigger);
    }

    if (_dynamic_collider)
    {
      if (_collider_path.empty())
      {
        obj->SetCollider(new BoxColliderDynamic);
        //DynamicBoundingBoxColliderSerializerYAML boxSerializer;
        //boxSerializer.Serialize(dynamic_cast<BoxColliderDynamic*>(obj->GetCollider()), "Soldier3Anim.mgoongaBoxColliderDynamic");
      }
      else
      {
        DynamicBoundingBoxColliderSerializerYAML boxSerializer;
        obj->SetCollider(boxSerializer.Deserialize(_collider_path));
      }
      obj->GetCollider()->CalculateExtremDots(obj.get()); // always needs this to set up rigger
    }
  }
  return obj;
}