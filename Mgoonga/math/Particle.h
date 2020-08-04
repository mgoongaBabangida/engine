#pragma once

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

#include "math.h"

//-------------------------------------------------------------------
class DLL_MATH Particle
{
public:
	Particle(glm::vec3 position, glm::vec3 velocity, float gravityEffect, float lifelength, float rotation, float scale, uint32_t _numRows) 
	:	 m_position(position), 
		 m_velocity(velocity), 
		 m_gravityEffect(gravityEffect), 
		 m_lifelength(lifelength), 
		 m_rotation(rotation), 
		 m_scale(scale), 
		 numRowsInTexture(_numRows)
	{
		GRAVITY = 0.0; //!?
	}

	Particle()
	: alive(false)
	{

	}

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
		alive			= true;
	}

	glm::vec3	   getPosition()			{ return m_position; }		 const
	float		   getRotaion()				{ return m_rotation; }		 const
	float		   getScale()				{ return m_scale; }			 const
	glm::vec2	   gettexOffset1()			{ return this->texOffset1; } const
	glm::vec2	   gettexOffset2()			{ return this->texOffset2; } const
	int32_t		   getNumRows()				{ return numRowsInTexture; } const
	float		   getBlend()				{ return blend; }			 const
	bool		   isAlive()				{ return alive; }			 const
	float		   getDistance()			{ return distance; }		 const
	void		   setDistance(float dist)  { distance = dist; }
	bool		   Update();

private:
	void			updateTextureCoordInfo();
	void			setTextureOffset(glm::vec2& offset, int index);

	bool			alive = true;
	glm::vec3		m_position;
	glm::vec3		m_velocity;
	float			m_gravityEffect;
	float			m_lifelength;
	float			m_rotation;
	float			m_scale;
	float			GRAVITY;// = 0.0f; ??? static  const??
	float			m_elapsedTime = 0.0f;
	glm::vec2		texOffset1;
	glm::vec2		texOffset2;
	float			blend;
	float			distance;
	uint32_t		speed = 100; //msc
	uint32_t		numRowsInTexture = 4;
};
