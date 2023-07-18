#pragma once

#include <base/base.h>
#include "opengl_assets.h"
#include "MyModel.h"

struct Texture;

//---------------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS TerrainModel : public ITerrainModel
{
public:
	TerrainModel();
	explicit TerrainModel(Texture* heightMap); //@todo copy generation in private !!!
	TerrainModel(Texture * diffuse, Texture * specular, Texture * m_normal, Texture * heightMap);
	TerrainModel(const TerrainModel& other);

	virtual ~TerrainModel();

	void initialize(const Texture* diffuse, const Texture* specular, bool spreed_texture = true);
	void initialize(const Texture* diffuse, const Texture* specular, const Texture* normal, const Texture* heightMap, bool spreed_texture = true);
	
	virtual void														Draw()					override;
	virtual const std::string&							GetName() const override { return mesh->Name(); }
	virtual const std::string&							GetPath() const override { return m_path; }

  virtual size_t													GetVertexCount() const;
  virtual size_t													GetMeshCount() const { return 1; }
  virtual std::vector<const IMesh*>				GetMeshes() const;
	virtual std::vector<const I3DMesh*>			Get3DMeshes() const;

	virtual bool														HasBones() const { return false; }
	virtual std::vector<const IBone*>				GetBones() const { return {}; }

  virtual size_t													GetAnimationCount() const { return 0; }
  virtual std::vector<const IAnimation*>	GetAnimations() const {
    return std::vector<const IAnimation*>();
  }

	float							GetHeight(float x , float z)	override;
	glm::vec3					GetNormal(float x, float z)		override;

	void							makePlaneIndices(unsigned int rows, unsigned int columns, unsigned int _lod = 1);
	
	std::vector<glm::vec3>	GetPositions()			const;
	std::vector<GLuint>		  GetIndeces()			const;

	std::vector<MyMesh*>	  getMeshes()				const;

	void setDiffuse(uint32_t _id);
	void setSpecular(uint32_t _id);

private:
	MyMesh*					mesh; // generate ourself inside constructor
	Material				m_material;
	std::string			m_path;

	Texture					m_height;
	
	GLuint		m_size;
	GLuint		m_rows;
	GLuint		m_columns;

	unsigned int	devisor = 10;

	void			makePlaneVerts(unsigned int dimensions, bool spreed_texture = true);
	void			makePlaneVerts(unsigned int rows, unsigned int columns, bool spreed_texture = true);
	void			makePlaneIndices(unsigned int dimensions);
	void			assignHeights(const Texture& heightMap);
	void			generateNormals(GLuint size);
	void			generateNormals(GLuint rows, GLuint columns);
	Vertex		findVertex(float x, float z);
};


