#include "stdafx.h"
#include "Hex.h"

#include <algorithm>

const float	Hex::common_height = 2.01f;
const float	Hex::radius = 0.5f;

//-------------------------------------------------------------------------------------
bool Hex::IsOn(float x, float z)
{
	glm::vec3 center = glm::vec3(m_center.x, common_height, m_center.y);

	/*float radius = radius * 0.57f;
	std::vector<glm::vec3> dots{
	glm::vec3(glm::cos(glm::radians(30.0f)) * radius,	common_height, glm::sin(glm::radians(30.0f)) * radius),
	glm::vec3(glm::cos(glm::radians(90.0f)) * radius,	common_height, glm::sin(glm::radians(90.0f)) * radius),
	glm::vec3(glm::cos(glm::radians(150.0f)) * radius,	common_height, glm::sin(glm::radians(150.0f)) * radius),
	glm::vec3(glm::cos(glm::radians(210.0f)) * radius,	common_height, glm::sin(glm::radians(210.0f)) * radius),
	glm::vec3(glm::cos(glm::radians(270.0f)) * radius,	common_height, glm::sin(glm::radians(270.0f)) * radius),
	glm::vec3(glm::cos(glm::radians(330.0f)) * radius,	common_height, glm::sin(glm::radians(330.0f)) * radius),
	glm::vec3(glm::cos(glm::radians(30.0f)) * radius,	common_height, glm::sin(glm::radians(30.0f)) * radius)
	};*/

	std::vector<glm::vec3> m_dots{
		glm::vec3(0.866,	0.0,	0.5)	* radius * 0.57f, //@todo magic numbers
		glm::vec3(0.0,		0.0,	1.0)	* radius * 0.57f,
		glm::vec3(-0.866, 0.0,	0.5)	* radius * 0.57f,
		glm::vec3(-0.866, 0.0, -0.5)	* radius * 0.57f,
		glm::vec3(0.0,		0.0, -1.0)	* radius * 0.57f,
		glm::vec3(0.866,	0.0, -0.5)	* radius * 0.57f,
		glm::vec3(0.866,	0.0,	0.5)	* radius * 0.57f
	};

	for (int i = 0; i < 6; ++i)
	{
		glm::mat3  triangle(center, center + m_dots[i], center + m_dots[i + 1]);
		if (dbb::IsInside(triangle, glm::vec3(x, common_height, z)))
			return true;
	}
	return false;
}

//------------------------------------------------------------------------------------
bool Hex::IsWater(shObject _terrain, float _waterHeight)
{
	glm::vec4 hex_transformed = glm::inverse(_terrain->GetTransform()->getModelMatrix())
								* glm::vec4{ m_center.x, _waterHeight, m_center.y, 1.0f };
	ITerrainModel* terrain_model = dynamic_cast<ITerrainModel*>(_terrain->GetModel());
	if (!terrain_model)
		return false;
	return terrain_model->GetHeight(hex_transformed.x, hex_transformed.z) < hex_transformed.y;
}

//------------------------------------------------------------------------------------------
bool Hex::IsBase() const
{
	return m_base;
}

//------------------------------------------------------------------------------------------
bool Hex::CanBeTaken(shObject _terrain, float _waterHeight, bool _can_go_to_base)
{
	if(!_can_go_to_base)
		return this->IsWater(_terrain, _waterHeight) && !this->IsTaken();
	else
		return (this->IsWater(_terrain, _waterHeight) || this->IsBase()) && !this->IsTaken();
}

//------------------------------------------------------------------------------------------
std::deque<Hex*> Hex::MakePath(const Hex* _destination, shObject _terrain, float _waterHeight, bool _can_go_to_base)
{
	std::deque<Hex*> path1, path2;
	if (_destination == this)
		return path1;
	Hex* cur = this;
	Hex* prev = nullptr;
	while (cur != _destination)
	{
		Hex* temp = cur;
		_PathRootOne(cur, prev, _destination, _terrain, _waterHeight, _can_go_to_base);
		prev = temp;

		if (path1.size() > 20)
		{
			std::cout << "first path missed" << std::endl; //@todo
			break;
		}

		if (cur)
			path1.push_back(cur);
		else
			return{};// could not get path, got nowhere
	}

	cur = this;
	while (cur != _destination)
	{
		Hex* temp = cur;
		_PathRootTwo(cur, prev, _destination, _terrain, _waterHeight, _can_go_to_base);
		prev = temp;

		if (path2.size() > 20)
		{
			std::cout << "second path missed" << std::endl;//@todo
			break;
		}

		if (cur)
			path2.push_back(cur);
		else
			return{};// could not get path, got nowhere
	}

	return path1.size() < path2.size() ? path1 : path2;
}

