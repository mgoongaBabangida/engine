#include "stdafx.h"

#include <base/Object.h>
#include "SandBoxScript.h"

eSandBoxScript::eSandBoxScript(IGame* _game)
	: m_game(_game)
{
}

void eSandBoxScript::Update(float _tick)
{
	std::vector<std::shared_ptr<eObject>> objs = m_game->GetObjects();
	if(!clock.isActive())
		clock.start();

	glm::vec3 velocity = -YAXIS * (g * clock.newFrame());
	object->GetRigidBody()->SetCurrentVelocity(velocity);	//@todo need to improve design with RigidBody
	object->GetRigidBody()->Move(objs);
}

void eSandBoxScript::CollisionCallback(const eCollision&)
{
	//std::cout << "Collision!" << std::endl;
}
