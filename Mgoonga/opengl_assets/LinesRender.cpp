#include "stdafx.h"
#include "LinesRender.h"
#include "GlDrawContext.h"

//-----------------------------------------------------------------------------------------------------
eLinesRender::eLinesRender(const std::string & vertexShaderPath, const std::string & fragmentShaderPath)
{
	linesShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

  MVPLoc = glGetUniformLocation(linesShader.ID(), "MVP");

  // Setup VAO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  
  glBindVertexArray(VAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glBindVertexArray(0);
}

//------------------------------------------------------------------------------------------------------
eLinesRender::~eLinesRender()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

//-----------------------------------------------------------------------------------------------------
void eLinesRender::Render(const Camera& _camera, std::vector<const LineMesh*>& _meshes)
{
  glUseProgram(linesShader.ID());
  glm::mat4 mvp = _camera.getProjectionMatrix() * _camera.getWorldToViewMatrix() * UNIT_MATRIX;
  glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &mvp[0][0]);

  if (_meshes.empty())
    return;

  for (auto& mesh : _meshes)
  {
    linesShader.SetUniformData("color", mesh->GetColor());
    //mesh->Draw();

    if (mesh->m_verices.empty())
      continue;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mesh->m_verices.size(), &mesh->m_verices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->m_indices.size() * sizeof(GLuint), &mesh->m_indices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    eGlDrawContext::GetInstance().DrawElements(GL_LINES, (GLsizei)mesh->m_indices.size(), GL_UNSIGNED_INT, 0, "eLinesRender");
    glBindVertexArray(0);
  }
}
