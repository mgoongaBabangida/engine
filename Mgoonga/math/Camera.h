#pragma once

#include "math.h"
#include <base/interfaces.h>

//---------------------------------------------------------------------
class DLL_MATH Camera : public IInputObserver
{
public:
	explicit Camera(float _width ,
		            float _height,
		            float _nearPlane,
		            float _farPlane,
					float _perspective_ratio = 60.0f,
					glm::vec3 _position		 = glm::vec3( -1.0f, 4.0f, -2.5f ),
					glm::vec3 _viewDirection = glm::vec3(0.6f, -0.10f, 0.8f));

	Camera(const Camera& other);
	Camera&				operator=(const Camera & other);
	~Camera() = default;

	virtual bool		OnMouseMove(uint32_t x, uint32_t y) override;
	virtual bool		OnKeyPress(uint32_t asci)			override;
	
	void				mouseUpdate(const glm::vec2& newMousePosition);

	glm::mat4			getWorldToViewMatrix() const;
	glm::vec3			getPosition() const;
	glm::vec3			getDirection() const;
	glm::mat3			getRotationMatrix() const;
	const glm::mat4& getProjectionMatrix() const;
	glm::mat4			getProjectionBiasedMatrix() const;
	const glm::mat4&	getProjectionOrthoMatrix() const { return projectionOrthoMatrix; };
	uint32_t			getWidth() const { return width;  }
	uint32_t			getHeight() const { return height; }
	float				getNearPlane() const { return nearPlane; }
	float				getFarPlane() const;

	void				moveForward();
	void				moveBackword();
	void				strafeLeft();	
	void				strafeRight();	
	void				moveUp();	
	void				moveDown();

	void setPosition(glm::vec3 newPos) {
		position = newPos;
	}

	void setDirection(glm::vec3 newDir) {
		viewDirection = newDir;
	}

	glm::vec3&			PositionRef()		{ return position; }
	glm::vec3&			ViewDirectionRef()  { return viewDirection; }

protected:
	glm::vec3			position;
	glm::vec3			viewDirection;
	glm::vec3			Up;
	glm::vec2			oldMousePosition;
	glm::vec3			strafeDirection;
	glm::mat3			rotationMatrix;
	const float			MOVEMENT_SPEED = 0.05f;//@should be flexible and frame dependent

	glm::mat4			projectionMatrix;
	glm::mat4			projectionOrthoMatrix;
	uint32_t			width;
	uint32_t			height;
	float				nearPlane;
	float				farPlane;
};


