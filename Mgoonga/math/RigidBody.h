#pragma once

#include "math.h"
#include <base/Object.h>

//---------------------------------------------------------
class DLL_MATH eRigidBody : public IRigidBody
{
public:

	virtual void SetObject(eObject* obj);
	
	virtual bool Update(float _dt, std::vector<std::shared_ptr<eObject> > _objects);

	virtual void ApplyForce(glm::vec3 _force);
	virtual void ApplyForce(glm::vec3 _direction, float _magitude);
	
	virtual void ApplyAcceleration(glm::vec3 _acceleration);
	virtual void ApplyAcceleration(glm::vec3 _direction, float _magitude);

	virtual void ApplyImpulse(glm::vec3 _force, float _dt);
	virtual void ApplyImpulse(glm::vec3 _direction, float _magitude, float _dt);

	virtual void TransferEnergy(float _joules);

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

	virtual void Move(std::vector<shObject> objects, float _dt);
	virtual void Turn(glm::vec3 direction, std::vector<shObject> objects);

	void				SetCurrentVelocity(glm::vec3 _vel) { m_velocity = _vel; } //@todo
	glm::vec3		Velocity()	const { return m_velocity; }

	static constexpr glm::vec3 s_gravity = glm::vec3(0.0f, -9.81f, 0.0f);
protected:
	bool		CollidesWith(eObject*, Side);
	void		ReactCollision(const eCollision& col);

	eObject*		m_object				= nullptr;
	float				m_turn_speed		= PI / 12.0f;
	float				m_speed					= 0.01f;// used for internal force
	
	glm::vec3		m_velocity			= { 0.0f ,0.0f ,0.0f };
	glm::vec3		m_acceleration	= { 0.0f ,0.0f ,0.0f };
	float				m_mass					= 1.0f;
	eCollision	m_collision;
};
