#include "stdafx.h"
#include "ParticleRender.h"
#include "Object.h"
#include "MyModel.h"
#include "MyMesh.h"
#include "ParticleSystem.h"
#include "Transform.h"

eParticleRender::eParticleRender(MyModel *			_model, 
								ParticleMesh*		_mesh, 
								IParticleSystem*	_sys, 
								ParticleSystemInfo _info)
: instancedBuffer(ParticleMesh::MAXPARTICLES * _mesh->getStep() / sizeof(float), 0.0f)
{
	particleShader.installShaders("ParticleVertexShader.glsl", "ParticleFragmentShader.glsl");

	modelToWorldMatrixUniformLocation		= glGetUniformLocation(particleShader.ID, "modelToWorldMatrix");
	fullTransformationUniformLocation		= glGetUniformLocation(particleShader.ID, "modelToProjectionMatrix");
	worldToProjectionMatrixUniformLocation	= glGetUniformLocation(particleShader.ID, "worldToProjectionMatrix");
	ProjectionMatrixUniformLocation			= glGetUniformLocation(particleShader.ID, "ProjectionMatrix");
	modelViewMatrixLocation					= glGetUniformLocation(particleShader.ID, "ModelViewMatrix");
	texOffset1Location						= glGetUniformLocation(particleShader.ID, "texOffset1");
	texOffset2Location						= glGetUniformLocation(particleShader.ID, "texOffset2");
	texCoordInfoLocation					= glGetUniformLocation(particleShader.ID, "texCoordInfo");

	systems.push_back(std::shared_ptr<IParticleSystem>(_sys));
	infos.push_back(_info);
	//get particle texture
	_model->SetTexture(_info.texture);
	object = new eObject(_model);
	mesh = _mesh;
}

void eParticleRender::Render(const glm::mat4 & _projectionMatrix, const Camera & _camera)
{
	glUseProgram(particleShader.ID);

	mat4 worldToProjectionMatrix = _projectionMatrix * _camera.getWorldToViewMatrix();
	glUniformMatrix4fv(ProjectionMatrixUniformLocation, 1, GL_FALSE, &_projectionMatrix[0][0]);  //Projection

	int info = 0;
	for (auto& system : systems)
	{
		object->getTransform()->billboard(_camera.getDirection());
		object->getTransform()->setScale(glm::vec3(infos[info].scale, infos[info].scale, infos[info].scale));

		system->generateParticles(infos[info].systemCenter, infos[info].texture);  

		std::vector<Particle>::iterator n_end = system->prepareParticles(_camera.getPosition());

		int counter = 0;
		//std::cout << "cycle--------------" << std::endl;
		int debug = 0;
		std::vector<Particle>::iterator iter = system->getParticles().begin();
		if (iter != system->getParticles().end() && n_end != system->getParticles().begin() ) 
		{
			while (iter != n_end) 
			{
				debug++;
				/*std::cout << "here alive" << std::endl;*/
				object->getTransform()->setTranslation(iter->getPosition());
				mat4 modelViewMatrix = _camera.getWorldToViewMatrix() * object->getTransform()->getModelMatrix();
				glUniformMatrix4fv(modelViewMatrixLocation, 1, GL_FALSE, &modelViewMatrix[0][0]);  //modelView
				
				LoadOffsetsInfo(iter->gettexOffset1(), iter->gettexOffset2(), iter->getNumRows(), iter->getBlend());

				for (int i = 0; i < 4; ++i) {
					for (int j = 0; j < 4; ++j) {
						instancedBuffer[counter++] = modelViewMatrix[i][j]; // [i][j] ? [j][i]
					}
				}
				instancedBuffer[counter++] = iter->gettexOffset1().x;
				instancedBuffer[counter++] = iter->gettexOffset1().y;
				instancedBuffer[counter++] = iter->gettexOffset2().x;
				instancedBuffer[counter++] = iter->gettexOffset2().y;
				instancedBuffer[counter++] = iter->getNumRows();
				instancedBuffer[counter++] = iter->getBlend();

				++iter;
			}
		}
		//std::cout <<"debug count "<< debug << std::endl;
		
		mesh->updateInstancedData(instancedBuffer);
		mesh->SetUpInstances(debug);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, this->infos[info].texture->id);
		glUniform1i(glGetUniformLocation(particleShader.ID, "texture_diffuse1"), 2);
		mesh->Draw();

		++info;
	}
}
   
void eParticleRender::AddParticleSystem(IParticleSystem* sys, ParticleSystemInfo info)
{
	systems.push_back(std::shared_ptr<IParticleSystem>(sys));
	infos.push_back(info);
}

void eParticleRender::LoadOffsetsInfo(glm::vec2 offset1, glm::vec2 offset2, float numRows, float blend)
{
	glUniform2f(texOffset1Location, offset1[0], offset1[1]);
	glUniform2f(texOffset2Location, offset2[0], offset2[1]);
	glUniform2f(texCoordInfoLocation, numRows, blend);
}
