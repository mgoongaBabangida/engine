#include "stdafx.h"

#include "TerrainMesh.h"
#include <math/Camera.h>
#include <opengl_assets/GlDrawContext.h>

//---------------------------------------------------
TerrainMesh::TerrainMesh(const std::string& _name)
  : MyMesh(_name) , m_position(0, 0), m_world_offset(0, 0)
{
	/*m_normalMap = Texture::GetTexture1x1(TColor::BLUE);*/
}

//----------------------------------------------------------------------------
void TerrainMesh::Draw()
{
	if (m_camera && indicesLods.size() > 1)
	{
		glm::vec3 center = glm::vec3((m_maxX - (m_maxX - m_minX) / 2),
																  m_maxY,
																 (m_maxZ - (m_maxZ - m_minZ) / 2));
		float dist = glm::length(m_camera->getPosition() - center);
		for (size_t i = indicesLods.size(); i > 0; --i)
		{
			if (dist >= m_LOD_Step * i)
			{
				SwitchLOD(i);
				break;
			}
		}
	}
	MyMesh::Draw();
}

//----------------------------------------------------------------------------
void TerrainMesh::MakePlaneVerts(unsigned int _dimensions, bool _spreed_texture)
{
	m_rows = _dimensions;
	m_columns = _dimensions;
	m_size = _dimensions;
	this->vertices.resize(_dimensions * _dimensions);
	int half = _dimensions / 2;
	for (int i = 0; i < _dimensions; i++)
	{
		for (int j = 0; j < _dimensions; j++)
		{
			Vertex& thisVert = this->vertices[i * _dimensions + j];
			thisVert.Position.x = (float)(j - half + (static_cast<int>(_dimensions - 1) * m_position.x)) / m_devisor;
			thisVert.Position.z = (float)(i - half + (static_cast<int>(_dimensions - 1) * m_position.x)) / m_devisor;
			thisVert.Position.y = 0;
			thisVert.Normal = glm::vec3(0.0f, 1.0f, 0.0f);

			if (_spreed_texture)
			{
				thisVert.TexCoords.x = j / (float)_dimensions;
				thisVert.TexCoords.y = i / (float)_dimensions;
			}
			else
			{
				thisVert.TexCoords.x = j % 2 ? 0.0f : 1.0f;
				thisVert.TexCoords.y = i % 2 ? 0.0f : 1.0f;
			}
		}
	}
}

//---------------------------------------------------------------------------
void TerrainMesh::MakePlaneVerts(unsigned int _rows, unsigned int _columns, bool _spreed_texture)
{
	m_rows = _rows;
	m_columns = _columns;
	this->vertices.resize(_rows * _columns);
	int half_r = (_rows) / 2;
	int half_c = (_columns) / 2;
	int counter = 0;
	for (int i = 0; i < _columns; i++)
	{
		for (int j = 0; j < _rows; j++)
		{
			Vertex& thisVert = this->vertices[i * _rows + j];
			thisVert.Position.x = static_cast<float>(j - half_r + (static_cast<int>(_rows-1) * m_position.x)) / static_cast<float>(m_devisor);
			thisVert.Position.z = static_cast<float>(i - half_c + (static_cast<int>(_columns-1) * m_position.y)) / static_cast<float>(m_devisor);
			thisVert.Position.y = 0;
			thisVert.Normal = glm::vec3(0.0f, 1.0f, 0.0f);
			if (_spreed_texture)
			{
				thisVert.TexCoords.x = static_cast<float>(j) / static_cast<float>(_rows);
				thisVert.TexCoords.y = static_cast<float>(i) / static_cast<float>(_columns);
			}
			else
			{
				thisVert.TexCoords.x = j % 2 ? 0.0f : 1.0f;
				thisVert.TexCoords.y = i % 2 ? 1.0f : 0.0f;
			}
			if (thisVert.Position.x < m_minX)
				m_minX = thisVert.Position.x;
			if (thisVert.Position.x > m_maxX)
				m_maxX = thisVert.Position.x;
			if (thisVert.Position.z < m_minZ)
				m_minZ = thisVert.Position.z;
			if (thisVert.Position.z > m_maxZ)
				m_maxZ = thisVert.Position.z;
			counter++;
		}
	}
}

