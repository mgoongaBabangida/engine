#include "stdafx.h"
#include "Object.h"
#include "MyModel.h"
#include "InterfacesDB.h"
#include "RigidBody.h"

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

eRigidBody * eObject::getRigidBody() const
{
	return movementApi;
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

void eObject::Move(std::vector<std::shared_ptr<eObject>> objects)
{
	movementApi->Move(objects);
}
void eObject::Turn(glm::vec3 direction, std::vector<std::shared_ptr<eObject>> objects)
{
	movementApi->Turn(direction, objects);
}

void eObject::Debug()
{
	
}
