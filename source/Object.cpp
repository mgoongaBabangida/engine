#include "stdafx.h"
#include "Object.h"
#include "MyModel.h"
#include "RigidBody.h"
#include "InterfacesDB.h"

eObject::eObject() 
{
	transform.reset(new Transform);
	collider.reset(new BoxCollider);
	transform->setScale(glm::vec3(1.0f, 1.0f, 1.0f)); // transfer to transform
	script.reset(nullptr);
	rigger.reset(nullptr);
	movementApi = new eRigidBody; // ?
	movementApi->setObject(this);
}

eObject::eObject(IModel * m, const std::string& _name) :m_model(m), name(_name)
{ 
	transform.reset(new Transform);
	collider.reset(new BoxCollider);
	script.reset(nullptr);
	transform->setScale(glm::vec3(1.0f, 1.0f, 1.0f));
	script.reset(nullptr);
	rigger.reset(nullptr);
	movementApi = new eRigidBody; // ?
	movementApi->setObject(this);
	collider->calculateExtremDots(m_model->getPositions());
}

eObject::~eObject()
{
	delete movementApi;
}

bool eObject::operator==(const eObject& _other)
{
	return getTransform()->getTranslation() == _other.getTransform()->getTranslation() 
			&& getModel() == _other.getModel();
}

bool eObject::operator!=(const eObject& _other)
{
	return !operator==(_other);
}

void eObject::setScript(IScript* scr)
{
	 script.reset(scr); scr->setObject(this);
}

void eObject::TurnRight(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->TurnRight(objects);
}
void eObject::TurnLeft(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->TurnLeft(objects);
}
void eObject::LeanRight(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->LeanRight(objects);
}
void eObject::LeanLeft(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->LeanLeft(objects);
}
void eObject::LeanForward(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->LeanForward(objects);
}
void eObject::LeanBack(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->LeanBack(objects);
}

void eObject::MoveForward(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->MoveForward(objects);
}
void eObject::MoveBack(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->MoveBack(objects);
}
void eObject::MoveLeft(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->MoveLeft(objects);
}
void eObject::MoveRight(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->MoveRight(objects);
}
void eObject::MoveUp(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->MoveUp(objects);
}
void eObject::MoveDown(std::vector<std::shared_ptr<eObject> > objects)
{
	movementApi->MoveDown(objects);
}

void eObject::Debug()
{
	
}


