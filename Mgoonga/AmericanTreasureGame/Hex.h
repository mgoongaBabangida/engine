#pragma once

#include <base/Object.h>
#include <math/PlaneLine.h>
#include <math.h>

#include <deque>

class eTerrain;

//---------------------------------------------------------------------
class Hex
{
public:
	static const float	common_height;
	static const float	radius;

	Hex(glm::vec2 c) : m_center( c ) {}
	float x() const { return m_center.x; }
	float z() const { return m_center.y; }

	bool IsOn(float x, float z);
	bool IsWater(shObject terrain, float waterHeight);
	std::deque<Hex*> MakePath(const Hex* _destination, shObject _terrain, float _waterHeight);
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