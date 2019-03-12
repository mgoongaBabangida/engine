#pragma once
#include <glm\glm\glm.hpp>

class Camera
{
private:
	glm::vec3 position;
	glm::vec3 viewDirection;
	const glm::vec3 Up;
	glm::vec2 oldMousePosition;
	const float MOVEMENT_SPEED = 0.05f;
	glm::vec3 strafeDirection;
	glm::mat3 rotationMatrix;
public:
	Camera();
	Camera(const Camera& other);
	Camera& operator=(const Camera & other);
	glm::mat4 getWorldToViewMatrix() const;
	glm::vec3 getPosition() const;
	glm::vec3 getDirection() const;
	glm::mat3 getRotationMatrix() const;
	void mouseUpdate(const glm::vec2& newMousePosition);
		
	void moveForward();
	void moveBackword();
	void strafeLeft();	
	void strafeRight();	
	void moveUp();	
	void moveDown();
	//
	void setPosition(glm::vec3 newPos) {
		position = newPos;
	}

	void setDirection(glm::vec3 newDir) {
		viewDirection = newDir;
	}
	~Camera();
};

