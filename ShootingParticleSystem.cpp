#include "ShootingParticleSystem.h"
#include <algorithm>
#include "Texture.h"
#include "Structures.h"
#include "Random.h"

ShootingParticleSystem::ShootingParticleSystem(float	  _pps,
												float	  _speed,
												float	  _gravityComplient,
												float	  _lifeLength,
												glm::vec3 _systemCenter,
												Texture*  _texture,
												remSnd*	  _sound,
												float	  _duration)
: ParticleSystem(_pps,_speed, _gravityComplient, _lifeLength,_systemCenter,_texture,_sound, _duration) {}

void ShootingParticleSystem::emitParticles()
{
		glm::vec3 dir(
			Random::RandomFloat(0.0f,1.0f)  * 2.0f - 1.0f,
			Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
			Random::RandomFloat(0.0f, 1.0f)  * 2.0f - 1.0f
		);
		dir = glm::normalize(dir) / Random::RandomFloat(300.0f, 600.0f);

	if (m_particles.size() < MAX_PARTICLES)
		m_particles.push_back(Particle(systemCenter, dir, 0, m_lifeLength, 0, 0, texture->numberofRows));
	else 
	{
		auto prt = std::find_if(m_particles.begin(), m_particles.end(), [](Particle& pt) {return !pt.isAlive(); });
		if (prt != m_particles.end())
			prt->reset(systemCenter, dir, 0, m_lifeLength, 0, 0, texture->numberofRows);
	}

}


