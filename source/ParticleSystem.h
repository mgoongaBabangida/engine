#pragma once
#include "Particle.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include "Clock.h"
#include <memory>
#include "InterfacesDB.h"
#include "Sound.h"
#include "Timer.h"

//class QTimer;
//FireWork

class ParticleSystem:public IParticleSystem
{
protected:
	int m_pps; //particles per second
	float m_speed;
	float m_gravityComplient;
	float m_lifeLength;
	std::vector<Particle> m_particles;
	Clock clock;
	//QTimer* timer;
	std::unique_ptr<dbb::Timer> timer;
	std::unique_ptr<remSnd> sound;
	float duration;
	virtual void emitParticles(glm::vec3 systemCenter, Texture* texture);
public:
	ParticleSystem(float pps, float speed, float gravityComplient, float lifeLength, remSnd* s, float dur=10000.0f);
	static const int MAX_PARTICLES = 1000;
	virtual void generateParticles(glm::vec3 systemCenter, Texture* texture);
	virtual std::vector<Particle>::iterator prepareParticles(glm::vec3 cameraPosition);
	virtual std::vector<Particle>& getParticles() { return  m_particles; }
	virtual ~ParticleSystem() {}
protected:
	virtual void Update();
};
