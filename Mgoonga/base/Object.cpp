#include "stdafx.h"
#include "Object.h"

//----------------------------------------------------------------------------
eObject::~eObject() = default;

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
void eObject::SetModel(std::shared_ptr<IModel> _m)
{
	model = _m;
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
