#pragma once
#include "ParticleSystem.h"
#include "Particle.h"
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <vector>
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
