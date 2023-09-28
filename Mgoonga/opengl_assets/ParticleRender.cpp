#include "stdafx.h"
#include <math/ParticleSystem.h>
#include "ParticleRender.h"
#include <base/Object.h>
#include "MyModel.h"
#include <math/Transform.h>
#include "ShapeGenerator.h"
#include "ShapeData.h"

#include <algorithm>

//------------------------------------------------------------------------------------------
eParticleRender::eParticleRender(std::shared_ptr<MyMesh> _mesh,
								 const Texture*				 _texture,
								 const std::string&		 _vertexShaderPath,
								 const std::string&		 _fragmentShaderPath)
: m_instancedBuffer(ParticleMesh::MAXPARTICLES  * ParticleMesh::SIZEOF / sizeof(float), 0.0f)
{
	m_particleShader.installShaders(_vertexShaderPath.c_str(), _fragmentShaderPath.c_str());

	projectionMatrixUniformLocation = glGetUniformLocation(m_particleShader.ID(), "ProjectionMatrix");

	m_model.reset(new MyModel(_mesh, "Particle model",_texture));
	m_object.reset(new eObject); // @todo prob transform will do
	m_object->SetModel(m_model.get());
	m_object->SetTransform(new Transform);
	
	ShapeData quad	= ShapeGenerator::makeQuad();
	m_mesh.reset(new ParticleMesh(quad));
	quad.cleanup();
}

//--------------------------------------------------------------------------------------
eParticleRender::~eParticleRender()
{
	m_model.release();
}

//-------------------------------------------------------------------------------------------------------------
void eParticleRender::Render(const Camera& _camera)
{
	glUseProgram(m_particleShader.ID());
	glUniformMatrix4fv(projectionMatrixUniformLocation, 1, GL_FALSE, &_camera.getProjectionMatrix()[0][0]);

	int info = 0;
	for(auto& system : m_systems)
	{
		if (!system->IsStarted())
			system->Start();

		if (system->IsFinished())
			continue;

		m_object->GetTransform()->billboard(_camera.getDirection());

		int counter		= 0;
		int instances	= 0;
		std::vector<Particle>::iterator n_end = system->PrepareParticles(_camera.getPosition());
		std::vector<Particle>::iterator iter = system->GetParticles().begin();
		if(iter != system->GetParticles().end() && n_end != system->GetParticles().begin())
		{
			while(iter != n_end)
			{
				++instances;
				m_object->GetTransform()->setTranslation(iter->getPosition());
				m_object->GetTransform()->setScale({ iter->getScale(),iter->getScale(),iter->getScale() });

				glm::mat4 modelViewMatrix = _camera.getWorldToViewMatrix() * m_object->GetTransform()->getModelMatrix();
				
				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						m_instancedBuffer[counter++] = modelViewMatrix[i][j];
					}
				}
				m_instancedBuffer[counter++] = iter->gettexOffset1().x;
				m_instancedBuffer[counter++] = iter->gettexOffset1().y;
				m_instancedBuffer[counter++] = iter->gettexOffset2().x;
				m_instancedBuffer[counter++] = iter->gettexOffset2().y;
				m_instancedBuffer[counter++] = iter->getNumRows();
				m_instancedBuffer[counter++] = iter->getBlend();

				++iter;
			}
		}	
		m_mesh->updateInstancedData(m_instancedBuffer);
		m_mesh->SetUpInstances(instances);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, system->GetTexture()->id);
		glUniform1i(glGetUniformLocation(m_particleShader.ID(), "texture_diffuse1"), 2);
		m_mesh->Draw();

		++info;
	}

	auto toRemove = std::remove_if(m_systems.begin(), m_systems.end(),[](std::shared_ptr<IParticleSystem> sys) { return sys->IsFinished();});
	if(toRemove != m_systems.end())
	{
		m_systems.erase(toRemove);
	}
}

void eParticleRender::AddParticleSystem(std::shared_ptr<IParticleSystem> _sys)
{
	if (std::find(m_systems.begin(), m_systems.end(), _sys) == m_systems.end())
		m_systems.push_back(_sys);
}
