#pragma once

#include "MyMesh.h"

//----------------------------------------------------
class TerrainMesh : public MyMesh
{
public:
  explicit TerrainMesh(const std::string& _name);

	glm::ivec2							GetPosition() const { return m_position; }
	void										SetPosition(glm::ivec2 _pos) { m_position = _pos; }

	glm::vec2								GetWorlOffset() const { return m_world_offset; }
	void										SetWorldOffset(glm::vec2 _pos) { m_world_offset = _pos; }

	void										AssignHeights(const Texture& heightMap, float _height_scale = 1.0f, float _max_height = 1.0f);
	void										MakePlaneIndices(unsigned int rows, unsigned int columns, unsigned int _lod = 1);

	void										MakePlaneVerts(unsigned int dimensions, bool spreed_texture = true);
	void										MakePlaneVerts(unsigned int rows, unsigned int columns, bool spreed_texture = true);
	void										MakePlaneIndices(unsigned int dimensions);
	Texture*								GenerateNormals(GLuint size);
	Texture*								GenerateNormals(GLuint rows, GLuint columns);
  std::optional<Vertex>		FindVertex(float x, float z);

	GLuint		m_size		= 0;
	GLuint		m_rows		= 0;
	GLuint		m_columns = 0;
protected:
	Texture				m_normalMap;
  unsigned int	m_devisor = 10;
	glm::ivec2		m_position;
	glm::vec2			m_world_offset;

	float m_minX = 1'000'000, m_minZ = 1'000'000, m_maxX = -1'000'000, m_maxZ = -1'000'000;
};
