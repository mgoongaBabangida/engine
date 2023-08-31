#include "stdafx.h"
#include "RigidBody.h"

#include "Transform.h"

//-------------------------------------------------------------------------------
void eRigidBody::SetObject(eObject * obj) { m_object = obj; }

//-------------------------------------------------------------------------------
bool eRigidBody::Update(float _dt, std::vector<std::shared_ptr<eObject> > _objects)
{
	std::vector<glm::vec3> rays;

	const glm::vec3 translation = m_object->GetTransform()->getTranslation();
	m_object->GetTransform()->setTranslation(translation + m_velocity * _dt + 0.5f * m_acceleration * (_dt * _dt));
	m_velocity += m_acceleration * _dt;

	m_collision.collider = m_object;
	// check the collision
	std::array<Side, 6> sides{ FORWARD , UP, DOWN, LEFT, RIGHT, BACK };
	for (auto side : sides)
	{
		if (m_object->GetCollider()->CollidesWith(*(m_object->GetTransform()), _objects, side, m_collision))
		{
			const glm::vec3 vel = m_object->GetTransform()->getTranslation() - translation; // velocity since last frame
			for (unsigned int i = 1; i <= 10; ++i)
			{
				m_object->GetTransform()->setTranslation(translation + (vel * (float(i) * 0.1f)));
				for (auto side : sides)
				{
					if (m_object->GetCollider()->CollidesWith(*(m_object->GetTransform()), _objects, side, m_collision))
					{
						m_object->GetTransform()->setTranslation(translation);
						ReactCollision(m_collision);
						return true;
					}
				}
			}
			throw("logic error rigid body");
		}
	}

	// rare case, check if the object got totally inside of AABB of one of the objects
	for (const auto& obj : _objects)
	{
		if (!obj->GetTransform() || !obj->GetCollider() || m_object == obj.get())
			continue;

		if (m_object->GetCollider()->IsInsideOfAABB(*(m_object->GetTransform()), *obj->GetTransform(), *obj->GetCollider()))
		{
			const glm::vec3 vel = m_object->GetTransform()->getTranslation() - translation; // velocity since last frame
			for (unsigned int i = 1; i <= 10; ++i)
			{
				m_object->GetTransform()->setTranslation(translation + (vel * (float(i) * 0.1f)));
				for (auto side : sides)
				{
					if (m_object->GetCollider()->CollidesWith(*(m_object->GetTransform()), { obj }, side, m_collision))
					{
						m_object->GetTransform()->setTranslation(translation);
						ReactCollision(m_collision);
						return true;
					}
				}
			}
			throw("logic error rigid body");
		}
	}
	return true;
}

//-------------------------------------------------------------------------------
void eRigidBody::ApplyForce(glm::vec3 _force)
{
	m_acceleration += _force / m_mass;
}

//-------------------------------------------------------------------------------
void eRigidBody::ApplyForce(glm::vec3 _direction, float _magitude)
{
	ApplyForce(_direction * _magitude);
}

//-------------------------------------------------------------------------------
void eRigidBody::ApplyAcceleration(glm::vec3 _acceleration)
{
	m_acceleration += _acceleration;
}

//------------------------------------------------------------------------------
void eRigidBody::ApplyAcceleration(glm::vec3 _direction, float _magitude)
{
	ApplyAcceleration(_direction* _magitude);
}

//-------------------------------------------------------------------------------
void eRigidBody::ApplyImpulse(glm::vec3 _force, float _dt)
{
	m_velocity += _force / m_mass * _dt;
}

//-------------------------------------------------------------------------------
void eRigidBody::ApplyImpulse(glm::vec3 _direction, float _magitude, float _dt)
{
	ApplyImpulse(_direction* _magitude, _dt);
}

//-------------------------------------------------------------------------------
void eRigidBody::TransferEnergy(float _joules)
{
	if (_joules == 0)
		return;

	//KE = 1/2 * m * v^2
	float deltaV = sqrt(2 * abs(_joules) / m_mass);
	m_velocity += _joules > 0 ? deltaV : -deltaV;
}

