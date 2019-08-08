#pragma once
#include "InterfacesDB.h"
#include "Particle.h"
#include <vector>
#include "Clock.h"
#include <memory>
#include "Sound.h"
#include "Timer.h"

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

class ParticleSystem: public IParticleSystem
{
public:
	ParticleSystem(float	 _pps, 
				   float	 _speed, 
				   float	 _gravityComplient, 
				   float	 _lifeLength, 
				   glm::vec3 _systemCenter, 
				   Texture*  _texture, 
				   RemSnd*	 _sound, 
				   float	 _duration = 10000.0f);
	
	static const int MAX_PARTICLES = 1000;
	
	virtual void								generateParticles();
	virtual std::vector<Particle>::iterator		prepareParticles(glm::vec3 cameraPosition);
	virtual std::vector<Particle>&				getParticles()	{ return  m_particles; }
	virtual bool								IsFinished()	{ return clock.timeEllapsedMsc() > duration + m_lifeLength;}

	virtual ~ParticleSystem()					{ timer->stop(); }
protected:
	virtual void								Update();
	virtual void								emitParticles();

	glm::vec3					systemCenter;
	int							m_pps; //particles per second
	float						m_speed;
	float						m_gravityComplient;
	float						m_lifeLength;
	std::vector<Particle>		m_particles;
	eClock						clock;
	std::unique_ptr<dbb::Timer> timer;
	RemSnd*						sound;
	float						duration;
};
