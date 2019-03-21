#pragma once

#include "InterfacesDB.h"
#include "Structures.h"
#include "Object.h"

class MovementAPI
{
public:
	void setObject(eObject* obj) { object = obj; }

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

protected:
	bool CollidesWith(eObject*, Side);

	eObject* object		= nullptr;
	float	 turnSpeed	= 3.14f / 12.0f;
	float	 SPEED		= 0.1f;
};
