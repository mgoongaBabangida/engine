#include "stdafx.h"
#include "Camera.h"
#include <glm\glm\gtx\transform.hpp>

//---------------------------------------------------------------------
Camera::Camera(float			_width,
							float				_height,
							float				_nearPlane,
							float				_farPlane,
							float				_perspectiveRatio,
							glm::vec3		_position, 
							glm::vec3		_viewDirection,
							glm::vec3 _up)
: up(_up)
, strafeDirection(1.0f, 0.0f, 0.0f)
, projectionOrthoMatrix(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, _nearPlane, _farPlane))
, width(static_cast<uint32_t>(_width))
, height(static_cast<uint32_t>(_height))
, nearPlane(_nearPlane)
, farPlane(_farPlane)
, zoom(_perspectiveRatio)
, position(_position)
, viewDirection(_viewDirection)
, camRay(*this)
{
	#define GLM_FORCE_RADIANS
	projectionMatrix = glm::perspective(glm::radians(zoom), ((float)width) / height, nearPlane, farPlane);
}
//---------------------------------------------------------------------
Camera::Camera(const Camera & other)
	: camRay(*this)
{
	*this = other;
}
//---------------------------------------------------------------------
Camera& Camera::operator=(const Camera& other)
{
	if (&other != this) 
	{
		position				= other.position;
		viewDirection			= other.viewDirection;
		up						= other.up;
		oldMousePosition		= other.oldMousePosition;
		strafeDirection			= other.strafeDirection;
		rotationMatrix			= other.rotationMatrix;
		MOVEMENT_SPEED			= other.MOVEMENT_SPEED;
		camRay					= other.camRay;
		projectionMatrix		= other.projectionMatrix;
		projectionOrthoMatrix	= other.projectionOrthoMatrix;
		width					= other.width;
		height					= other.height;
		nearPlane				= other.nearPlane;
		farPlane				= other.farPlane;
		zoom					= other.zoom;
		strafeThreshold			= other.strafeThreshold;
	}
	return *this;
}
//---------------------------------------------------------------------
glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, up);
}
//---------------------------------------------------------------------
glm::vec3 Camera::getPosition() const
{
	return position;
}
//---------------------------------------------------------------------
glm::vec3 Camera::getDirection() const
{
	return viewDirection;
}
//---------------------------------------------------------------------
glm::mat3 Camera::getRotationMatrix() const
{
	 return this->rotationMatrix;
}
//---------------------------------------------------------------------
const glm::mat4& Camera::getProjectionMatrix() const
{
  return projectionMatrix;
}
//---------------------------------------------------------------------
glm::mat4 Camera::getProjectionBiasedMatrix() const
{
	 return glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
					  glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
					  glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
					  glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)) * projectionMatrix;
}
//---------------------------------------------------------------------
const glm::mat4& Camera::getProjectionOrthoMatrix() const
{
	 return projectionOrthoMatrix;
}
//---------------------------------------------------------------------
std::vector<glm::vec4> Camera::getFrustumCornersWorldSpace() const
{
	const auto inv = glm::inverse(projectionMatrix * getWorldToViewMatrix());

	std::vector<glm::vec4> frustumCorners;
	for (unsigned int x = 0; x < 2; ++x)
	{
		for (unsigned int y = 0; y < 2; ++y)
		{
			for (unsigned int z = 0; z < 2; ++z)
			{
				const glm::vec4 pt =
					inv * glm::vec4(
						 2.0f * x - 1.0f,
						 2.0f * y - 1.0f,
						 2.0f * z - 1.0f,
						 1.0f);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}
	return frustumCorners;
}
//---------------------------------------------------------------------
void Camera::mouseUpdate(const glm::vec2& newMousePosition)
{
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if(glm::length(mouseDelta) > strafeThreshold)
	{
		oldMousePosition = newMousePosition;
	}
	else
	{
		strafeDirection = glm::cross(up,viewDirection);
		viewDirection = glm::mat3(
			glm::rotate(-mouseDelta.x * rotation_speed, up) *
			glm::rotate(mouseDelta.y * rotation_speed, strafeDirection)) * viewDirection;

		oldMousePosition = newMousePosition;

		rotationMatrix = rotationMatrix * glm::mat3(glm::rotate(-mouseDelta.x * rotation_speed, up) *
																								glm::rotate(mouseDelta.y * rotation_speed, strafeDirection));
	}
}
//---------------------------------------------------------------------
float Camera::getFarPlane() const
{
	return farPlane;
}//---------------------------------------------------------------------

void Camera::moveForward()
{
	position += MOVEMENT_SPEED * viewDirection;
}
//---------------------------------------------------------------------
void Camera::moveBackword()
{
	position -= MOVEMENT_SPEED * viewDirection;
}
//---------------------------------------------------------------------
void Camera::strafeLeft()
{
	position += -MOVEMENT_SPEED * strafeDirection;
}
//---------------------------------------------------------------------
void Camera::strafeRight()
{
	position += MOVEMENT_SPEED * strafeDirection;
}
//---------------------------------------------------------------------
void Camera::moveUp() 
{
	position += MOVEMENT_SPEED * up;
}
//---------------------------------------------------------------------
void Camera::moveDown() 
{
	position += -MOVEMENT_SPEED * up;
}
//---------------------------------------------------------------------
void Camera::setNearPlane(float _near)
{
	nearPlane			= _near;
	projectionMatrix	= glm::perspective(glm::radians(zoom), ((float)width) / height, nearPlane, farPlane);
}
//---------------------------------------------------------------------
void Camera::setFarPlane(float _far)
{
	farPlane = _far;
	projectionMatrix = glm::perspective(glm::radians(zoom), ((float)width) / height, nearPlane, farPlane);
}
//---------------------------------------------------------------------
void Camera::UpdateProjectionMatrix()
{
	projectionMatrix = glm::perspective(glm::radians(zoom), ((float)width) / height, nearPlane, farPlane);
}
