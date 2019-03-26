#pragma once

#include "InterfacesDB.h"
#include "Structures.h"
#include "Object.h"

class eRigidBody
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

	virtual void Move(std::vector<std::shared_ptr<eObject> > objects);
	virtual void Turn(glm::vec3 direction, std::vector<std::shared_ptr<eObject>> objects);
	glm::vec3	 Velocity()	const { return velocity; }
protected:
	bool		CollidesWith(eObject*, Side);
	void		ReactCollision(const eCollision& col) {}

	eObject*	object		= nullptr;
	float		turnSpeed	= PI / 12.0f;
	float		speed		= 0.1f;
	glm::vec3	velocity	= NONE;
	eCollision	collision;
};