//-------------------------------------------------------------------------------
bool eRigidBody::CollidesWith(eObject* _other, Side _side)
{
	m_collision.collider = m_object;
	m_collision.collidee = _other;
	if (!_other->GetCollider())
		return false;

	return m_object->GetCollider()->CollidesWith(*(m_object->GetTransform()),
																						 *(_other->GetTransform()),
																						 *(_other->GetCollider()),
																						 _side,
																						 m_collision);
}

//-------------------------------------------------------------------------------
void eRigidBody::ReactCollision(const eCollision& _col)
{
	if (_col.collider->GetScript())_col.collider->GetScript()->CollisionCallback(_col);
	if (_col.collidee->GetScript())_col.collidee->GetScript()->CollisionCallback(_col);
}

//-------------------------------------------------------------------------------
void eRigidBody::MoveForward(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getForward();
	dir = glm::normalize(dir) * (m_speed - thrsh);
	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += dir);
	for (auto& obj : objects)
	{
		if (*m_object != *obj && CollidesWith(obj.get(), Side::FORWARD))
		{
			m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() -= dir);
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::MoveBack(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getForward();
	dir = glm::normalize(-dir) * (m_speed - thrsh);
	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += dir);
	for (auto& obj : objects)
	{
		if (*m_object != *obj && CollidesWith(obj.get(), Side::BACK))
		{
			m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() -= dir);
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::MoveRight(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getForward();
	dir = glm::normalize(glm::cross(dir, YAXIS)) * (m_speed - thrsh);
	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += dir);
	for (auto& obj : objects)
	{
		if (*m_object != *obj && CollidesWith(obj.get(), Side::RIGHT))
		{
			m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() -= dir);
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::MoveLeft(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getForward();
	dir = glm::normalize(glm::cross(YAXIS, dir)) * (m_speed - thrsh);
	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += dir);
	for (auto& obj : objects)
	{
		if (*m_object != *obj && CollidesWith(obj.get(), Side::LEFT))
		{
			m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() -= dir);
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::MoveUp(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getUp();
	dir = glm::normalize(dir) * (m_speed - thrsh);
	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += dir);
	for (auto& obj : objects)
	{
		if (*m_object != *obj && CollidesWith(obj.get(), Side::UP))
		{
			m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() -= dir);
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::MoveDown(std::vector<std::shared_ptr<eObject> > objects)
{
	float thrsh = 0.001f;
	glm::vec3 dir = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getUp();
	dir = -glm::normalize(dir) * (m_speed - thrsh);
	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += dir);
	for (auto& obj : objects)
	{
		if (*m_object != *obj && CollidesWith(obj.get(), Side::DOWN))
		{
			m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() -= dir);
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::Move(std::vector<std::shared_ptr<eObject>> objects, float _dt)
{
	auto translation = m_object->GetTransform()->getTranslation();
	/*m_object->GetTransform()->setTranslation(translation += m_velocity * _dt + 0.5f * m_acceleration * (_dt * _dt));
	m_velocity += m_acceleration * _dt;*/

	m_object->GetTransform()->setTranslation(m_object->GetTransform()->getTranslation() += m_velocity);
	eCollision collision;
	collision.collider = m_object;
	std::array<Side, 6> sides{ FORWARD , UP, DOWN, LEFT, RIGHT, BACK };
	for (auto side : sides)
	{
		if (m_object->GetCollider()->CollidesWith(*(m_object->GetTransform()), objects, side, collision))
		{
			m_object->GetTransform()->setTranslation(translation);
			ReactCollision(collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::Turn(glm::vec3 direction, std::vector<std::shared_ptr<eObject>> objects) // $todo include speed
{
	glm::vec3 forward = glm::mat3(m_object->GetTransform()->getModelMatrix()) * m_object->GetTransform()->getForward();
	float angle = glm::dot(forward, direction);
	glm::vec3 asix = glm::cross(forward, direction);
	glm::quat rotation;
	angle >= 0 ? rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, glm::acos(angle), asix))
		: rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, 2.0f * PI - (glm::acos(angle)), -asix));
	m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::RIGHT)
			|| CollidesWith(obj.get(), Side::LEFT)
			|| CollidesWith(obj.get(), Side::UP)
			|| CollidesWith(obj.get(), Side::DOWN)))
		{
			angle >= 0 ? rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -glm::acos(angle), asix))
				: rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -(2.0f * PI - (glm::acos(angle))), -asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::TurnRight(std::vector<std::shared_ptr<eObject>> objects)
{
	// Conversion from axis-angle
	// In GLM the angle must be in degrees here, so convert it.
	glm::vec3 asix = glm::toMat4(m_object->GetTransform()->getRotation()) * glm::vec4(m_object->GetTransform()->getUp(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, m_turn_speed, asix));
	//q_rotation = rotation * q_rotation;
	m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::RIGHT) || (CollidesWith(obj.get(), Side::LEFT))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -m_turn_speed, asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::TurnLeft(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(m_object->GetTransform()->getRotation()) * glm::vec4(m_object->GetTransform()->getUp(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -m_turn_speed, asix));
	m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::RIGHT) || (CollidesWith(obj.get(), Side::LEFT))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, m_turn_speed, asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::LeanRight(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(m_object->GetTransform()->getRotation()) * glm::vec4(m_object->GetTransform()->getForward(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, m_turn_speed, asix));
	m_object->GetTransform()->setRotation(glm::quat(rotation * m_object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::RIGHT)
			|| (CollidesWith(obj.get(), Side::UP))
			|| (CollidesWith(obj.get(), Side::DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -m_turn_speed, asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::LeanLeft(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 asix = glm::toMat4(m_object->GetTransform()->getRotation()) * glm::vec4(m_object->GetTransform()->getForward(), 1.0f);
	//glm::quat rotation = glm::gtx::quaternion::angleAxis(glm::degrees(PI/3), asix);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -m_turn_speed, asix));
	m_object->GetTransform()->setRotation(glm::quat(rotation * m_object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::LEFT)
			|| (CollidesWith(obj.get(), Side::UP))
			|| (CollidesWith(obj.get(), Side::DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, m_turn_speed, asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::LeanForward(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 Xasix = glm::cross(m_object->GetTransform()->getUp(), m_object->GetTransform()->getForward());
	glm::vec3 asix = glm::toMat4(m_object->GetTransform()->getRotation()) * glm::vec4(Xasix, 1.0f);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, m_turn_speed, asix));
	m_object->GetTransform()->setRotation(glm::quat(rotation * m_object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::UP) || (CollidesWith(obj.get(), Side::DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -m_turn_speed, asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}

//-------------------------------------------------------------------------------
void eRigidBody::LeanBack(std::vector<std::shared_ptr<eObject>> objects)
{
	glm::vec3 Xasix = glm::cross(m_object->GetTransform()->getUp(), m_object->GetTransform()->getForward());
	glm::vec3 asix = glm::toMat4(m_object->GetTransform()->getRotation()) * glm::vec4(Xasix, 1.0f);
	glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, -m_turn_speed, asix));
	m_object->GetTransform()->setRotation(glm::quat(rotation * m_object->GetTransform()->getRotation()));
	for (auto& obj : objects)
	{
		if (*m_object != *obj && (CollidesWith(obj.get(), Side::UP) || (CollidesWith(obj.get(), Side::DOWN))))
		{
			glm::quat rotation = glm::toQuat(glm::rotate(UNIT_MATRIX, m_turn_speed, asix));
			m_object->GetTransform()->setRotation(rotation * m_object->GetTransform()->getRotation());
			ReactCollision(m_collision);
			break;
		}
	}
}