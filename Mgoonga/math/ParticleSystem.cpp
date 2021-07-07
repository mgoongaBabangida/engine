#include "stdafx.h"
#include "ParticleSystem.h"
#include <base/base.h>

#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>

//------------------------------------------------------------------------------------
ParticleSystem::ParticleSystem(float	 _pps, 
							   float	 _speed, 
							   float	 _gravityComplient, 
							   float	 _lifeLength,
							   glm::vec3 _systemCenter,
							   Texture*  _texture,
							   ISound*	 _sound,
							   size_t	 _num_rows_in_texture,
							   float	 _duration)
: IParticleSystem(_texture, 0.05f)
, duration(_duration)
, m_pps(_pps)
, m_speed(_speed)
, m_gravityComplient(_gravityComplient)
, m_lifeLength(_lifeLength)
, systemCenter(_systemCenter)
, sound(_sound)
, num_rows_in_texture(_num_rows_in_texture)
, m_particles(MAX_PARTICLES, Particle{})
{
  m_particles.resize(MAX_PARTICLES);
	clock.start();

	if(sound) sound->Play();

	timer.reset(new math::Timer([this]()->bool {this->Update(); return true; }));
	timer->start(100);
	srand(time(0));
}

//-------------------------------------------------------------------------
void ParticleSystem::GenerateParticles()
{
	if(clock.timeEllapsedMsc() < duration)
	{
		int msc = clock.newFrame();
		float new_particles = (float)msc / 1000.0f * (float)m_pps;
		for (int i = 0; i < new_particles; ++i)
			emitParticles();
	}
	else
	{
		if(sound && sound->isPlaying())
		{
			sound->Stop();
		}
	}
}

//------------------------------------------------------------------------------
void ParticleSystem::emitParticles()
{
	glm::vec3 dir = _calculateParticles();

	if (cur_particles < MAX_PARTICLES)
	{
		m_particles[cur_particles].reset(systemCenter, dir, 0, m_lifeLength, 0, 0, num_rows_in_texture);
		++cur_particles;
	}
	else 
	{
		auto prt = std::find_if(m_particles.begin(), m_particles.end(), [](Particle& pt) {return !pt.isAlive(); });
		if (prt != m_particles.end())
			prt->reset(systemCenter, dir, 0, m_lifeLength, 0, 0, num_rows_in_texture);
	}
}

//--------------------------------------------------------------------------------------
glm::vec3 ParticleSystem::_calculateParticles()
{
	glm::vec3 dir(0.0);
	float theta, phi;
	float angle = ((float)(1 + rand() % 100)) / 100;
	theta = glm::mix(0.0f, PI / 6.0f, angle);
	phi = glm::mix(0.0f, PI * 2, angle);
	dir.x = (sinf(theta)*cosf(phi)) / 50;// (30 + (1 + rand() % 20));
	dir.y = (cosf(theta)) / 50;// (30 + (1 + rand() % 20));
	dir.z = (sinf(theta) * sinf(phi)) / 50;// (30 + (1 + rand() % 20));

	return dir;
}

//-----------------------------------------------------------------------------------
std::vector<Particle>::iterator ParticleSystem::PrepareParticles(glm::vec3 _cameraPosition) 
{
	// sort alive or not
	std::vector<Particle>::iterator n_end = m_particles.begin();
	
	//Find last alive and set distance
	while(n_end != m_particles.end() && n_end->isAlive()) 
	{
		n_end->setDistance(glm::length(glm::vec3(_cameraPosition - n_end->getPosition())));
		++n_end;
	}
	std::sort(m_particles.begin(), n_end, [](Particle& prt1, Particle& prt2) { return prt1.getDistance() > prt2.getDistance(); });
	return n_end;
}

//-------------------------------------------------------------------------------------
void ParticleSystem::Update()
{
	GenerateParticles();
	for (auto& prt : m_particles) 
	{
		prt.Update();
	}
	std::sort(m_particles.begin(), m_particles.end(), 
			  [](Particle& prt1, Particle& prt2) 
			{ return prt1.isAlive() > prt2.isAlive(); });
}
