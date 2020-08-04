#pragma once

#include "ParticleSystem.h"

//--------------------------------------------------------------------
class DLL_MATH ShootingParticleSystem : public ParticleSystem
{
public:
	ShootingParticleSystem(float	 _pps,
						   float	 _speed,
						   float	 _gravityComplient,
						   float	 _lifeLength,
						   glm::vec3 _systemCenter,
						   Texture*  _texture,
						   ISound*	 _sound,
						   float	 _duration = 10000.0f);
	virtual ~ShootingParticleSystem();
protected:
	virtual glm::vec3 _calculateParticles() override;
};
