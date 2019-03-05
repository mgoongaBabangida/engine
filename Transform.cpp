#include "Transform.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <glm/gtc/constants.hpp>

#include "Structures.h"

void Transform::setRotation(float x, float y, float z)
{
	q_rotation = glm::quat(glm::vec3(x, y, z)); // from euler angles (tutorial 17)
	UpdateModelMatrix();
}

glm::vec4 Transform::getRotationVector() const
{
	return  glm::toMat4(q_rotation) * glm::vec4(forward, 1.0f);
}

void Transform::UpdateModelMatrix()
{
	glm::mat4 rotatM0 = glm::toMat4(q_rotation);
	totalTransform = glm::translate(m_translation)* rotatM0* glm::scale(glm::vec3(m_scale.x, m_scale.y, m_scale.z));
}

glm::mat4 Transform::getModelMatrix() const
{
	return totalTransform;
}

glm::quat Transform::RotationBetweenVectors(glm::vec3 start, glm::vec3 dest)
{
	start = normalize(start);
	dest = normalize(dest);

	float cosTheta = dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f) {
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(180.0f, rotationAxis);
	}

	rotationAxis = cross(start,dest);

	float s = sqrt((1 + cosTheta) * 2);
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f,
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}

Transform Transform::interpolate(const Transform& first, const Transform& second, float progression)
{
	progression = glm::clamp(progression, 0.0f, 1.0f);
	Transform ret;
	glm::vec3 dir = first.m_translation + second.m_translation;
	ret.setTranslation(first.m_translation + dir * progression );
	ret.setRotation(glm::lerp(first.q_rotation, second.q_rotation, progression));
	ret.setScale(glm::vec3((first.m_scale.x + second.m_scale.x) / 2, (first.m_scale.y + second.m_scale.y) / 2, (first.m_scale.z + second.m_scale.z) / 2));
	return ret;
}

void Transform::billboard(glm::vec3 direction)
{
	// Rotation aroun Y
	float projZ = glm::dot(glm::normalize(direction), glm::vec3(0.0f, 0.0f, 1.0f));
	float projX = glm::dot(glm::normalize(direction), glm::vec3(-1.0f, 0.0f, 0.0f)); //or +1
	glm::vec3 projXZ = glm::normalize(glm::vec3(projX, 0, projZ));
	
	float Yrot = direction.x > 0.0f 
				? acos(glm::dot(projXZ, glm::vec3(0.0f, 0.0f, 1.0f))) 
				: (float)str::PI * 2 - acos(glm::dot(projXZ, glm::vec3(0.0f, 0.0f, 1.0f)));

	// Rotation aroun X
	float Xrot = glm::dot(glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f)) > 0 
				? -(str::PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f)))) 
				: -(str::PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f))));

	setRotation(Xrot, Yrot, 0);
}

bool Transform::turnTo(glm::vec3 dest, float speed) //speed ?
{	
	glm::vec3 target_dir	= glm::normalize( glm::vec3(dest - m_translation) );
	glm::vec3 cur_directoin = glm::normalize(forward * glm::mat3(glm::toMat4(q_rotation)));
	cur_directoin.x			= -cur_directoin.x;
	float angle				= glm::dot(target_dir, cur_directoin);

	glm::vec3 ASIX = glm::normalize(glm::cross( target_dir, cur_directoin ) );

	//float rot_angle = ASIX.y > 0.0f ? 
	//if (abs(angle) < 0.001f)
	if(angle > 0.99f)
		return false;

	glm::quat rot;

	if(ASIX.y <= 0.0f && angle >= 0) 
	{ //x+ z+
		rot = glm::toQuat(glm::rotate(glm::mat4(), glm::acos(angle),-ASIX)); // turn left less then 90
		//std::cout << "first case" << std::endl;
	}
	else if(ASIX.y > 0.0f && angle >= 0) 
	{ //x- z+
		rot = glm::toQuat(glm::rotate(glm::mat4(), (glm::acos(angle)), -ASIX));// turn right less then 90
		//std::cout << "second case" << std::endl;
	}
	else if(ASIX.y <= 0.0f && angle < 0) 
	{   //x+ z -
		rot = glm::toQuat(glm::rotate(glm::mat4(), 2.0f * str::PI - (glm::acos(angle)), ASIX));
		//std::cout << "third case" << std::endl;
	}
	else if(ASIX.y > 0.0f && angle < 0) 
	{   //x- z -
		rot = glm::toQuat(glm::rotate(glm::mat4(), 2.0f * str::PI - (glm::acos(angle)), ASIX));
		//std::cout << "fourth case" << std::endl;
	}
	else 
	{
		//std::cout<<"_Debug else"<<std::endl;
	}

	setRotation(rot * getRotation());

	return true;
}




