#pragma once

#include <base/interfaces.h>

#include "Texture.h"
#include "ShapeData.h"

#include <math/Bezier.h>

//----------------------------------------------------------------------------------------------
class DLL_OPENGL_ASSETS MyMesh: public I3DMesh
{
public:
	enum class RenderMode { DEFAULT, WIREFRAME };

	explicit MyMesh(const std::string& _name);

	MyMesh(const MyMesh&) = delete;

	virtual ~MyMesh();

	MyMesh(const std::string& _name, std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture*> textures);
	MyMesh(const std::string& _name, const ShapeData& data);
	
	virtual void								Draw()			override;
	virtual const std::string&	Name() const { return name; }

  virtual size_t														GetVertexCount() const override { return vertices.size(); }
	virtual const std::vector<Vertex>&				GetVertexs() const override { return vertices; }
	virtual const std::vector<unsigned int>&	GetIndices() const override { return indicesLods[0]; }

	virtual void                              BindVAO() const override { glBindVertexArray(this->VAO); }
	virtual void                              UnbindVAO() const override { glBindVertexArray(0); }

  virtual std::vector<TextureInfo>		GetTextures() const;
	virtual void												AddTexture(Texture* _t) override { textures.push_back(_t); }
  virtual void												setTextures(std::vector<Texture*>);

	virtual void												setupMesh();
	virtual void												calculatedTangent() override;
	virtual void												ReloadVertexBuffer() override;

	bool SwitchLOD(GLuint _LOD);
	GLuint LODInUse() const;

	void SetRenderMode(RenderMode _mode);
	RenderMode GetRenderMode();

public: //@todo should be protected
	/*  Mesh Data  */
	std::vector<Vertex>								vertices;
	std::vector<std::vector<GLuint>>	indicesLods;
	std::vector<Texture*>							textures;

protected:
	/*  Render data  */
	GLuint VAO;
	GLuint VBO;
	std::vector<GLuint> EBO;

	std::string name;

	GLuint LOD_index__in_use = 0;
	RenderMode m_render_mode = RenderMode::DEFAULT;
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

	virtual std::vector<TextureInfo>	GetTextures() const;
	virtual void											AddTexture(Texture* _t) override { textures.push_back(*_t); }

	virtual void											calculatedTangent()  override {} //@todo ?
	virtual void											ReloadVertexBuffer() override {}//@todo ?

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
	enum class GeometryType
	{
		Hex = 0,
		Circle = 1
	};

	SimpleGeometryMesh(const std::vector<glm::vec3>&, float _radius, GeometryType _type, glm::vec3 _color);
	virtual ~SimpleGeometryMesh();

	float GetRadius() const { return m_radius; }
	glm::vec4 GetColor() const { return m_color; }
	void SetColor(glm::vec4 _c);
	GeometryType GetGeometryType() const { return m_type; }
	void SetDots(const std::vector<glm::vec3>&);

	virtual void								Draw();
	virtual const std::string&	Name() const { return m_name; }
	virtual size_t							GetVertexCount() const { return m_dots.size(); }

	virtual bool HasMaterial() const { return false; }
	virtual void SetMaterial(const Material&) {}
	virtual std::optional<Material> GetMaterial() const { return std::nullopt; }

protected:
	std::string m_name = "SimpleGeometryMesh";
	std::vector<glm::vec3> m_dots;
	float									 m_radius;
	glm::vec4							 m_color;
	GeometryType					 m_type;

	GLuint hexVAO;
	GLuint hexVBO;
};

//------------------------------------------------------
class DLL_OPENGL_ASSETS BezierCurveMesh : public IMesh
{
public:
	explicit BezierCurveMesh(const dbb::Bezier& _bezier, bool _is2D = false);
	virtual ~BezierCurveMesh();

	dbb::Bezier& GetBezier() { return m_bezier; };
	bool Is2D() { return m_2d; }
	void Update();

	virtual void								Draw();
	virtual const std::string&	Name() const { return m_name; }
	virtual size_t							GetVertexCount() const { return 4; }
	
	virtual bool HasMaterial() const { return false; }
	virtual void SetMaterial(const Material&) {}
	virtual std::optional<Material> GetMaterial() const { return std::nullopt; }

protected:
	std::string m_name = "BezierCurveMesh";
	dbb::Bezier m_bezier;
	GLuint VAO;
	GLuint VBO;
	bool m_2d = false;
};

//------------------------------------------------------
class DLL_OPENGL_ASSETS LineMesh : public IMesh
{
public:
	LineMesh(const std::vector<glm::vec3>&, const std::vector<GLuint>&, glm::vec4);
	glm::vec4 GetColor() const { return m_color; }
	~LineMesh();

	void UpdateData(const std::vector<glm::vec3>&, const std::vector<GLuint>&, glm::vec4 _color);

	virtual void								Draw();
	virtual const std::string&	Name() const { return m_name; }
	virtual size_t							GetVertexCount() const { return m_verices.size(); }

	virtual bool HasMaterial() const { return false; }
	virtual void SetMaterial(const Material&) {}
	virtual std::optional<Material> GetMaterial() const { return std::nullopt; }

	GLuint VAO, VBO, EBO;
	std::vector<glm::vec3>	m_verices;
	std::vector<GLuint>			m_indices;
protected:
	std::string m_name = "LineMesh";
	glm::vec4								m_color;
};