//---------------------------------------------------------------------------
void TerrainMesh::MakePlaneIndices(unsigned int _dimensions)
{
	this->indicesLods[0].resize((_dimensions) * (_dimensions) * 2 * 3);// 2 triangles per square, 3 indices per triangle dim-1???
	int runner = 0;
	for (int row = 0; row < _dimensions - 1; row++)
	{
		for (int col = 0; col < _dimensions - 1; col++) //order?
		{
			this->indicesLods[0][runner++] = _dimensions * row + col;
			this->indicesLods[0][runner++] = _dimensions * row + col + _dimensions;
			this->indicesLods[0][runner++] = _dimensions * row + col + _dimensions + 1;

			this->indicesLods[0][runner++] = _dimensions * row + col;
			this->indicesLods[0][runner++] = _dimensions * row + col + _dimensions + 1;
			this->indicesLods[0][runner++] = _dimensions * row + col + 1;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void TerrainMesh::SetCamera(Camera* _camera)
{
	m_camera = _camera;
}

//----------------------------------------------------------------------------------------------- -
void TerrainMesh::SetTessellationRenderingInfo(const TessellationRenderingInfo& _info)
{
	m_tessellation_info = _info;
}

//-----------------------------------------------------------------------------------------------
std::vector<glm::mat3> TerrainMesh::GetBoundingTriangles() const
{
	std::vector<glm::mat3> ret; // Getting 12 triangles of the bouning cube
	ret.push_back(glm::mat3(glm::vec3(m_maxX, m_maxY, m_minZ),
		glm::vec3(m_minX, m_maxY, m_maxZ),
		glm::vec3(m_maxX, m_maxY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_maxX, m_maxY, m_minZ),
		glm::vec3(m_minX, m_maxY, m_minZ),
		glm::vec3(m_minX, m_maxY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_maxX, m_minY, m_minZ),
		glm::vec3(m_minX, m_minY, m_maxZ),
		glm::vec3(m_maxX, m_minY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_maxX, m_minY, m_minZ),
		glm::vec3(m_minX, m_minY, m_minZ),
		glm::vec3(m_minX, m_minY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_minX, m_maxY, m_maxZ),
		glm::vec3(m_minX, m_minY, m_maxZ),
		glm::vec3(m_maxX, m_minY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_minX, m_maxY, m_maxZ),
		glm::vec3(m_maxX, m_minY, m_maxZ),
		glm::vec3(m_maxX, m_maxY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_minX, m_maxY, m_minZ),
		glm::vec3(m_minX, m_minY, m_minZ),
		glm::vec3(m_maxX, m_minY, m_minZ)));

	ret.push_back(glm::mat3(glm::vec3(m_minX, m_maxY, m_minZ),
		glm::vec3(m_maxX, m_minY, m_minZ),
		glm::vec3(m_maxX, m_maxY, m_minZ)));

	ret.push_back(glm::mat3(glm::vec3(m_maxX, m_maxY, m_minZ),
		glm::vec3(m_maxX, m_minY, m_minZ),
		glm::vec3(m_maxX, m_minY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_maxX, m_maxY, m_minZ),
		glm::vec3(m_maxX, m_minY, m_maxZ),
		glm::vec3(m_maxX, m_maxY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_minX, m_maxY, m_minZ),
		glm::vec3(m_minX, m_minY, m_minZ),
		glm::vec3(m_minX, m_minY, m_maxZ)));

	ret.push_back(glm::mat3(glm::vec3(m_minX, m_maxY, m_minZ),
		glm::vec3(m_minX, m_minY, m_maxZ),
		glm::vec3(m_minX, m_maxY, m_maxZ)));
	return ret;
}

//-----------------------------------------------------------------------------------------------
std::vector<glm::vec3> TerrainMesh::GetExtrems() const
{
	std::vector<glm::vec3> ret;
	ret.push_back(glm::vec3(m_maxX, m_maxY, m_maxZ));
	ret.push_back(glm::vec3(m_maxX, m_maxY, m_minZ));
	ret.push_back(glm::vec3(m_minX, m_maxY, m_minZ));
	ret.push_back(glm::vec3(m_minX, m_maxY, m_maxZ));
	ret.push_back(glm::vec3(m_maxX, m_minY, m_maxZ));
	ret.push_back(glm::vec3(m_maxX, m_minY, m_minZ));
	ret.push_back(glm::vec3(m_minX, m_minY, m_minZ));
	ret.push_back(glm::vec3(m_minX, m_minY, m_maxZ));
	return ret;
}

//-----------------------------------------------------------------------------------------------
glm::vec3 TerrainMesh::GetCenter() const
{
	return glm::vec3(m_maxX - glm::length(m_maxX - m_minX) / 2,
									 m_maxY - glm::length(m_maxY - m_minY) / 2,
									 m_maxZ - glm::length(m_maxZ - m_minZ) / 2);
}

//-----------------------------------------------------------------------------------------------
void TerrainMesh::AssignHeights(const Texture& _heightMap, float _height_scale, float _max_height, float _min_height, int32_t _normal_sharpness, bool _apply_normal_blur)
{
	glBindTexture(GL_TEXTURE_2D, _heightMap.id);
	if (_heightMap.mChannels == 4)
	{
		GLfloat* buffer = new GLfloat[_heightMap.mTextureHeight * _heightMap.mTextureWidth * 4];
		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer);
		m_heightMap.TextureFromBuffer((GLfloat*)buffer, _heightMap.mTextureWidth, _heightMap.mTextureHeight, GL_RGBA, GL_REPEAT, GL_LINEAR);
		if (_heightMap.mTextureHeight == m_rows && _heightMap.mTextureWidth == m_columns)
		{
			for (int i = 0; i < _heightMap.mTextureHeight * _heightMap.mTextureWidth * 4; i += 4)
			{
				float height = (float)(buffer[i] * _height_scale);
				this->vertices[i / 4].Position.y = height <= _max_height ? height : _max_height;
				if (this->vertices[i / 4].Position.y < m_minY)
					m_minY = this->vertices[i / 4].Position.y;
				if (this->vertices[i / 4].Position.y > m_maxY)
					m_maxY = this->vertices[i / 4].Position.y;
			}
		}
		else
		{

		}
		delete[] buffer;
	}
	else if (_heightMap.mChannels == 1)
	{
			GLfloat* buffer = new GLfloat[_heightMap.mTextureHeight * _heightMap.mTextureWidth];
			glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, buffer);
			m_heightMap.TextureFromBuffer((GLfloat*)buffer, _heightMap.mTextureWidth, _heightMap.mTextureHeight, GL_RED, GL_REPEAT, GL_LINEAR);
			if (_heightMap.mTextureHeight == m_rows && _heightMap.mTextureWidth == m_columns)
			{
				for (int i = 0; i < _heightMap.mTextureHeight * _heightMap.mTextureWidth; ++i)
				{
					float height = (float)(buffer[i] * _height_scale);
					this->vertices[i].Position.y = height <= _max_height ? height : _max_height;

					if (this->vertices[i].Position.y < m_minY)
						m_minY = this->vertices[i].Position.y;
					if (this->vertices[i].Position.y > m_maxY)
						m_maxY = this->vertices[i].Position.y;
				}
			}
			else
			{
				unsigned int height_map_res_ratio = _heightMap.mTextureHeight / m_rows;
				for (int col = 0; col < m_columns; ++col)
				{
					for (int row = 0; row < m_rows; ++row)
					{
						unsigned int index = col * _heightMap.mTextureWidth * height_map_res_ratio + row * height_map_res_ratio; // or mTextureHeight ??

						float height = buffer[index] * _height_scale;
						this->vertices[col* m_columns + row].Position.y = (height <= _max_height ? height : _max_height);
						this->vertices[col * m_columns + row].Position.y = (this->vertices[col * m_columns + row].Position.y > _min_height
																																? this->vertices[col * m_columns + row].Position.y : _min_height);

						if (this->vertices[col * m_columns + row].Position.y < m_minY)
							m_minY = this->vertices[col * m_columns + row].Position.y;
						if (this->vertices[col * m_columns + row].Position.y > m_maxY)
							m_maxY = this->vertices[col * m_columns + row].Position.y;
					}
				}
			}
			_GenerateNormalMap(buffer, _heightMap.mTextureWidth, _heightMap.mTextureHeight, _normal_sharpness);
			delete[] buffer;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

//--------------------------------------------------------------------------------------------
void TerrainMesh::MakePlaneIndices(unsigned int _rows, unsigned int _columns, unsigned int _lod)
{
	if (this->indicesLods.size() < _lod)
		this->indicesLods.push_back({});

	this->indicesLods[_lod - 1].resize((_rows) * (_columns) * 2 * 3);// 2 triangles per square, 3 indices per triangle dim-1???
	int runner = 0;
	float MinX = 0, MinZ = 0, MaxX = 0, MaxZ = 0;
	unsigned int lod_scale = pow(2, (_lod - 1));
	for (int col = 0; col < _columns - lod_scale; col += lod_scale)
	{
		for (int row = 0; row < _rows - lod_scale; row += lod_scale)
		{
			this->indicesLods[_lod - 1][runner++] = col * _rows + row;
			this->indicesLods[_lod - 1][runner++] = col * _rows + row + _rows * lod_scale;
			this->indicesLods[_lod - 1][runner++] = col * _rows + row + _rows * lod_scale + lod_scale;

			this->indicesLods[_lod - 1][runner++] = col * _rows + row;
			this->indicesLods[_lod - 1][runner++] = col * _rows + row + _rows * lod_scale + lod_scale;
			this->indicesLods[_lod - 1][runner++] = col * _rows + row + lod_scale;

			for (int i = 0; i < 6 && runner < this->indicesLods[_lod - 1].size(); ++i)
			{
				if (this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.x < MinX)
					MinX = this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.x;
				if (this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.x > MaxX)
					MaxX = this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.x;
				if (this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.z < MinZ)
					MinZ = this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.z;
				if (this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.z > MaxZ)
					MaxZ = this->vertices[this->indicesLods[_lod - 1][runner - i]].Position.z;
			}
		}
	}
	this->indicesLods[_lod - 1].resize(runner);
}

//---------------------------------------------------------------------------
Texture* TerrainMesh::GenerateNormals(GLuint _size)
{
	for (unsigned int i = 0; i < this->indicesLods[0].size(); i += 3)
	{
		glm::vec3& pos1 = this->vertices[this->indicesLods[0][i]].Position;
		glm::vec3& pos2 = this->vertices[this->indicesLods[0][i + 1]].Position;
		glm::vec3& pos3 = this->vertices[this->indicesLods[0][i + 2]].Position;
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(pos3 - pos1), glm::vec3(pos2 - pos1)));  //side ?
		this->vertices[this->indicesLods[0][i]].Normal += normal;
		this->vertices[this->indicesLods[0][i + 1]].Normal += normal;
		this->vertices[this->indicesLods[0][i + 2]].Normal += normal;
	}
	for (unsigned int i = 0; i < this->vertices.size(); ++i)
	{
		this->vertices[i].Normal = glm::normalize(this->vertices[i].Normal);
	}

	// Load to normal map
	if (false)
	{
		GLfloat* buffer = new GLfloat[this->vertices.size() * 4];
		for (int i = 0; i < this->vertices.size(); ++i)
		{
			buffer[i * 4]			= this->vertices[i].Normal.x;//(mesh->vertices[i].Normal.x + 1.0f) / 2.0f;  // ( - 0.5) *2;//(GLubyte)128.f;
			buffer[i * 4 + 1] = this->vertices[i].Normal.z; //(mesh->vertices[i].Normal.y+1.0f)/2.0f;128
			buffer[i * 4 + 2] = this->vertices[i].Normal.y;// (mesh->vertices[i].Normal.z + 1.0f) / 2.0f; //(mesh->vertices[i].Normal.z+1.0f)/2.0f;255
			buffer[i * 4 + 3] = 1.0f;
		}

		m_normalMap = Texture(m_size, m_size, 4); // free texture !!!
		m_normalMap.TextureFromBuffer(buffer, _size, _size);
		delete[] buffer;
	}
	return &m_normalMap;
}

//---------------------------------------------------------------------------
Texture* TerrainMesh::GenerateNormals(GLuint rows, GLuint columns)
{
	for (unsigned int i = 0; i < this->indicesLods[0].size(); i += 3)
	{
		glm::vec3& pos1 = this->vertices[this->indicesLods[0][i]].Position;
		glm::vec3& pos2 = this->vertices[this->indicesLods[0][i + 1]].Position;
		glm::vec3& pos3 = this->vertices[this->indicesLods[0][i + 2]].Position;
		glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(pos3 - pos1), glm::vec3(pos2 - pos1)));  //side ?

		if (normal.y < 0)
			normal = -normal;
		this->vertices[this->indicesLods[0][i]].Normal		 += normal;
		this->vertices[this->indicesLods[0][i + 1]].Normal += normal;
		this->vertices[this->indicesLods[0][i + 2]].Normal += normal;
	}
	for (unsigned int i = 0; i < this->vertices.size(); ++i)
	{
		this->vertices[i].Normal = glm::normalize(this->vertices[i].Normal);
	}

	// Load to normal map
	if (false) 		// Load to normal map
	{
		GLfloat* buffer = new GLfloat[this->vertices.size() * 4];
		for (int i = 0; i < this->vertices.size(); ++i)
		{
			buffer[i * 4]			= (this->vertices[i].Normal.x + 1.0f) / 2.0f;
			buffer[i * 4 + 1] = (this->vertices[i].Normal.z + 1.0f) / 2.0f;
			buffer[i * 4 + 2] = (this->vertices[i].Normal.y + 1.0f) / 2.0f;
			buffer[i * 4 + 3] = 1.0f;
		}
		m_normalMap = Texture(m_rows, m_columns, 4); // free texture !!!
		m_normalMap.TextureFromBuffer(buffer, rows, columns);
		delete[] buffer;
	}
	return &m_normalMap;
}

