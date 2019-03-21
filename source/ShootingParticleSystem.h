#pragma once
#include "ParticleSystem.h"
#include "Particle.h"
#include "Clock.h"
#include "InterfacesDB.h"
#include "Sound.h"

class ShootingParticleSystem :public ParticleSystem
{
public:
	ShootingParticleSystem(float	 _pps,
						   float	 _speed,
						   float	 _gravityComplient,
						   float	 _lifeLength,
						   glm::vec3 _systemCenter,
						   Texture*  _texture,
						   remSnd*	 _sound,
						   float	 _duration = 10000.0f);
protected:
	virtual void emitParticles() override;
};
