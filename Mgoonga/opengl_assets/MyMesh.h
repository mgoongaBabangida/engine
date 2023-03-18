#pragma once

#include <base/interfaces.h>

#include "Texture.h"
#include "ShapeData.h"

//----------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS MyMesh: public IMesh
{
public:
	MyMesh();
	MyMesh(const MyMesh&) = default;
	virtual ~MyMesh();

	MyMesh(std::vector<MyVertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures);
	explicit MyMesh(const ShapeData& data);
	
	virtual void Draw()			override;
  virtual size_t GetVertexCount() const override { return vertices.size(); }
  virtual std::vector<const Texture*> GetTextures() const;

  virtual void setTextures(std::vector<Texture*>);
	virtual void setupMesh();
	virtual void calculatedTangent();

	virtual const std::string& Name() const { return name; }
public:
	/*  Mesh Data  */
	std::vector<MyVertex>	vertices;
	std::vector<GLuint>		indices;
	std::vector<Texture*>	textures;

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	std::string name;
};

//----------------------------------------------------------------------------------------------
class ParticleMesh : public IMesh
{
public:
	static const int		MAXPARTICLES	= 1000;
	static const GLsizei	SIZEOF = sizeof(glm::mat4) + sizeof(glm::vec2) * 3;
	
	ParticleMesh(std::vector< MyVertex> vertices, std::vector<GLuint> indices, std::vector<Texture> textures);
	ParticleMesh(const ShapeData & data);
	ParticleMesh(const ParticleMesh&) = delete;

  virtual ~ParticleMesh();

	virtual void		Draw() override;
  virtual size_t GetVertexCount() const override { return vertices.size(); }
	virtual const std::string& Name() const override { return ""; }
  virtual std::vector<const Texture*> GetTextures() const;

  void				SetUpInstances(GLuint _instances) { instances = _instances; }
	void				updateInstancedData(std::vector<float>& buffer);

public:
  /*  Mesh Data  */
  std::vector<MyVertex>	vertices;
  std::vector<GLuint>		indices;
  std::vector<Texture>	textures;
  GLuint					instances;

protected:
	/*  Render data  */
	GLuint VAO, VBO, EBO;
	GLuint VBOinstanced;
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
	virtual void Draw();
	virtual size_t GetVertexCount() const { return m_dots.size(); }
	virtual std::vector<const Texture*> GetTextures() const { return {}; }
	virtual bool HasMaterial() const { return false; }
	virtual void SetMaterial(const Material&) {}
	virtual std::optional<Material> GetMaterial() const { return std::nullopt; }
	virtual const std::string& Name() const { return m_name; }
protected:
	std::string m_name = "SimpleGeometryMesh";
	std::vector<glm::vec3> m_dots;
	float m_radius; //@todo?

	GLuint hexVAO;
	GLuint hexVBO;
};
