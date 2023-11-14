#include "MeshLineRender.h"
#include "GlDrawContext.h"
#include <math/Rigger.h>

//---------------------------------------------------------------
eMeshLineRender::eMeshLineRender(const std::string& _vS, const std::string& _fS)
  : matrices(MAX_BONES)
{
  m_shader.installShaders(_vS.c_str(), _fS.c_str());
  m_shader.GetUniformInfoFromShader();
  m_shader.GetUniformDataFromShader();
}

//---------------------------------------------------------------
eMeshLineRender::~eMeshLineRender()
{
}

//---------------------------------------------------------------
void eMeshLineRender::Render(const Camera& _camera, const Light& _light, const std::vector<shObject>& _objects)
{
  glUseProgram(m_shader.ID());
	m_shader.SetUniformData("outline", true);
	glm::mat4 worldToProjectionMatrix = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix();
	for (auto& object : _objects)
	{
		glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
		m_shader.SetUniformData("modelToProjectionMatrix", modelToProjectionMatrix);
		m_shader.SetUniformData("modelToWorldMatrix", object->GetTransform()->getModelMatrix());

		if (object->GetRigger() != nullptr)
		{
			matrices = object->GetRigger()->GetMatrices();
			Rigger* rigger = dynamic_cast<Rigger*>(object->GetRigger());
			m_shader.SetUniformData("outline_bone", rigger->GetActiveBoneIndex());
		}
		else
		{
			for (auto& m : matrices)
				m = UNIT_MATRIX;
			m_shader.SetUniformData("outline_bone", MAX_BONES);
		}

		glUniformMatrix4fv(glGetUniformLocation(m_shader.ID(), "gBones"), MAX_BONES, GL_FALSE, &matrices[0][0][0]); //@todo set with shader
		object->GetModel()->Draw();
		for (auto& mesh : object->GetModel()->Get3DMeshes())
		{
			mesh->BindVAO();
			eGlDrawContext::GetInstance().DrawElements(GL_LINES, (GLsizei)mesh->GetIndices().size(), GL_UNSIGNED_INT, 0, "eMeshLineRender");
			mesh->UnbindVAO();
		}
	}
	m_shader.SetUniformData("outline", false);
}
