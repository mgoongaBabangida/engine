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

//FireWork

class ShootingParticleSystem :public ParticleSystem
{
protected:

	virtual void emitParticles(glm::vec3 systemCenter, Texture* texture) override;
public:
	ShootingParticleSystem(float pps, float speed, float gravityComplient, float lifeLength, remSnd* s, float dur = 10000.0f);
	static const int MAX_PARTICLES = 1000;
	//virtual void generateParticles(glm::vec3 systemCenter, Texture* texture) override;
};
