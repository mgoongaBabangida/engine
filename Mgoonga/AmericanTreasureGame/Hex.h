#pragma once

#include <math/PlaneLine.h>
#include <vector>
#include <memory>
#include <math.h>

static const float	common_height = 2.01f;
static const float	radius = 0.5f;

class eTerrain;

//---------------------------------------------------------------------
class Hex
{
public:
	Hex(glm::vec2 c) : m_center( c ) {}
	float x() const { return m_center.x; }
	float z() const { return m_center.y; }

	bool IsOn(float x, float z);
	bool IsWater(std::shared_ptr<eTerrain> terrain, float waterHeight);
	std::vector<Hex*> MakePath(Hex*);
	void SetNeighbour(std::vector<Hex>&);
	void Debug();

protected:
	glm::vec2	m_center;
	Hex*		up			= nullptr;
	Hex*		up_right	= nullptr;
	Hex*		up_left		= nullptr;
	Hex*		down		= nullptr;
	Hex*		down_right	= nullptr;
	Hex*		down_left	= nullptr;
};