#pragma once

#include <base/interfaces.h>
#include <math/Timer.h>
#include <math/Particle.h>

#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>

//----------------------------------------------------
class DLL_MATH ParticleSystem: public IParticleSystem
{
public:
	ParticleSystem(float	 _pps,
								 float	 _speed,
								 float	 _gravityComplient,
								 float	 _lifeLength,
								 glm::vec3 _systemCenter,
								 const Texture*  _texture,
								 ISound*	 _sound,
								 size_t	 _num_rows_in_texture,
								 float	 _duration = 10000.0f);
	
	static const int32_t MAX_PARTICLES = 1000;
	
	virtual void								              GenerateParticles();
	virtual std::vector<Particle>::iterator		PrepareParticles(glm::vec3 cameraPosition);
	virtual std::vector<Particle>&				    GetParticles()	{ return  m_particles; }
	virtual bool								              IsFinished();

	virtual ~ParticleSystem();

protected:
	virtual void								  Update();
	void								          emitParticles();
	virtual	glm::vec3							_calculateParticles();

	glm::vec3										systemCenter;
	int													m_pps; //particles per second
	float												m_speed;
	float												m_gravityComplient;
	float												m_lifeLength;
	std::vector<Particle>				m_particles;
	math::eClock								clock;
	std::unique_ptr<math::Timer>timer;
	ISound*											sound;
	float												duration;
	uint32_t										cur_particles = 0;
	size_t											num_rows_in_texture = 1;
};