//---------------------------------------------------------------------------
void TerrainMesh::GenerateTessellationData()
{
	m_tessellation_data.Clear();
	if (this->indicesLods.empty())
		return;

	m_tessellation_data.m_vertices.reserve(indicesLods.back().size() / 6 * 20);
	for (unsigned int i = 0; i < indicesLods.back().size(); i += 6) // take the lowest detailed mesh
	{
		// indices in patch quad -> 0, 1, 2, 5
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i]].Position.x); // v.x
		m_tessellation_data.m_vertices.push_back(0.0f); // v.y
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i]].Position.z); // v.z
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i]].TexCoords.x); // u
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i]].TexCoords.y); // v

		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 5]].Position.x); // v.x
		m_tessellation_data.m_vertices.push_back(0.0f); // v.y
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 5]].Position.z); // v.z
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 5]].TexCoords.x); // u
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 5]].TexCoords.y); // v

		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 1]].Position.x); // v.x
		m_tessellation_data.m_vertices.push_back(0.0f); // v.y
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 1]].Position.z); // v.z
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 1]].TexCoords.x); // u
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 1]].TexCoords.y); // v

		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 2]].Position.x); // v.x
		m_tessellation_data.m_vertices.push_back(0.0f); // v.y
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 2]].Position.z); // v.z
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 2]].TexCoords.x); // u
		m_tessellation_data.m_vertices.push_back(this->vertices[this->indicesLods.back()[i + 2]].TexCoords.y); // v
	}

	glGenVertexArrays(1, &m_tessellation_data.m_terrainVAO);
	glBindVertexArray(m_tessellation_data.m_terrainVAO);

	glGenBuffers(1, &m_tessellation_data.m_terrainVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_tessellation_data.m_terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_tessellation_data.m_vertices.size(), &m_tessellation_data.m_vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texCoord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
	glEnableVertexAttribArray(1);
}

