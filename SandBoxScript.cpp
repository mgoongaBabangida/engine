#include "stdafx.h"
#include "SandBoxScript.h"
#include "RigidBody.h"

void eSandBoxScript::Update(std::vector<std::shared_ptr<eObject>> objs)
{
	if(!clock.isActive()) { clock.start(); }
	glm::vec3 velocity = -YAXIS * (g * clock.newFrame());
	object->getRigidBody()->SetCurrentVelocity(velocity);	//$need to improve design with RigidBody
	object->Move(objs);
}

void eSandBoxScript::CollisionCallback(const eCollision&)
{
	std::cout << "Collision!" << std::endl;
}