//------------------------------------------------------------------------------------------
void Hex::SetNeighbour(std::vector<Hex>& hexes)
{
	std::vector<Hex>::iterator neighbour = std::find_if(hexes.begin(), hexes.end(), [this](Hex& other)
		{ return fabs(other.x() - x() - radius)  < 0.01f && fabs(other.z() - (z())) < 0.01f; });

	if (neighbour != hexes.end())
		up = &(*neighbour);
	
	neighbour = std::find_if(hexes.begin(), hexes.end(), [this](Hex& other)
		{ return fabs(other.x() - x() + radius) < 0.01f  && fabs(other.z() - (z())) < 0.01f; });
	
	if(neighbour != hexes.end())
		down = &(*neighbour);
	
	neighbour = std::find_if(hexes.begin(), hexes.end(), [this](Hex& other)
		{ return fabs(other.x()  - (x() + glm::cos(glm::radians(60.0f)) * radius)) < 0.01f && fabs(other.z()  - (z() + glm::sin(glm::radians(60.0f)) * radius)) < 0.01f ;});
	
	if (neighbour != hexes.end())
		up_right = &(*neighbour);

	neighbour = std::find_if(hexes.begin(), hexes.end(), [this](Hex& other) { return fabs(other.x() - (x() + glm::cos(glm::radians(120.0f)) * radius)) < 0.01f
																				  && fabs(other.z() - (z() + glm::sin(glm::radians(120.0f)) * radius)) < 0.01f; });
	if (neighbour != hexes.end())
		down_right = &(*neighbour);

	neighbour = std::find_if(hexes.begin(), hexes.end(), [this](Hex& other)
		{ return fabs(other.x() - (x() + glm::cos(glm::radians(240.0f)) * radius)) < 0.01f && fabs(other.z() - (z() + glm::sin(glm::radians(240.0f)) * radius)) < 0.01f; });
	
	if (neighbour != hexes.end())
		down_left = &(*neighbour);

	neighbour = std::find_if(hexes.begin(), hexes.end(), [this](Hex& other)
		{ return fabs(other.x() - (x() + glm::cos(glm::radians(300.0f)) * radius)) < 0.01f && fabs(other.z() - (z() + glm::sin(glm::radians(300.0f)) * radius)) < 0.01f; });
	
	if (neighbour != hexes.end())
		up_left = &(*neighbour);
}

//----------------------------------------------------------------------------------------------------------
void Hex::Debug()
{
	std::cout << "HEX " << std::endl;
	std::cout << "Center x= " << m_center.x << " Center y = " << m_center.y << std::endl;
	if(up)			std::cout << "UP x= " << up->m_center.x << " UP y = " << up->m_center.y << std::endl;
	if(down)		std::cout << "Down x= " << down->m_center.x << " Down y = " << down->m_center.y << std::endl;
	if(up_right)	std::cout << "up_right x= " << up_right->m_center.x << " up_right y = " << up_right->m_center.y << std::endl;
	if (up_left)	std::cout << "up_left x= " << up_left->m_center.x << " up_left y = " << up_left->m_center.y << std::endl;
	if (down_right) std::cout << "down_right x= " << down_right->m_center.x << " down_right y = " << down_right->m_center.y << std::endl;
	if (down_left)	std::cout << "down_left x= " << down_left->m_center.x << " down_left y = " << down_left->m_center.y << std::endl;
	 std::cout << "----------------------------------------------" << std::endl;
}

//----------------------------------------------------------------
void Hex::_PathRootOne(Hex*& cur, Hex* _prev, const Hex* _destination, shObject _terrain, float _waterHeight, bool _can_go_to_base)
{
	if (_destination->m_center.x > cur->m_center.x && _destination->m_center.y == cur->m_center.y) //up
	{
		if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x < cur->m_center.x && _destination->m_center.y == cur->m_center.y) // down
	{
		if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up != _prev)
			cur = cur->up;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x > cur->m_center.x && _destination->m_center.y > cur->m_center.y) // up right
	{
		if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x > cur->m_center.x && _destination->m_center.y < cur->m_center.y) // up left
	{
		if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x <= cur->m_center.x && _destination->m_center.y > cur->m_center.y) // down right / right
	{
		if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->up != _prev)
			cur = cur->up;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x <= cur->m_center.x && _destination->m_center.y < cur->m_center.y) // down left/ left
	{
		if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else
			cur = _prev;
	}
}

//----------------------------------------------------------------
void Hex::_PathRootTwo(Hex*& cur, Hex* _prev, const Hex* _destination, shObject _terrain, float _waterHeight, bool _can_go_to_base)
{
	if (_destination->m_center.x > cur->m_center.x && _destination->m_center.y == cur->m_center.y) //up
	{
		if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x < cur->m_center.x && _destination->m_center.y == cur->m_center.y) // down
	{
		if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x > cur->m_center.x && _destination->m_center.y > cur->m_center.y) // up right
	{
		if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base)  && cur->down != _prev)
			cur = cur->down;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x > cur->m_center.x && _destination->m_center.y < cur->m_center.y) // up left
	{
		if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x <= cur->m_center.x && _destination->m_center.y > cur->m_center.y) // down right / right
	{
		if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else
			cur = _prev;
	}
	else if (_destination->m_center.x <= cur->m_center.x && _destination->m_center.y < cur->m_center.y) // down left/ left
	{
		if (cur->down_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_left != _prev)
			cur = cur->down_left;
		else if (cur->down->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down != _prev)
			cur = cur->down;
		else if (cur->up_left->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_left != _prev)
			cur = cur->up_left;
		else if (cur->up->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up != _prev)
			cur = cur->up;
		else if (cur->down_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->down_right != _prev)
			cur = cur->down_right;
		else if (cur->up_right->CanBeTaken(_terrain, _waterHeight, _can_go_to_base) && cur->up_right != _prev)
			cur = cur->up_right;
		else
			cur = _prev;
	}
}
