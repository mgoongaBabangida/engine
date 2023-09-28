#pragma once

#include "Shader.h"
#include "Texture.h"
#include <math/Camera.h>
#include <math/Particle.h>
#include <base/interfaces.h>
#include <math/Timer.h>
#include "MyMesh.h"

#include <chrono>
#include <memory>
#include <vector>

class eObject;
class MyModel;

//------------------------------------------------------------------------------------
class eParticleRender
{
public:
	eParticleRender(std::shared_ptr<MyMesh>, const Texture*, const std::string&, const std::string&);
	~eParticleRender();

	void	Render(const Camera &camera);
	void	AddParticleSystem(std::shared_ptr<IParticleSystem> sys);

	Shader& GetShader() { return m_particleShader; }

protected:
	Shader											        m_particleShader;
	std::unique_ptr<MyModel>						m_model;
	std::unique_ptr<ParticleMesh>				m_mesh;
	std::unique_ptr<eObject>						m_object;
	
	std::vector<std::shared_ptr<IParticleSystem> >	m_systems;
	std::vector<float>								              m_instancedBuffer;

	GLuint															projectionMatrixUniformLocation;
};
