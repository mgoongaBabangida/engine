#include "stdafx.h"
#include "ShootingParticleSystem.h"
#include <base/base.h>
#include "Random.h"
#include <algorithm>

//----------------------------------------------------------------------------------------------------
ShootingParticleSystem::ShootingParticleSystem(float	  _pps,
												float	  _speed,
												float	  _gravityComplient,
												float	  _lifeLength,
												glm::vec3 _systemCenter,
												const Texture*  _texture,
												ISound*	  _sound,
												float	  _duration)
: ParticleSystem(_pps,_speed, _gravityComplient, _lifeLength,_systemCenter,_texture,_sound, static_cast<int64_t>(_duration))
{}

ShootingParticleSystem::~ShootingParticleSystem()
{
}

//------------------------------------------------------------------------------------------------
glm::vec3 ShootingParticleSystem::_calculateParticles()
{
	glm::vec3 dir(
		math::Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
		math::Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
		math::Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f
	);
	return glm::normalize(dir) / math::Random::RandomFloat(300.0f, 600.0f);
}


