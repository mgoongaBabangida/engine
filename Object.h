#pragma once

#include <memory>

#include "Transform.h"
#include "BoxCollider.h"
#include "InterfacesDB.h"
#include "Rigger.h"

class IModel;
class MovementAPI;

class eObject
{
public:
	eObject();
	eObject(IModel* m);
	virtual ~eObject();

	bool operator==(const eObject&);
	bool operator!=(const eObject&);

	//Setters
	void		 setRigger(Rigger* r)		{ rigger.reset(r); }
	void		 setScript(IScript* scr)	{ script.reset(scr); scr->setObject(this); }
	//Getters
	IScript*	 getScript()		const	{ return script.get();		}
	Transform*	 getTransform()		const	{ return transform.get();	}
	BoxCollider* getCollider()		const	{ return collider.get();	}
	IModel*		 getModel()			const	{ return m_model;			}
	Rigger*		 getRigger()		const	{ return rigger.get();		}

	virtual void TurnRight(std::vector<std::shared_ptr<eObject> > objects);
	virtual void TurnLeft(std::vector<std::shared_ptr<eObject> > objects);
	virtual void LeanRight(std::vector<std::shared_ptr<eObject> > objects);
	virtual void LeanLeft(std::vector<std::shared_ptr<eObject> > objects);
	virtual void LeanForward(std::vector<std::shared_ptr<eObject> > objects);
	virtual void LeanBack(std::vector<std::shared_ptr<eObject> > objects);

	virtual void MoveForward(std::vector<std::shared_ptr<eObject> > objects);
	virtual void MoveBack(std::vector<std::shared_ptr<eObject> > objects);
	virtual void MoveLeft(std::vector<std::shared_ptr<eObject> > objects);
	virtual void MoveRight(std::vector<std::shared_ptr<eObject> > objects);
	virtual void MoveUp(std::vector<std::shared_ptr<eObject> > objects);
	virtual void MoveDown(std::vector<std::shared_ptr<eObject> > objects);

	void		 Debug();

protected:
	IModel*							m_model;
	std::unique_ptr<IScript>		script;
	std::unique_ptr<Transform>		transform;
	std::unique_ptr<BoxCollider>	collider;
	std::unique_ptr<Rigger>			rigger;
	MovementAPI*					movementApi;
};