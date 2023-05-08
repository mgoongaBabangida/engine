#pragma once

#include <base/interfaces.h>

#include "Texture.h"
#include "ShapeData.h"

#include <math/Bezier.h>

//----------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS MyMesh: public I3DMesh
{
public:
	explicit MyMesh(const std::string& _name);
	MyMesh(const MyMesh&) = default;
	virtual ~MyMesh();

	MyMesh(const std::string& _name, std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures);
	MyMesh(const std::string& _name, const ShapeData& data);
	
	virtual void								Draw()			override;
	virtual const std::string&	Name() const { return name; }

  virtual size_t														GetVertexCount() const override { return vertices.size(); }
	virtual const std::vector<Vertex>&				GetVertexs() const override { return vertices; }
	virtual const std::vector<unsigned int>&	GetIndices() const override { return indices; }
	virtual void                              BindVAO() const override { glBindVertexArray(this->VAO); }
	virtual void                              UnbindVAO() const override { glBindVertexArray(0); }

  virtual std::vector<const Texture*> GetTextures() const;
  virtual void												setTextures(std::vector<Texture*>);
	virtual void												setupMesh();
	virtual void												calculatedTangent();

public:
	/*  Mesh Data  */
	std::vector<Vertex>	vertices;
	std::vector<GLuint>		indices;
	std::vector<Texture*>	textures;

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	std::string name;
};

//----------------------------------------------------------------------------------------------
class ParticleMesh : public I3DMesh
{
public:
	static const int		MAXPARTICLES	= 1000;
	static const GLsizei	SIZEOF = sizeof(glm::mat4) + sizeof(glm::vec2) * 3;
	
	ParticleMesh(std::vector< Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	ParticleMesh(const ShapeData & data);
	ParticleMesh(const ParticleMesh&) = delete;

  virtual ~ParticleMesh();

	virtual void								Draw() override;
	virtual const std::string&	Name() const override { return name; }

  virtual size_t														GetVertexCount() const override { return vertices.size(); }
	virtual const std::vector<Vertex>&				GetVertexs() const override { return vertices; }
	virtual const std::vector<unsigned int>&	GetIndices() const override { return indices; }
	virtual void                              BindVAO() const override { glBindVertexArray(this->VAO); }
	virtual void                              UnbindVAO() const override { glBindVertexArray(0); }

	virtual std::vector<const Texture*> GetTextures() const;

  void				SetUpInstances(GLuint _instances) { instances = _instances; }
	void				updateInstancedData(std::vector<float>& buffer);

public:
  /*  Mesh Data  */
  std::vector<Vertex>		vertices;
  std::vector<GLuint>		indices;
  std::vector<Texture>	textures;
  GLuint								instances;

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	GLuint VBOinstanced;
	std::string name= "ParticleMesh";
	/*  Functions    */
	virtual void		setupMesh();
};

//------------------------------------------------------
class DLL_OPENGL_ASSETS SimpleGeometryMesh : public IMesh
{
public:
	SimpleGeometryMesh(const std::vector<glm::vec3>&, float);

	~SimpleGeometryMesh();

	float GetRadius() const { return m_radius; }

	virtual void								Draw();
	virtual const std::string&	Name() const { return m_name; }
	virtual size_t							GetVertexCount() const { return m_dots.size(); }

	virtual std::vector<const Texture*>				GetTextures() const { return {}; }
	
	virtual bool HasMaterial() const { return false; }
	virtual void SetMaterial(const Material&) {}
	virtual std::optional<Material> GetMaterial() const { return std::nullopt; }

protected:
	std::string m_name = "SimpleGeometryMesh";
	std::vector<glm::vec3> m_dots;
	float									 m_radius; //@todo?

	GLuint hexVAO;
	GLuint hexVBO;
};

//------------------------------------------------------
class DLL_OPENGL_ASSETS BezierCurveMesh : public IMesh
{
public:
	explicit BezierCurveMesh(const dbb::Bezier& _bezier);

	virtual ~BezierCurveMesh();
	dbb::Bezier& GetBezier() { return m_bezier; };
	void Update();

	virtual void								Draw();
	virtual const std::string&	Name() const { return m_name; }
	virtual size_t							GetVertexCount() const { return 4; }

	virtual std::vector<const Texture*> GetTextures() const { return {}; }
	
	virtual bool HasMaterial() const { return false; }
	virtual void SetMaterial(const Material&) {}
	virtual std::optional<Material> GetMaterial() const { return std::nullopt; }

protected:
	std::string m_name = "BezierCurveMesh";
	dbb::Bezier m_bezier;
	GLuint VAO;
	GLuint VBO;
};
