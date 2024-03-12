#pragma once

#include <base/Object.h>
#include <math/Geometry.h>
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
	bool IsTaken() const { return m_taken; }
	void SetTaken(bool _taken) { m_taken = _taken; }
	bool IsBase() const;
	void SetBase(bool _taken) { m_base = _taken; }

	bool CanBeTaken(shObject _terrain, float _waterHeight, bool _can_go_to_base);

	std::deque<Hex*> MakePath(const Hex* _destination, shObject _terrain, float _waterHeight, bool _can_go_to_base = false);

	void SetNeighbour(std::vector<Hex>&);
	void Debug();

protected:
	static void _PathRootOne(Hex*& _cur, Hex* _prev, const Hex* _destination, shObject terrain, float waterHeight, bool _can_go_to_base = false);
	static void _PathRootTwo(Hex*& _cur, Hex* _prev, const Hex* _destination, shObject terrain, float waterHeight, bool _can_go_to_base = false);

	glm::vec2	m_center;
	Hex*			up			= nullptr;
	Hex*			up_right	= nullptr;
	Hex*			up_left		= nullptr;
	Hex*			down		= nullptr;
	Hex*			down_right	= nullptr;
	Hex*			down_left	= nullptr;

	bool			m_taken = false;
	bool			m_base = false;
};