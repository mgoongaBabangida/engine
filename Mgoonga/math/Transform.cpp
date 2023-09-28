#include "stdafx.h"
#include "Transform.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm/gtc/constants.hpp>

#include <base/base.h>

//-----------------------------------------------------------------------------
void Transform::setRotation(float x, float y, float z)
{
	q_rotation = glm::quat(glm::vec3(x, y, z)); // from euler angles (tutorial 17)
	UpdateModelMatrix();
}

//-----------------------------------------------------------------------------
glm::vec4 Transform::getRotationVector() const
{
	return  glm::toMat4(q_rotation) * glm::vec4(forward, 1.0f);
}

//-----------------------------------------------------------------------------
glm::vec4 Transform::getRotationUpVector() const
{
	return  glm::toMat4(q_rotation) * glm::vec4(Up, 1.0f);
}

//-----------------------------------------------------------------------------
void Transform::UpdateModelMatrix()
{
	glm::mat4 rotatM0 = glm::toMat4(q_rotation);
	totalTransform = glm::translate(m_translation)* rotatM0* glm::scale(glm::vec3(m_scale.x, m_scale.y, m_scale.z));
}

//-----------------------------------------------------------------------------
const glm::mat4& Transform::getModelMatrix() const
{
	return totalTransform;
}

//-----------------------------------------------------------------------------
void Transform::setModelMatrix(const glm::mat4& _mat)
{
	totalTransform = _mat;
	m_translation = { _mat[0][3],_mat[1][3], _mat[2][3] };
	float s_x = glm::length(glm::vec3{ _mat[0][0],_mat[1][0], _mat[2][0] });
	float s_y = glm::length(glm::vec3{ _mat[0][1],_mat[1][1], _mat[2][1] });
	float s_z = glm::length(glm::vec3{ _mat[0][2],_mat[1][2], _mat[2][2] });
	m_scale = { s_x, s_y , s_z };
	glm::mat4 rotation;
	rotation[0][0] = _mat[0][0] / s_x;
	rotation[0][1] = _mat[0][1] / s_y;
	rotation[0][2] = _mat[0][2] / s_z;
	rotation[0][3] = 0;
	rotation[1][0] = _mat[1][0] / s_x;
	rotation[1][1] = _mat[1][1] / s_y;
	rotation[1][2] = _mat[1][2] / s_z;
	rotation[1][3] = 0;
	rotation[2][0] = _mat[2][0] / s_x;
	rotation[2][1] = _mat[2][1] / s_y;
	rotation[2][2] = _mat[2][2] / s_z;
	rotation[2][3] = 0;
	rotation[3][0] = 0;
	rotation[3][1] = 0;
	rotation[3][2] = 0;
	rotation[3][3] = 1;
	q_rotation = glm::quat(rotation);
}

//-----------------------------------------------------------------------------
bool Transform::isRotationValid()
{
	return q_rotation.x > 0.0f || q_rotation.x < 1.0f 
		&& q_rotation.y > 0.0f || q_rotation.y < 1.0f
		&& q_rotation.z > 0.0f || q_rotation.z < 1.0f;
}

//-----------------------------------------------------------------------------
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
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
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

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
void Transform::billboard(glm::vec3 direction)
{
	// Rotation aroun Y
	float projZ = glm::dot(glm::normalize(direction), glm::vec3(0.0f, 0.0f, 1.0f));
	float projX = glm::dot(glm::normalize(direction), glm::vec3(-1.0f, 0.0f, 0.0f)); //or +1
	glm::vec3 projXZ = glm::normalize(glm::vec3(projX, 0, projZ));
	
	float Yrot = direction.x > 0.0f 
				? acos(glm::dot(projXZ, glm::vec3(0.0f, 0.0f, 1.0f))) 
				: (float)PI * 2 - acos(glm::dot(projXZ, glm::vec3(0.0f, 0.0f, 1.0f)));

	// Rotation aroun X
	float Xrot = glm::dot(glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f)) > 0 
				? -(PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f)))) 
				: -(PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f))));

	setRotation(Xrot, Yrot, 0);
}

//-----------------------------------------------------------------------------
bool Transform::turnTo(glm::vec3 dest, float speed) //@todo speed is not used
{	
	if (dest == m_translation)
		return false;

	glm::vec3 target_dir	= glm::normalize(glm::vec3(dest - m_translation) );
	glm::vec3 cur_directoin = glm::normalize(glm::mat3(glm::toMat4(q_rotation)) * forward);

	float angle			= glm::dot(cur_directoin, target_dir);
	glm::vec3 asix	= glm::cross(cur_directoin, target_dir);

	if (angle > 0.999f || angle < -1.0f)
		return false;
	
	glm::quat rot;
	if (target_dir + cur_directoin == glm::vec3{} || (angle == 1.0f && glm::length(asix) == 0)) // 180 degrees
	{ 
		rot = RotationBetweenVectors(cur_directoin, target_dir); //will not be correct if speed is used
		setRotation(rot * getRotation());
		return true;
	}
	else if (angle > 0.0f)
		{ rot = glm::toQuat(glm::rotate(UNIT_MATRIX, glm::acos(angle), asix));}
	else if (angle < 0.0f)
		{ rot = glm::toQuat(glm::rotate(UNIT_MATRIX, 2 * PI - (glm::acos(angle)), -asix));}
	else 
		{ return false;} // angle inf(nan)

	setRotation(rot * getRotation());
	return true;
}