//---------------------------------------------------------------------------
void TerrainMesh::DrawTessellated(std::function<void(const TessellationRenderingInfo&)> _info_updater)
{
	if(!m_tessellation_info.base_start_heights.empty() && !m_tessellation_info.texture_scale.empty())
		_info_updater(m_tessellation_info);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_heightMap.id);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_normalMap.id);
	glPatchParameteri(GL_PATCH_VERTICES, 4/* num pointsin patch*/);
	glBindVertexArray(m_tessellation_data.m_terrainVAO);
	eGlDrawContext::GetInstance().DrawArrays(GL_PATCHES, 0, 4/* num pointsin patch*/ * (m_tessellation_data.m_vertices.size() / 5), "TerrainMesh");
}

//---------------------------------------------------------------------------
std::optional<Vertex> TerrainMesh::FindVertex(float _x, float _z)
{
	float newX = glm::floor(_x);
	newX = newX + (glm::round((_x - newX) * m_devisor)) / m_devisor;
	float newZ = glm::floor(_z);
	newZ = newZ + (glm::round((_z - newZ) * m_devisor)) / m_devisor;

	auto vert = std::find_if(this->vertices.begin(), this->vertices.end(), [newX, newZ](const Vertex& v)
		{ return v.Position.x == newX && v.Position.z == newZ; });
	if (vert != this->vertices.end())
		return *vert;
	else
		return std::nullopt;
}

