#pragma once

#include "MyMesh.h"

//--------------------------------------------------------------------------------------------------------------
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
	void										MakePlaneIndices(unsigned int dimensions);

	void										MakePlaneVerts(unsigned int dimensions, bool spreed_texture = true);
	void										MakePlaneVerts(unsigned int rows, unsigned int columns, bool spreed_texture = true);

	Texture*								GenerateNormals(GLuint rows, GLuint columns);
	Texture*								GenerateNormals(GLuint size);

	void										GenerateTessellationData();
	void										DrawTessellated();
  std::optional<Vertex>		FindVertex(float x, float z);

	GLuint		m_size		= 0;
	GLuint		m_rows		= 0;
	GLuint		m_columns = 0;
protected:
	Texture				m_heightMap;
	Texture				m_normalMap;
  unsigned int	m_devisor = 10;
	glm::ivec2		m_position;
	glm::vec2			m_world_offset;

	float m_minX = 1'000'000, m_minZ = 1'000'000, m_maxX = -1'000'000, m_maxZ = -1'000'000;

	struct TessellationData
	{
		unsigned int m_terrainVAO, m_terrainVBO;
		std::vector<float> m_vertices;
		TessellationData::~TessellationData()
		{
			glDeleteVertexArrays(1, &m_terrainVAO);
			glDeleteBuffers(1, &m_terrainVBO);
		}
	};
	TessellationData m_tessellation_data;
};
