#pragma once

#include "math.h"
#include <base/Object.h>

class DLL_MATH eRigidBody : public IRigidBody
{
public:
	void SetObject(eObject* obj);
	
	virtual void TurnRight(std::vector<shObject > objects);
	virtual void TurnLeft(std::vector<shObject > objects);
	virtual void LeanRight(std::vector<shObject > objects);
	virtual void LeanLeft(std::vector<shObject > objects);
	virtual void LeanForward(std::vector<shObject > objects);
	virtual void LeanBack(std::vector<shObject > objects);

	virtual void MoveForward(std::vector<shObject> objects);
	virtual void MoveBack(std::vector<shObject> objects);
	virtual void MoveLeft(std::vector<shObject> objects);
	virtual void MoveRight(std::vector<shObject> objects);
	virtual void MoveUp(std::vector<shObject> objects);
	virtual void MoveDown(std::vector<std::shared_ptr<eObject> > objects);

	virtual void Move(std::vector<shObject> objects);
	virtual void Turn(glm::vec3 direction, std::vector<shObject> objects);

	void		 SetCurrentVelocity(glm::vec3 _vel) { velocity = _vel; }
	glm::vec3	 Velocity()	const { return velocity; }

protected:
	bool		CollidesWith(eObject*, Side);
	void		ReactCollision(const eCollision& col);

	eObject*	object = nullptr;
	float		turnSpeed = PI / 12.0f;
	float		speed = 0.01f;
	glm::vec3	velocity = NONE;
	eCollision	collision;
};