//---------------------------------------------------------------------------
void TerrainMesh::_GenerateNormalMap(const GLfloat* _heightmap, unsigned int _width, unsigned int _height, int32_t _normal_sharpness, bool _apply_normal_blur)
{
	std::vector<GLfloat> normalMapBuffer(_width * _height * 3);
	for (int y = 0; y < _height; ++y)
	{
		for (int x = 0; x < _width; ++x)
		{
			float hL = _heightmap[y * _width + std::max(x - 1, 0)];
			float hR = _heightmap[y * _width + std::min((int)x + 1, (int)_width - 1)];
			float hU = _heightmap[std::max(y - 1, 0) * _width + x];
			float hD = _heightmap[std::min((int)y + 1, (int)_height - 1) * _width + x];

			glm::vec3 normal;
			normal.x = hL - hR;
			normal.z = hU - hD;
			normal.y = _normal_sharpness * 0.001f; // You can adjust this value as needed for the vertical scaling
			normal = glm::normalize(normal);
			normal.x = (normal.x + 1.0f) / 2.0f;
			normal.y = (normal.y + 1.0f) / 2.0f;
			normal.z = (normal.z + 1.0f) / 2.0f;
			normalMapBuffer[(y * _width + x) * 3] = normal.x;
			normalMapBuffer[(y * _width + x) * 3 + 1] = normal.y;
			normalMapBuffer[(y * _width + x) * 3 + 2] = normal.z;
		}
	}

	if(_apply_normal_blur)
		_SmoothNormals(normalMapBuffer, _width, _height);

	m_normalMap.TextureFromBuffer(&normalMapBuffer[0], _width, _height, GL_RGB, GL_REPEAT, GL_LINEAR);
}