//
//void eObject::MoveForward(std::vector<std::shared_ptr<eObject> > objects)
//{
//	float thrsh = 0.001f;
//	glm::vec3 dir = glm::mat3(transform->getModelMatrix()) * transform->getForward();
//	transform->setTranslation(transform->getTranslation() += glm::normalize(dir) * (speed - thrsh));
//	for (auto& obj : objects) {
//		if(this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if(collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), FORWARD)) 
//			{
//				transform->setTranslation(transform->getTranslation() -= glm::normalize(dir) * (speed - thrsh));
//				break;
//			}
//		}
//	}
//}
//
//void eObject::MoveBack(std::vector<std::shared_ptr<eObject> > objects)
//{
//	float thrsh = 0.001f;
//	glm::vec3 dir = glm::mat3(transform->getModelMatrix()) * transform->getForward();
//	transform->setTranslation(transform->getTranslation() += glm::normalize(-dir) * (speed - thrsh));
//	for (auto& obj : objects) 
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), BACK))
//			{
//				transform->setTranslation(transform->getTranslation() -= glm::normalize(-dir) * (speed - thrsh));
//				break;
//			}
//		}
//	}
//}
//
//void eObject::MoveRight(std::vector<std::shared_ptr<eObject> > objects)
//{
//	float thrsh = 0.001f;
//	glm::vec3 dir = glm::mat3(transform->getModelMatrix()) * transform->getForward();
//	transform->setTranslation(transform->getTranslation() += glm::normalize(glm::cross(dir, YAXIS))* (speed - thrsh));
//	for (auto& obj : objects) 
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), RIGHT)) 
//			{
//				transform->setTranslation(transform->getTranslation() -= glm::normalize(glm::cross(dir, YAXIS))* (speed - thrsh));
//				break;
//			}
//		}
//	}
//}
//
//void eObject::MoveLeft(std::vector<std::shared_ptr<eObject> > objects)
//{
//	float thrsh = 0.001f;
//	glm::vec3 dir = glm::mat3(transform->getModelMatrix()) * transform->getForward();
//	transform->setTranslation(transform->getTranslation() += glm::normalize(glm::cross(YAXIS, dir)) * (speed - thrsh));
//	for (auto& obj : objects) 
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), LEFT)) 
//			{
//				transform->setTranslation(transform->getTranslation() -= glm::normalize(glm::cross(YAXIS, dir)) * (speed - thrsh));
//				break;
//			}
//		}
//	}
//}
//
//void eObject::MoveUp(std::vector<std::shared_ptr<eObject> > objects)
//{
//	float thrsh = 0.001f;
//	glm::vec3 dir = glm::mat3(transform->getModelMatrix()) * transform->getUp();
//	transform->setTranslation(transform->getTranslation() += glm::normalize(dir) * (speed - thrsh));
//	for (auto& obj : objects) 
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), UP))
//			{
//				transform->setTranslation(transform->getTranslation() -= glm::normalize(dir) * (speed - thrsh));
//				break;
//			}
//		}
//	}
//}
//
//void eObject::MoveDown(std::vector<std::shared_ptr<eObject> > objects)
//{
//	float thrsh = 0.001f;
//	glm::vec3 dir = glm::mat3(transform->getModelMatrix()) * transform->getUp();
//	transform->setTranslation(transform->getTranslation() -= glm::normalize(dir) * (speed - thrsh));
//	for (auto& obj : objects)
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), DOWN))
//			{
//				transform->setTranslation(transform->getTranslation() += glm::normalize(dir) * (speed - thrsh));
//				break;
//			}
//		}
//	}
//}
//
//void eObject::TurnRight(std::vector<std::shared_ptr<eObject>> objects)
//{
//	// Conversion from axis-angle
//	// In GLM the angle must be in degrees here, so convert it.
//	glm::vec3 asix = glm::toMat4(transform->getRotation()) * glm::vec4(transform->getUp() ,1.0f);
//	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
//	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed , asix)); 
//	//q_rotation = rotation * q_rotation;
//	transform->setRotation(rotation * transform->getRotation());
//	for (auto& obj : objects) 
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), LEFT) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), RIGHT) )
//			{
//					glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix)); 
//					transform->setRotation(rotation * transform->getRotation());
//					break;
//			}
//		}
//	}
//}
//
//void eObject::TurnLeft(std::vector<std::shared_ptr<eObject>> objects)
//{
//	glm::vec3 asix = glm::toMat4(transform->getRotation()) * glm::vec4(transform->getUp(), 1.0f);
//	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
//	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed , asix)); 
//	transform->setRotation(rotation * transform->getRotation());
//	for (auto& obj : objects)
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), LEFT) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), RIGHT) ) 
//			{
//				glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
//				transform->setRotation(rotation * transform->getRotation());
//				break;
//			}
//		}
//	}
//}
//
//void eObject::LeanRight(std::vector<std::shared_ptr<eObject>> objects)
//{
//	glm::vec3 asix = glm::toMat4(transform->getRotation()) * glm::vec4( transform->getForward() , 1.0f);
//	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
//	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix)); 
//	transform->setRotation(glm::quat(rotation * transform->getRotation()));
//	for (auto& obj : objects) {
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), UP) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), DOWN)||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), RIGHT) )
//			{
//				glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix));
//				transform->setRotation(rotation * transform->getRotation());
//				break;
//			}
//		}
//	}
//}
//
//void eObject::LeanLeft(std::vector<std::shared_ptr<eObject>> objects)
//{
//	glm::vec3 asix = glm::toMat4(transform->getRotation()) * glm::vec4(transform->getForward(), 1.0f);
//	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
//	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix)); 
//	transform->setRotation(glm::quat(rotation * transform->getRotation()));
//	for (auto& obj : objects) {
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), UP) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), DOWN) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), LEFT) )
//			{
//				glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix)); 
//				transform->setRotation(rotation * transform->getRotation());
//				break;
//			}
//		}
//	}
//}
//
//void eObject::LeanForward(std::vector<std::shared_ptr<eObject>> objects)
//{
//	glm::vec3 Xasix = glm::cross(transform->getUp(), transform->getForward() );
//	glm::vec3 asix = glm::toMat4(transform->getRotation()) * glm::vec4(Xasix, 1.0f);
//	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix)); 
//	transform->setRotation(glm::quat(rotation * transform->getRotation()));
//	for (auto& obj : objects) {
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), UP) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), DOWN)) {
//				glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix)); 
//				transform->setRotation(rotation * transform->getRotation());
//				break;
//			}
//		}
//	}
//}
//
//void eObject::LeanBack(std::vector<std::shared_ptr<eObject>> objects)
//{
//	glm::vec3 Xasix = glm::cross( transform->getUp(), transform->getForward() );
//	glm::vec3 asix = glm::toMat4(transform->getRotation()) * glm::vec4(Xasix, 1.0f);
//	glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), -turnSpeed, asix)); 
//	transform->setRotation(glm::quat(rotation * transform->getRotation()));
//	for (auto& obj : objects) 
//	{
//		if (this->transform->getTranslation() != obj->transform->getTranslation() && this->m_model != obj->m_model)  // self-check! ??
//		{
//			if (collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), UP) ||
//				collider->collidesWith(*(transform.get()), *(obj->transform.get()), *(obj->collider.get()), DOWN)) 
//			{
//				glm::quat rotation = glm::toQuat(glm::rotate(glm::mat4(), turnSpeed, asix));
//				transform->setRotation(rotation * transform->getRotation());
//				break;
//			}
//		}
//	}
//}
