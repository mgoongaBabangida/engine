#include "stdafx.h"
#include "ParticleSystem.h"
#include "Random.h"
#include <base/base.h>

#include <algorithm>
#include <chrono>
#include <random>

//------------------------------------------------------------------------------------
ParticleSystem::ParticleSystem(float	 _pps,
															 float	 _speed,
															 float	 _gravityComplient,
															 float	 _lifeLength,
															 glm::vec3 _systemCenter,
															 const Texture*  _texture,
															 ISound*	 _sound,
															 size_t	 _num_rows_in_texture,
															 float	 _duration)
: IParticleSystem(_texture)
, m_duration(_duration)
, m_pps(static_cast<uint32_t>(_pps))
, m_speed(_speed)
, m_scale(0.05f, 0.05f, 0.05f)
, m_gravity_complient(_gravityComplient)
, m_life_length(_lifeLength / 1000)
, m_system_center(_systemCenter)
, sound(_sound)
, num_rows_in_texture(_num_rows_in_texture)
, m_particles(MAX_PARTICLES, Particle{})
{
  m_particles.resize(MAX_PARTICLES);
}

//--------------------------------------------------------------------------------------
ParticleSystem::~ParticleSystem()
{
	if (timer)
		timer->stop();
}

//-------------------------------------------------------------------------
void ParticleSystem::Start()
{
	if (timer)
		timer->stop();

	timer.reset(new math::Timer([this]()->bool { this->Update(); return true; }));

	clock.restart();

	if (sound)
		sound->Play();

	timer->start(40);
	srand(time(0));
}

//-------------------------------------------------------------------------
void ParticleSystem::GenerateParticles()
{
	if(clock.timeEllapsedMsc() < m_duration || m_loop)
	{
		int64_t msc = clock.newFrame();
		float new_particles = (float)msc / 1000.0f * (float)m_pps;
		for (int i = 0; i < new_particles; ++i)
			_emitParticles();
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
void ParticleSystem::_emitParticles()
{
	glm::vec3 dir = _calculateParticles();
	glm::vec3 origin = m_system_center;
	if (m_base_radius > 0.0f)
	{
		origin.x = origin.x + math::Random::RandomFloat(-m_base_radius, m_base_radius);
		origin.z = origin.z + math::Random::RandomFloat(-m_base_radius, m_base_radius);
	}

	if (cur_particles < MAX_PARTICLES)
	{
		m_particles[cur_particles].reset(origin, dir, m_gravity_complient, m_life_length * 1000, 0, 0, static_cast<uint32_t>(num_rows_in_texture));
		++cur_particles;
	}
	else
	{
		auto prt = std::find_if(m_particles.begin(), m_particles.end(), [](Particle& pt) {return !pt.isAlive(); });
		if (prt != m_particles.end())
			prt->reset(origin, dir, m_gravity_complient, m_life_length * 1000, 0, 0, static_cast<uint32_t>(num_rows_in_texture));
	}
}

//--------------------------------------------------------------------------------------
glm::vec3 ParticleSystem::_calculateParticles()
{
	if (m_type == CONE)
	{
		float theta = glm::mix(0.0f, m_cone_angle,	math::Random::RandomFloat(0.0f, 1.0f));
		float phi = glm::mix(0.0f, PI * 2.0f,				math::Random::RandomFloat(0.0f, 1.0f));

		glm::vec3 dir(0.0);
		dir.x = (sinf(theta) * cosf(phi)) * m_speed;	// (30 + (1 + rand() % 20));
		dir.y = (cosf(theta))							* m_speed;	// (30 + (1 + rand() % 20));
		dir.z = (sinf(theta) * sinf(phi)) * m_speed;	// (30 + (1 + rand() % 20));
		return dir;
	}
	else if (m_type == SPHERE) //@todo also use speed
	{
		glm::vec3 dir(
			math::Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
			math::Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
			math::Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f
		);
		return glm::normalize(dir) / math::Random::RandomFloat(300.0f, 600.0f);
	}
	assert("ParticleSystem::_calculateParticles");
	return {};
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

//---------------------------------------------------------------------------------------
bool ParticleSystem::IsFinished()
{
	if (!m_loop)
		return clock.timeEllapsedMsc() > m_duration + (m_life_length * 1000);
	else
		return false;
}

//------------------------------------------------------------------------------
bool ParticleSystem::IsStarted()
{
	return clock.isActive();
}

//-------------------------------------------------------------------------------------
void ParticleSystem::Reset()
{
	clock.reset();
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
