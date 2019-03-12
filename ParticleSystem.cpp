#include "ParticleSystem.h"
#include <algorithm>
#include "Texture.h"
#include "Structures.h"
#include <chrono>
#include <iostream>
#include <random>
#include "Sound.h"

ParticleSystem::ParticleSystem(float pps, float speed, float gravityComplient, float lifeLength, remSnd* s, float dur):duration(dur) {
	m_pps = pps;
	m_speed = speed;
	m_gravityComplient = gravityComplient;
	m_lifeLength = lifeLength;
	sound.reset(s);

	m_particles.reserve(MAX_PARTICLES);

	clock.start();
	if(sound!=nullptr)
	 sound->Play();

	timer.reset(new dbb::Timer([this]()->bool {this->Update(); return true; }));
	timer->start(100);

	srand(time(0));
}

void ParticleSystem::generateParticles(glm::vec3 systemCenter, Texture* texture)
{
	if(clock.timeEllapsedMsc() < this->duration)
	{
		int msc = clock.newFrame();
		float new_particles = (float)msc / 1000.0f * (float)m_pps;
		for (int i = 0; i < new_particles; ++i)
			emitParticles(systemCenter, texture);
	}
	else
	{
		if (sound->isPlaying())
		{
			sound->Stop();
		}
	}
}

void ParticleSystem::emitParticles(glm::vec3 systemCenter, Texture* texture)
{
	glm::vec3 dir(0.0);
	float theta, phi;
	float angle = ((float)(1 + rand() % 100)) / 100;
	theta = glm::mix(0.0f, str::PI / 6.0f, angle);
	phi = glm::mix(0.0f, str::PI * 2, angle);
	dir.x = (sinf(theta)*cosf(phi)) / 50;// (30 + (1 + rand() % 20));
	dir.y = (cosf(theta)) / 50;// (30 + (1 + rand() % 20));
	dir.z = (sinf(theta) * sinf(phi)) / 50;// (30 + (1 + rand() % 20));
										   //for(int i=0;i<50;++i)

	
	

	if (m_particles.size() < MAX_PARTICLES)
		m_particles.push_back(Particle(systemCenter, dir, 0, m_lifeLength, 0, 0, texture));
	else 
	{
		auto prt = std::find_if(m_particles.begin(), m_particles.end(), [](Particle& pt) {return !pt.isAlive(); });
		if (prt != m_particles.end())
			prt->reset(systemCenter, dir, 0, m_lifeLength, 0, 0, texture);
	}
}

std::vector<Particle>::iterator ParticleSystem::prepareParticles(glm::vec3 cameraPosition ) {

	// sort alive or not
	std::sort(m_particles.begin(), m_particles.end(), [](Particle& prt1, Particle& prt2) {return prt1.isAlive() > prt2.isAlive(); });
	std::vector<Particle>::iterator n_end = m_particles.begin();
	//find last alive and set distance
	while (n_end != m_particles.end() && n_end->isAlive()) {
		n_end->setDistance(glm::length(glm::vec3(cameraPosition - n_end->getPosition())));
		++n_end;
	}

	std::sort(m_particles.begin(), n_end, [](Particle& prt1, Particle& prt2) {return prt1.getDistance() < prt2.getDistance(); });

	return n_end;
}

void ParticleSystem::Update()
{
	for (auto& prt : m_particles) {
		prt.Update();
	}
}
