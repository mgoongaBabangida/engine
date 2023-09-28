#pragma once

#include "math.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

namespace dbb
{
	struct Bezier;
}

//-------------------------------------------------------------------
class DLL_MATH Particle
{
public:
	Particle(glm::vec3 position,
					 glm::vec3 velocity,
					 float gravityEffect,
					 float lifelength,
					 float rotation,
					 float scale,
					 uint32_t _numRows)
	:	 m_position(position),
		 m_velocity(velocity),
		 m_gravityEffect(gravityEffect),
		 m_lifelength(lifelength),
		 m_rotation(rotation),
		 m_scale(scale), 
		 numRowsInTexture(_numRows),
		 m_alive(false)
	{}

	Particle()
	: m_alive(false)
	{}

	void reset(glm::vec3 position,
						 glm::vec3 velocity,
						 float	 gravityEffect,
						 float	 lifelength,
						 float	 rotation,
						 float	 scale,
						 uint32_t _numRows)
	{
		m_position		= position; 
		m_velocity		= velocity;
		m_gravityEffect = gravityEffect;
		m_lifelength	= lifelength; 
		m_rotation		= rotation; 
		m_scale			= scale; 
		m_elapsedTime	= 0;
		m_alive			= true;
	}

	bool				Update(float _elapsed);

	glm::vec3		getPosition() { return m_position; }		 const
	float				getRotaion() { return m_rotation; }		 const

	float				getScale();			 const
	void				setScale(float _scale)		{ m_scale = _scale; }

	glm::vec2	  gettexOffset1()						{ return this->texOffset1; } const
	glm::vec2	  gettexOffset2()						{ return this->texOffset2; } const

	int32_t		  getNumRows()							{ return numRowsInTexture; } const
	float				getBlend()								{ return blend; }			 const

	bool				isAlive()									{ return m_alive; }			 const
	
	float				getDistance()							{ return distance; }		 const
	void				setDistance(float dist)		{ distance = dist; }

	void				setScaleCurve(dbb::Bezier* _scale_curve) { m_scale_curve = _scale_curve; }

private:
	void			_updateTextureCoordInfo();
	void			_setTextureOffset(glm::vec2& offset, int index);

	static float		GRAVITY;

	bool						m_alive; //m_elapsedTime >= m_lifelength
	
	glm::vec3				m_position;
	glm::vec3				m_velocity;
	float						m_elapsedTime = 0.0f;

	float						m_lifelength = 1'000'000; //@todo float max
	
	float						m_rotation;//?
	float						m_scale;
	dbb::Bezier*		m_scale_curve = nullptr;

	//texturing
	glm::vec2				texOffset1;
	glm::vec2				texOffset2;
	float						blend;

	float						distance;

	uint32_t				numRowsInTexture = 4; //outside?

	float						m_gravityEffect = 1.0f; //?
};
