#include "stdafx.h"

#include "Particle.h"
#include "Bezier.h"

float		Particle::GRAVITY = -0.000'098f;

//----------------------------------------------------------------
void Particle::_updateTextureCoordInfo()
{
	if (m_alive)
	{
		float lifeFactor = m_elapsedTime / m_lifelength;
		if (lifeFactor > 1.0f)
			return;
		int stagecount = numRowsInTexture * numRowsInTexture;
		float atlasProgression = lifeFactor * stagecount;
		int index1 = (int)glm::floor(atlasProgression);
		int index2 = index1 == stagecount - 1 ? index1 : index1 + 1;
		blend = atlasProgression - index1;

		_setTextureOffset(texOffset1, index1);
		_setTextureOffset(texOffset2, index2);
	}
}

//----------------------------------------------------------------
void Particle::_setTextureOffset(glm::vec2& offset, int index)
{
	int column	= index % numRowsInTexture;
	int row		= index / numRowsInTexture;
	offset.x	= (float)column/(float)numRowsInTexture;
	offset.y	= (float)row / (float)numRowsInTexture;
}

//----------------------------------------------------------------
bool Particle::Update(float _elapsed)
{
	if (!m_alive)
		return false;

	//@todo update scale, speed and other over lifetime with formula
	// if not timer should be different
	m_velocity.y += GRAVITY * m_gravityEffect;//  *(m_elapsedTime / 10); //el time 0-5
	m_position += m_velocity;
	m_elapsedTime += _elapsed;

	m_alive = m_elapsedTime < m_lifelength;
	if(m_alive)
		_updateTextureCoordInfo();
	return m_alive;
}

//----------------------------------------------------------------
const float Particle::getScale()
{
	if (m_scale_curve != nullptr)
	{
		float lifeFactor = glm::clamp(m_elapsedTime / m_lifelength, 0.0f, 1.0f);
		return dbb::GetPoint(*m_scale_curve, lifeFactor).y * m_scale;
	}
	else
		return m_scale;
}
