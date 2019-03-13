#include "stdafx.h"
#include "Particle.h"
#include "Texture.h"

void Particle::updateTextureCoordInfo()
{
	float lifeFactor = m_elapsedTime / m_lifelength;
	int stagecount = tex->numberofRows * tex->numberofRows; //16
	float atlasProgression = lifeFactor * stagecount;
	int index1 = glm::floor(atlasProgression); 
	int index2 = index1 == stagecount-1 ? index1 : index1+1;
	this->blend = atlasProgression - index1;
	setTextureOffset(texOffset1, index1);
	setTextureOffset(texOffset2, index2);
}

void Particle::setTextureOffset(glm::vec2& offset, int index)
{
	int column = index % tex->numberofRows;
	int row = index / tex->numberofRows;
	offset.x = (float)column/(float) tex->numberofRows ;
	offset.y = (float)row / (float)tex->numberofRows ;
}

bool Particle::Update()
{
	// if not timer should be different
	m_velocity.y += GRAVITY * m_gravityEffect;//  *(m_elapsedTime / 10); //el time 0-5
	m_position += m_velocity;
	m_elapsedTime += 100; // !!!Hardcode 0.1 second per call

	updateTextureCoordInfo();
	alive = m_elapsedTime < m_lifelength;
	/*if (!alive)
		m_position =glm::vec3(1000, -1000,1000);*/
	return alive;
}
