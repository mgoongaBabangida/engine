#pragma once

#include "MyMesh.h"

//----------------------------------------------------
class TerrainMesh : public MyMesh
{
public:
  explicit TerrainMesh(const std::string& _name);

	glm::ivec2							GetPosition() const { return m_position; }

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
};
