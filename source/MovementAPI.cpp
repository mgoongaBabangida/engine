#include "MovementAPI.h"

bool MovementAPI::CollidesWith(eObject* _other, Side _side)
{
	return object->getCollider()->collidesWith(*(object->getTransform()), 
											   *(_other->getTransform()),
											   *(_other->getCollider()), _side);
}

void MovementAPI::MoveForward(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->getTransform()->getModelMatrix()) * object->getTransform()->getForward();
	object->getTransform()->setTranslation(object->getTransform()->getTranslation() += glm::normalize(dir) * (SPEED - thrsh));
	for (auto& obj : objects) 
	{
		if(*object != *obj && CollidesWith(obj.get(), FORWARD))
		{
			object->getTransform()->setTranslation(object->getTransform()->getTranslation() -= glm::normalize(dir) * (SPEED - thrsh));
			break;
		}
	}
}

void MovementAPI::MoveBack(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->getTransform()->getModelMatrix()) * object->getTransform()->getForward();
	object->getTransform()->setTranslation(object->getTransform()->getTranslation() += glm::normalize(-dir) * (SPEED - thrsh));
	for (auto& obj : objects)
	{
		if(*object != *obj && CollidesWith(obj.get(), BACK))
		{
			object->getTransform()->setTranslation(object->getTransform()->getTranslation() -= glm::normalize(-dir) * (SPEED - thrsh));
			break;
		}
	}
}

void MovementAPI::MoveRight(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->getTransform()->getModelMatrix()) * object->getTransform()->getForward();
	object->getTransform()->setTranslation(object->getTransform()->getTranslation() += glm::normalize(glm::cross(dir, YAXIS)) * (SPEED - thrsh));
	for (auto& obj : objects)
	{
		if(*object != *obj && CollidesWith(obj.get(), RIGHT))
		{
			object->getTransform()->setTranslation(object->getTransform()->getTranslation() -= glm::normalize(glm::cross(dir, YAXIS))* (SPEED - thrsh));
			break;
		}
	}
}

void MovementAPI::MoveLeft(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->getTransform()->getModelMatrix()) * object->getTransform()->getForward();
	object->getTransform()->setTranslation(object->getTransform()->getTranslation() += glm::normalize(glm::cross(YAXIS, dir)) * (SPEED - thrsh));
	for (auto& obj : objects)
	{
		if(*object != *obj && CollidesWith(obj.get(), LEFT))
		{
			object->getTransform()->setTranslation(object->getTransform()->getTranslation() -= glm::normalize(glm::cross(YAXIS, dir)) * (SPEED - thrsh));
			break;
		}
	}
}

void MovementAPI::MoveUp(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->getTransform()->getModelMatrix()) * object->getTransform()->getUp();
	object->getTransform()->setTranslation(object->getTransform()->getTranslation() += glm::normalize(dir) * (SPEED - thrsh));
	for (auto& obj : objects)
	{
		if(*object != *obj && CollidesWith(obj.get(), UP))
		{
			object->getTransform()->setTranslation(object->getTransform()->getTranslation() -= glm::normalize(dir) * (SPEED - thrsh));
			break;
		}
	}
}
void MovementAPI::MoveDown(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(object->getTransform()->getModelMatrix()) * object->getTransform()->getUp();
	object->getTransform()->setTranslation(object->getTransform()->getTranslation() -= glm::normalize(dir) * (SPEED - thrsh));
	for (auto& obj : objects)
	{
		if (*object != *obj && CollidesWith(obj.get(), DOWN))
		{
			object->getTransform()->setTranslation(obj->getTransform()->getTranslation() += glm::normalize(dir) * (SPEED - thrsh));
			break;
		}
	}
}

void MovementAPI::TurnRight(std::vector<std::shared_ptr<eObject>> objects)
{
	// Conversion from axis-angle
	// In GLM the angle must be in degrees here, so convert it.
	glm::vec3 asix = glm::toMat4(object->getTransform()->getRotation()) * glm::vec4(object->getTransform()->getUp(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
	//q_rotation = rotation * q_rotation;
	object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
	for (auto& obj : objects)
	{
		if(*object != *obj && (CollidesWith(obj.get(), RIGHT) || (CollidesWith(obj.get(), LEFT))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
			object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
			break;
		}
	}
}

void MovementAPI::TurnLeft(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(object->getTransform()->getRotation()) * glm::vec4(object->getTransform()->getUp(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
	object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
	for (auto& obj : objects)
	{
		if(*object != *obj && (CollidesWith(obj.get(), RIGHT) || (CollidesWith(obj.get(), LEFT))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
			object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
			break;
		}
	}
}

void MovementAPI::LeanRight(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(object->getTransform()->getRotation()) * glm::vec4(object->getTransform()->getForward(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
	object->getTransform()->setRotation(glm::quat(rotation * object->getTransform()->getRotation()));
	for (auto& obj : objects) 
	{
		if (*object != *obj && (CollidesWith(obj.get(), RIGHT) 
								|| (CollidesWith(obj.get(), UP))
								|| (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
			object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
			break;
		}
	}
}

void MovementAPI::LeanLeft(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(object->getTransform()->getRotation()) * glm::vec4(object->getTransform()->getForward(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
	object->getTransform()->setRotation(glm::quat(rotation * object->getTransform()->getRotation()));
	for (auto& obj : objects) 
	{
		if(*object != *obj && (CollidesWith(obj.get(), LEFT)
			|| (CollidesWith(obj.get(), UP))
			|| (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
			object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
			break;
		}
	}
}

void MovementAPI::LeanForward(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 Xasix = glm::cross(object->getTransform()->getUp(), object->getTransform()->getForward());
	glm::vec3 asix = glm::toMat4(object->getTransform()->getRotation()) * glm::vec4(Xasix, 1.0f);
	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
	object->getTransform()->setRotation(glm::quat(rotation * object->getTransform()->getRotation()));
	for (auto& obj : objects) 
	{
		if(*object != *obj && (CollidesWith(obj.get(), UP) || (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
			object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
			break;
		}
	}
}

void MovementAPI::LeanBack(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 Xasix = glm::cross(object->getTransform()->getUp(), object->getTransform()->getForward());
	glm::vec3 asix = glm::toMat4(object->getTransform()->getRotation()) * glm::vec4(Xasix, 1.0f);
	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
	object->getTransform()->setRotation(glm::quat(rotation * object->getTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*object != *obj && (CollidesWith(obj.get(), UP) || (CollidesWith(obj.get(), DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
			object->getTransform()->setRotation(rotation * object->getTransform()->getRotation());
			break;
		}
	}
}