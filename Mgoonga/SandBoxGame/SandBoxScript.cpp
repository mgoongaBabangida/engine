#include "stdafx.h"

#include <iostream>

#include <base/Object.h>
#include "SandBoxScript.h"

void eSandBoxScript::Update(std::vector<std::shared_ptr<eObject>> objs)
{
	if(!clock.isActive()) { clock.start(); }
	glm::vec3 velocity = -YAXIS * (g * clock.newFrame());
	object->GetRigidBody()->SetCurrentVelocity(velocity);	//$need to improve design with RigidBody
	object->GetRigidBody()->Move(objs);
}

void eSandBoxScript::CollisionCallback(const eCollision&)
{
	std::cout << "Collision!" << std::endl;
}
