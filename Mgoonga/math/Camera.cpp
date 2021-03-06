#include "stdafx.h"
#include "Camera.h"
#include <glm\glm\gtx\transform.hpp>
#include <iostream>

//---------------------------------------------------------------------
Camera::Camera(float      _width,
			   float	  _height,
			   float	  _nearPlane,
			   float	  _farPlane,
			   float	  _perspective_ratio, 
			   glm::vec3 _position, 
			   glm::vec3 _viewDirection)
	: position(_position)
	, viewDirection(_viewDirection)
	, Up(0.0f, 1.0f, 0.0f)
	, strafeDirection(1.0f, 0.0f, 0.0f)
	, projectionOrthoMatrix(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, _nearPlane, _farPlane))
	, width(_width)
	, height(_height)
	, nearPlane(_nearPlane)
	, farPlane(_farPlane)
{
	#define GLM_FORCE_RADIANS
	projectionMatrix = glm::perspective(glm::radians(_perspective_ratio), ((float)_width) / _height, _nearPlane, _farPlane);
}

Camera::Camera(const Camera & other)
{
	Up				 = other.Up;
	position		 = other.position;
	viewDirection	 = other.viewDirection;
	oldMousePosition = other.oldMousePosition;
	strafeDirection	 = other.strafeDirection;
	rotationMatrix	 = other.rotationMatrix;
	projectionMatrix = other.projectionMatrix;
}

Camera& Camera::operator=(const Camera& other)
{
	if (&other != this) 
	{  
		position		 = other.position;
		viewDirection	 = other.viewDirection;
		oldMousePosition = other.oldMousePosition;
		strafeDirection	 = other.strafeDirection;
		rotationMatrix	 = other.rotationMatrix;
		projectionMatrix = other.projectionMatrix;
	}
	return *this;
}

bool Camera::OnMouseMove(uint32_t x, uint32_t y)
{
	mouseUpdate(glm::vec2(x, y));
	return true;
}

bool Camera::OnKeyPress(uint32_t asci)
{
	switch(asci)
	{
		case ASCII_W: moveForward();  return true;
		case ASCII_S: moveBackword(); return true;
		case ASCII_D: strafeLeft();   return true;
		case ASCII_A: strafeRight();  return true;
		case ASCII_R: moveUp();		  return true;
		case ASCII_F: moveDown();	  return true;
		default:					  return false;
	}
	return false;
}

glm::mat4 Camera::getWorldToViewMatrix() const
{
	return glm::lookAt(position, position + viewDirection, Up);
}

glm::vec3 Camera::getPosition() const
{
	return position;
}

glm::vec3 Camera::getDirection() const
{
	return viewDirection;
}

 glm::mat3 Camera::getRotationMatrix() const
{
	 return this->rotationMatrix;
}

 glm::mat4 Camera::getProjectionBiasedMatrix() const
 {
	 return glm::mat4(glm::vec4(0.5f, 0.0f, 0.0f, 0.0f),
		    glm::vec4(0.0f, 0.5f, 0.0f, 0.0f),
		    glm::vec4(0.0f, 0.0f, 0.5f, 0.0f),
		    glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)) * projectionMatrix;
 }

void Camera::mouseUpdate(const glm::vec2& newMousePosition)
{
	glm::vec2 mouseDelta = newMousePosition - oldMousePosition;
	if(glm::length(mouseDelta) > 5.0f)
	{
		oldMousePosition = newMousePosition;
	}
	else
	{
		strafeDirection = glm::cross(Up,viewDirection);
		viewDirection = glm::mat3(
			glm::rotate(-mouseDelta.x * MOVEMENT_SPEED * 0.1f , Up) * 
			glm::rotate(mouseDelta.y * MOVEMENT_SPEED * 0.1f, strafeDirection)) * viewDirection;
		oldMousePosition = newMousePosition;
		//
		rotationMatrix = rotationMatrix* glm::mat3(
			glm::rotate(-mouseDelta.x * MOVEMENT_SPEED * 0.1f, Up) *
			glm::rotate(mouseDelta.y * MOVEMENT_SPEED * 0.1f, strafeDirection));
	}
}

void Camera::moveForward() 
{
	position += MOVEMENT_SPEED * viewDirection;
}
void Camera::moveBackword()
{
	position -= MOVEMENT_SPEED * viewDirection;
}
void Camera::strafeLeft()
{
	position += -MOVEMENT_SPEED * strafeDirection;
}
void Camera::strafeRight()
{
	position += MOVEMENT_SPEED * strafeDirection;
}
void Camera::moveUp() 
{
	position += MOVEMENT_SPEED * Up;
}
void Camera::moveDown() 
{
	position += -MOVEMENT_SPEED * Up;
}