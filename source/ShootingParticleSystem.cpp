#include "ShootingParticleSystem.h"
#include <algorithm>
#include "Texture.h"
#include "Structures.h"
#include "Random.h"



void ShootingParticleSystem::emitParticles(glm::vec3 systemCenter, Texture * texture)
{
		glm::vec3 dir(
			Random::RandomFloat(0.0f,1.0f)  * 2.0f - 1.0f,
			Random::RandomFloat(0.0f, 1.0f) * 2.0f - 1.0f,
			Random::RandomFloat(0.0f, 1.0f)  * 2.0f - 1.0f
		);
		dir = glm::normalize(dir) / Random::RandomFloat(300.0f, 600.0f);

	if (m_particles.size() < MAX_PARTICLES)
		m_particles.push_back(Particle(systemCenter, dir, 0, m_lifeLength, 0, 0, texture));
	else 
	{
		auto prt = std::find_if(m_particles.begin(), m_particles.end(), [](Particle& pt) {return !pt.isAlive(); });
		if (prt != m_particles.end())
			prt->reset(systemCenter, dir, 0, m_lifeLength, 0, 0, texture);
	}

}

ShootingParticleSystem::ShootingParticleSystem(float pps, float speed, float gravityComplient, float lifeLength, remSnd* s,float dur)
	:ParticleSystem(pps, speed, gravityComplient, lifeLength, s,dur)
{
	
}

//void ShootingParticleSystem::generateParticles(glm::vec3 systemCenter, Texture * texture)
//{
//	if (clock.timeEllapsedMsc() < this->duration)
//	{
//		int msc = clock.newFrame();
//		float new_particles = (float)msc / 1000.0f * (float)m_pps;
//		for (int i = 0; i < new_particles; ++i)
//			emitParticles(systemCenter, texture);
//	}
//	else
//	{
//		if (sound->isPlaying())
//		{
//			sound->Stop();
//		}
//	}
//}


