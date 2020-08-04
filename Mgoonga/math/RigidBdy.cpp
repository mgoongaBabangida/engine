#include "stdafx.h"
#include "RigidBdy.h"

#include "Transform.h"

//-------------------------------------------------------------------------------
void eRigidBody::SetObject(eObject * obj) { object = obj; }

bool eRigidBody::CollidesWith(eObject* _other, Side _side)
{
	collision.collider = object;
	collision.collidee = _other;
	return object->GetCollider()->CollidesWith(*(object->GetTransform()),
		*(_other->GetTransform()),
		*(_other->GetCollider()),
		_side,
		collision);
}

void eRigidBody::ReactCollision(const eCollision& _col)
{
	if (_col.collider->GetScript())_col.collider->GetScript()->CollisionCallback(_col);
	if (_col.collidee->GetScript())_col.collidee->GetScript()->CollisionCallback(_col);
}

void eRigidBody::MoveForward(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getForward();
	velocity = glm::normalize(dir) * (speed - thrsh);
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), FORWARD))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::MoveBack(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getForward();
	velocity = glm::normalize(-dir) * (speed - thrsh);
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), BACK))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::MoveRight(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getForward();
	velocity = glm::normalize(glm::cross(dir, YAXIS)) * (speed - thrsh);
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), RIGHT))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::MoveLeft(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getForward();
	velocity = glm::normalize(glm::cross(YAXIS, dir)) * (speed - thrsh);
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), LEFT))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::MoveUp(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getUp();
	velocity = glm::normalize(dir) * (speed - thrsh);
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), UP))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::MoveDown(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getUp();
	velocity = -glm::normalize(dir) * (speed - thrsh);
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), DOWN))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::Move(std::vector<std::shared_ptr<eObject>> objects)
{
	object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += velocity);
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), RIGHT) //$todo optimize
			|| CollidesWith(obj.get(), LEFT)
			|| CollidesWith(obj.get(), UP)
			|| CollidesWith(obj.get(), DOWN)
			|| CollidesWith(obj.get(), FORWARD)
			|| CollidesWith(obj.get(), BACK)))
		{
			object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() -= velocity);
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::Turn(glm::vec3 direction, std::vector<std::shared_ptr<eObject>> objects) // $todo include speed
{
	glm::vec3 forward = glm::mat3(object->GetTransform()->getModelMatrix()) * object->GetTransform()->getForward();
	float angle = glm::dot(forward, direction);
	glm::vec3 asix = glm::cross(forward, direction);
	glm::quat rotation;
	angle >= 0 ? rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, glm::acos(angle), asix))
		: rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, 2.0f * PI - (glm::acos(angle)), -asix));
	object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), RIGHT)
			|| CollidesWith(obj.get(), LEFT)
			|| CollidesWith(obj.get(), UP)
			|| CollidesWith(obj.get(), DOWN)))
		{
			angle >= 0 ? rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -glm::acos(angle), asix))
				: rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -(2.0f * PI - (glm::acos(angle))), -asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::TurnRight(std::vector<std::shared_ptr<eObject>> objects)
{
	// Conversion from axis-angle
	// In GLM the angle must be in degrees here, so convert it.
	glm::vec3 asix = glm::toMat4(object->GetTransform()->getRotation()) * glm::vec4(object->GetTransform()->getUp(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, turnSpeed, asix));
	//q_rotation = rotation * q_rotation;
	object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), RIGHT) || (CollidesWith(obj.get(), LEFT))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -turnSpeed, asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::TurnLeft(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(object->GetTransform()->getRotation()) * glm::vec4(object->GetTransform()->getUp(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -turnSpeed, asix));
	object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), RIGHT) || (CollidesWith(obj.get(), LEFT))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, turnSpeed, asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::LeanRight(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(object->GetTransform()->getRotation()) * glm::vec4(object->GetTransform()->getForward(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, turnSpeed, asix));
	object->GetTransform()->setRotation(glm::quat(rotation * object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), RIGHT)
			|| (CollidesWith(obj.get(), UP))
			|| (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -turnSpeed, asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::LeanLeft(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(object->GetTransform()->getRotation()) * glm::vec4(object->GetTransform()->getForward(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -turnSpeed, asix));
	object->GetTransform()->setRotation(glm::quat(rotation * object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), LEFT)
			|| (CollidesWith(obj.get(), UP))
			|| (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, turnSpeed, asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::LeanForward(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 Xasix = glm::cross(object->GetTransform()->getUp(), object->GetTransform()->getForward());
	glm::vec3 asix = glm::toMat4(object->GetTransform()->getRotation()) * glm::vec4(Xasix, 1.0f);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, turnSpeed, asix));
	object->GetTransform()->setRotation(glm::quat(rotation * object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), UP) || (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -turnSpeed, asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}

void eRigidBody::LeanBack(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 Xasix = glm::cross(object->GetTransform()->getUp(), object->GetTransform()->getForward());
	glm::vec3 asix = glm::toMat4(object->GetTransform()->getRotation()) * glm::vec4(Xasix, 1.0f);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -turnSpeed, asix));
	object->GetTransform()->setRotation(glm::quat(rotation * object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), UP) || (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, turnSpeed, asix));
			object->GetTransform()->setRotation(rotation * object->GetTransform()->getRotation());
			ReactCollision(collision);
			break;
		}
	}
}