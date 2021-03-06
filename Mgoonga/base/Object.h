#pragma once

#include <memory>
#include <vector>
#include "interfaces.h"

struct IModelDeleter
{
  void operator()(IModel *p);
};

//---------------------------------------------------------------------------------
class DLL_BASE eObject
{
public:
	explicit eObject() {}
	virtual ~eObject();

	bool operator==(const eObject&);
	bool operator!=(const eObject&);

	//Setters
	void				SetRigger(IRigger* r);
	void				SetScript(IScript* scr);
	void				SetTransform(ITransform*);
	void				SetCollider(ICollider*);
	void				SetModel(IModel*);
	void				SetRigidBody(IRigidBody*);
	void 				SetName(const std::string& _name) { name = _name; }

	//Getters
	IScript*			GetScript()		const;
	ITransform*			GetTransform()	const;
	ICollider*			GetCollider()	const;
	IModel*				GetModel()		const;
	IRigger*			GetRigger()		const;
	IRigidBody*			GetRigidBody()	const	{ return movementApi.get(); }
	const std::string&	Name()			const	{ return name;				}

protected:
	std::unique_ptr<IModel, IModelDeleter>			model;
	std::unique_ptr<IScript>		script;
	std::unique_ptr<ITransform>		transform;
	std::unique_ptr<ICollider>		collider;
	std::unique_ptr<IRigger>		rigger;
	std::unique_ptr<IRigidBody>		movementApi;
	std::string						name;
};

using shObject = std::shared_ptr<eObject>;