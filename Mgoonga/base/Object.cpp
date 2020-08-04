#include "stdafx.h"
#include "Object.h"

#include "interfaces.h"

//#include "RigidBody.h"
//#include "BoxCollider.h"

//----------------------------------------------------------------------------
//eObject::eObject() 
//{
//	transform.reset(new Transform);
//	transform->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); // transfer to transform
//	collider.reset(new BoxCollider);
//	movementApi.reset(new eRigidBody);
//	movementApi->SetObject(this);
//
//	script.reset(nullptr);
//	rigger.reset(nullptr);
//}

//----------------------------------------------------------------------------
//eObject::eObject(IModel* m, const std::string& _name) 
//	: model(m), 
//	  name(_name)
//{ 
//	transform.reset(new Transform);
//	transform->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
//	collider.reset(new BoxCollider);
//	collider->CalculateExtremDots(model->GetPositions());
//	movementApi.reset(new eRigidBody);
//	movementApi->SetObject(this);
//
//	script.reset(nullptr);
//	script.reset(nullptr);
//	rigger.reset(nullptr);
//}

//----------------------------------------------------------------------------
eObject::~eObject()
{}

//----------------------------------------------------------------------------
bool eObject::operator==(const eObject& _other)
{
	return GetTransform()->getTranslation() == _other.GetTransform()->getTranslation() 
			&& GetModel() == _other.GetModel();
}

//----------------------------------------------------------------------------
bool eObject::operator!=(const eObject& _other)
{
	return !operator==(_other);
}

//---------------------------------------------------------------------------
void eObject::SetRigger(IRigger * _r)
{
	rigger.reset(_r);
}

//----------------------------------------------------------------------------
void eObject::SetScript(IScript* _scr)
{
	 script.reset(_scr); 
	 _scr->SetObject(this);
}

//-----------------------------------------------------------------------------
void eObject::SetTransform(ITransform* _t)
{
	transform.reset(_t);
}

//-----------------------------------------------------------------------------
void eObject::SetCollider(ICollider* _c)
{
	collider.reset(_c);
}

//-----------------------------------------------------------------------------
void eObject::SetModel(IModel* _m)
{
	model.reset(_m);
}

//-----------------------------------------------------------------------------
void eObject::SetRigidBody(IRigidBody* _rb)
{
	movementApi.reset(_rb);
}

IScript*			eObject::GetScript()	const { return script.get(); }
ITransform*			eObject::GetTransform()	const { return transform.get();	}
ICollider*			eObject::GetCollider()	const { return collider.get(); }
IModel*				eObject::GetModel()		const { return model.get(); }
IRigger*			eObject::GetRigger()	const { return rigger.get(); }
