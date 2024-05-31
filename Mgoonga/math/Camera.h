#pragma once

#include "math.h"
#include "CameraRay.h"

//---------------------------------------------------------------------
class DLL_MATH Camera
{
public:
	Camera(float width,
				 float height,
				 float nearPlane,
				 float farPlane,
				 float perspectiveRatio = 60.0f,
				 glm::vec3 _position		 = glm::vec3( -1.0f, 4.0f, -2.5f ),
				 glm::vec3 _viewDirection = glm::vec3(0.6f, -0.10f, 0.8f),
				 glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f));

	Camera(const Camera& other);
	Camera&					operator=(const Camera & other);
	~Camera() = default;
	
	void					mouseUpdate(const glm::vec2& newMousePosition);

	std::vector<glm::vec4>	getFrustumCornersWorldSpace()	const;
	glm::mat4								getWorldToViewMatrix()			const;
	glm::vec3								getPosition()					const;
	glm::vec3								getDirection()					const;
	glm::mat3								getRotationMatrix()				const;
	const glm::mat4&				getProjectionMatrix()			const;
	glm::mat4								getProjectionBiasedMatrix()		const;
	const glm::mat4&				getProjectionOrthoMatrix()		const;
	uint32_t								getWidth()						const	{ return width;  }
	uint32_t								getHeight()						const	{ return height; }
	float										getNearPlane()					const	{ return nearPlane; }
	float										getFarPlane()					const;
	float										getZoom()						const	{ return zoom; }
	dbb::CameraRay&					getCameraRay()							{ return camRay; }

	bool					VisualiseFrustum()				const	{ return m_visualise_frustum; }
	void					SetVisualiseFrustum(bool _v)			{ m_visualise_frustum = _v; }
	void					UpdateProjectionMatrix();

	void					moveForward();
	void					moveBackword();
	void					strafeLeft();	
	void					strafeRight();	
	void					moveUp();	
	void					moveDown();

	void setPosition(glm::vec3 newPos)								{ position = newPos;}
	void setDirection(glm::vec3 newDir)								{ viewDirection = newDir; }

	void setNearPlane(float near);

	void setFarPlane(float far);

	glm::vec3&			PositionRef()							{ return position; }
	glm::vec3&			ViewDirectionRef()						{ return viewDirection; }
	float&					NearPlaneRef()							{ return nearPlane; }
	float&					FarPlaneRef()							{ return farPlane; }
	uint32_t&				StrafeThresholdRef()					{ return strafeThreshold; }
	float&					MovementSpeedRef()						{ return MOVEMENT_SPEED; }

protected:
	glm::vec3		position;
	glm::vec3		viewDirection;
	glm::vec3		up;
	glm::vec2		oldMousePosition;
	glm::vec3		strafeDirection;
	glm::mat3		rotationMatrix;
	float				MOVEMENT_SPEED			= 0.000'005f;
	float				rotation_speed			= 0.005f;

	glm::mat4		projectionMatrix;
	glm::mat4		projectionOrthoMatrix;

	uint32_t		width;
	uint32_t		height;
	float				nearPlane;
	float				farPlane;
	float				zoom					= 60.0f;

	dbb::CameraRay	camRay;

	uint32_t		strafeThreshold			= 5;
	bool				m_visualise_frustum =	 false;
};


