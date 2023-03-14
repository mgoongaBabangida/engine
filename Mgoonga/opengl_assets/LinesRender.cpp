#include "stdafx.h"
#include "LinesRender.h"

//-----------------------------------------------------------------------------------------------------
eLinesRender::eLinesRender(const std::string & vertexShaderPath, const std::string & fragmentShaderPath)
{
	linesShader.installShaders(vertexShaderPath.c_str(), fragmentShaderPath.c_str());

	viewLoc		  = glGetUniformLocation(linesShader.ID(), "view");
	projectionLoc = glGetUniformLocation(linesShader.ID(), "projection");

  // Setup VAO
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
}

//------------------------------------------------------------------------------------------------------
eLinesRender::~eLinesRender()
{
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
}

//-----------------------------------------------------------------------------------------------------
void eLinesRender::Render(const Camera& _camera, const std::vector<glm::vec3>& _lines, std::vector<GLuint> _indices)
{
  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* _lines.size(), &_lines[0], GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLuint), &_indices[0], GL_DYNAMIC_DRAW);

	glUseProgram(linesShader.ID());
	glm::mat4 view = _camera.getWorldToViewMatrix();
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &_camera.getProjectionMatrix()[0][0]);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glDrawElements(GL_LINES, _indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
