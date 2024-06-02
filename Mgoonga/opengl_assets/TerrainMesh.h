#pragma once

#include "MyMesh.h"

class Camera;

//--------------------------------------------------------------------------------------------------------------
class TerrainMesh : public MyMesh
{
public:
  explicit TerrainMesh(const std::string& _name);

	virtual void						Draw()			override;

	void										DrawTessellated(std::function<void(const TessellationRenderingInfo&)> _info_updater);
	void										GenerateTessellationData();

	glm::ivec2							GetPosition() const { return m_position; }
	void										SetPosition(glm::ivec2 _pos) { m_position = _pos; }

	glm::vec2								GetWorlOffset() const { return m_world_offset; }
	void										SetWorldOffset(glm::vec2 _pos) { m_world_offset = _pos; }

	void										SetCamera(Camera* _camera);
	void										SetTessellationRenderingInfo(const TessellationRenderingInfo&);

	std::vector<glm::mat3>	GetBoundingTriangles() const;
	std::vector<glm::vec3>	GetExtrems() const;
	glm::vec3								GetCenter() const;

	void										AssignHeights(const Texture& heightMap, float _height_scale = 1.0f, float _max_height = 1.0f, float _min_height = 0.0f, int32_t _normal_sharpness = 10, bool _apply_normal_blur = false);

	void										MakePlaneIndices(unsigned int rows, unsigned int columns, unsigned int _lod = 1);
	void										MakePlaneIndices(unsigned int dimensions);

	void										MakePlaneVerts(unsigned int dimensions, bool spreed_texture = true);
	void										MakePlaneVerts(unsigned int rows, unsigned int columns, bool spreed_texture = true);

	Texture*								GenerateNormals(GLuint rows, GLuint columns);
	Texture*								GenerateNormals(GLuint size);

	GLuint									GetNormalMapId() const { return m_normalMap.m_id; }

	float					GetNormalSigma() const { return m_normal_sigma; }
	int32_t				GetKernelSize() const { return m_kernel_size; }

	void					SetNormalSigma(float _sigma)  { m_normal_sigma = _sigma; }
	void					SetKernelSize(int32_t _kernel) { m_kernel_size = _kernel; }

  std::optional<Vertex>		FindVertex(float x, float z);

	GLuint		Size() const { return m_size; }
	GLuint		Rows() const { return m_rows; }
	GLuint		Columns() const { return m_columns; }

protected:
	void _GenerateNormalMap(const GLfloat* _heightmap, unsigned int _width, unsigned int _height, int32_t normal_sharpness, bool _apply_normal_blur = false);

	void _SmoothNormals(std::vector<float>& normalMap, int width, int height);

	Texture				m_heightMap;
	Texture				m_normalMap;

	GLuint		m_size = 0;
	GLuint		m_rows = 0;
	GLuint		m_columns = 0;

	float					m_normal_sigma = 2.0f;
	int32_t				m_kernel_size = 5;

  unsigned int	m_devisor = 10;
	glm::ivec2		m_position;
	glm::vec2			m_world_offset;
	float					m_LOD_Step = 1.5f;

	float m_minX = 1'000'000,
				m_minZ = 1'000'000,
				m_maxX = -1'000'000,
				m_maxZ = -1'000'000,
				m_minY = 1'000'000,
				m_maxY = -1'000'000;

	Camera* m_camera = nullptr;

	struct TessellationData
	{
		unsigned int m_terrainVAO, m_terrainVBO;
		std::vector<float> m_vertices;
		void Clear()
		{
			m_vertices.clear();
			glDeleteVertexArrays(1, &m_terrainVAO);
			glDeleteBuffers(1, &m_terrainVBO);
		}
		TessellationData::~TessellationData()
		{
			Clear();
		}
	};
	TessellationData m_tessellation_data;

	TessellationRenderingInfo m_tessellation_info;
};
