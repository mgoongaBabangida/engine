#include "stdafx.h"

#include "TerrainMesh.h"

//---------------------------------------------------
TerrainMesh::TerrainMesh(const std::string& _name)
  : MyMesh(_name)
{
	m_normalMap = Texture::GetTexture1x1(TColor::BLUE);
}

//----------------------------------------------------------------------------
void TerrainMesh::MakePlaneVerts(unsigned int _dimensions, bool _spreed_texture)
{
	this->vertices.resize(_dimensions * _dimensions);
	int half = _dimensions / 2;
	for (int i = 0; i < _dimensions; i++)
	{
		for (int j = 0; j < _dimensions; j++)
		{
			Vertex& thisVert = this->vertices[i * _dimensions + j];
			thisVert.Position.x = (float)(j - half) / m_devisor;
			thisVert.Position.z = (float)(i - half) / m_devisor;
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
	this->vertices.resize(_rows * _columns);
	int half_r = _rows / 2;
	int half_c = _columns / 2;
	int counter = 0;
	float MinX = 0, MinZ = 0, MaxX = 0, MaxZ = 0; //debug only
	for (int i = 0; i < _columns; i++)
	{
		for (int j = 0; j < _rows; j++)
		{
			Vertex& thisVert = this->vertices[i * _rows + j];
			thisVert.Position.x = static_cast<float>(j - half_r) / static_cast<float>(m_devisor);
			thisVert.Position.z = static_cast<float>(i - half_c) / static_cast<float>(m_devisor);
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
			if (thisVert.Position.x < MinX)  //debug only
				MinX = thisVert.Position.x;
			if (thisVert.Position.x > MaxX)
				MaxX = thisVert.Position.x;
			if (thisVert.Position.z < MinZ)
				MinZ = thisVert.Position.z;
			if (thisVert.Position.z > MaxZ)
				MaxZ = thisVert.Position.z;
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
void TerrainMesh::AssignHeights(const Texture& _heightMap, float _height_scale, float _max_height)
{
	glBindTexture(GL_TEXTURE_2D, _heightMap.id);
	if (_heightMap.mChannels == 4)
	{
		GLfloat* buffer = new GLfloat[_heightMap.mTextureHeight * _heightMap.mTextureWidth * 4];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, buffer);
		int counter = 0;
		for (int i = 0; i < _heightMap.mTextureHeight * _heightMap.mTextureWidth * 4; i += 4)
		{
			float height = (float)(buffer[i] * _height_scale);
			this->vertices[i / 4].Position.y = height <= _max_height ? height : _max_height;
		}
		delete[] buffer;
	}
	else if (_heightMap.mChannels == 1)
	{
		GLfloat* buffer = new GLfloat[_heightMap.mTextureHeight * _heightMap.mTextureWidth];

		glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, buffer);
		int counter = 0;
		for (int i = 0; i < _heightMap.mTextureHeight * _heightMap.mTextureWidth; ++i)
		{
			float height = (float)(buffer[i] * _height_scale);
			this->vertices[i].Position.y = height <= _max_height ? height : _max_height;
		}
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