// Function to apply Gaussian blur to a normal map
//---------------------------------------------------------------------------
void TerrainMesh::_SmoothNormals(std::vector<float>& normalMap, int width, int height)
{
	// Define Gaussian kernel size (e.g., 5x5)

	// Calculate half kernel size
	int halfKernel = m_kernel_size / 2;

	// Generate Gaussian kernel
	std::vector<float> kernel(m_kernel_size * m_kernel_size);
	float sum = 0.0f;
	for (int i = -halfKernel; i <= halfKernel; ++i) {
		for (int j = -halfKernel; j <= halfKernel; ++j) {
			int index = (i + halfKernel) * m_kernel_size + (j + halfKernel);
			float weight = exp(-(i * i + j * j) / (2 * m_normal_sigma * m_normal_sigma));
			kernel[index] = weight;
			sum += weight;
		}
	}

	// Normalize the kernel
	for (int i = 0; i < m_kernel_size * m_kernel_size; ++i) {
		kernel[i] /= sum;
	}

	// Apply the Gaussian blur separately to each component of the normal vectors
	std::vector<float> smoothedNormalMap(normalMap.size());
	for (int c = 0; c < 3; ++c) { // Iterate over x, y, z components
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				float sum = 0.0f;
				for (int ky = -halfKernel; ky <= halfKernel; ++ky) {
					for (int kx = -halfKernel; kx <= halfKernel; ++kx) {
						int offsetX = x + kx;
						int offsetY = y + ky;
						if (offsetX >= 0 && offsetX < width && offsetY >= 0 && offsetY < height) {
							int index = (offsetY * width + offsetX) * 3 + c; // Component index
							int kernelIndex = (ky + halfKernel) * m_kernel_size + (kx + halfKernel);
							sum += normalMap[index] * kernel[kernelIndex];
						}
					}
				}
				int index = (y * width + x) * 3 + c; // Component index
				smoothedNormalMap[index] = sum;
			}
		}
	}

	// Normalize the smoothed normal vectors
	for (size_t i = 0; i < smoothedNormalMap.size(); i += 3) {
		float x = smoothedNormalMap[i];
		float y = smoothedNormalMap[i + 1];
		float z = smoothedNormalMap[i + 2];
		float length = sqrt(x * x + y * y + z * z);
		if (length > 0.0f) {
			smoothedNormalMap[i] /= length;
			smoothedNormalMap[i + 1] /= length;
			smoothedNormalMap[i + 2] /= length;
		}
	}

	// Copy the smoothed normal map back to the original normal map
	normalMap = smoothedNormalMap;
}
