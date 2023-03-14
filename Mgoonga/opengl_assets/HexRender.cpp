#include "stdafx.h"
#include "HexRender.h"

eHexRender::eHexRender(const std::string&	vS,
					   const std::string&	fS,
					   const std::string&	gS,
					   std::vector<glm::vec3> _dots,
					   float _radius)
	: dots(_dots)
	, radius(_radius)
{
	hex_shader.installShaders(vS.c_str(), fS.c_str(), gS.c_str());

	MVPLoc = glGetUniformLocation(hex_shader.ID(), "MVP");
	radiusLoc = glGetUniformLocation(hex_shader.ID(), "radius");
	// Setup hex VAO
	if (!dots.empty())
	{
		glGenVertexArrays(1, &hexVAO);
		glGenBuffers(1, &hexVBO);
		glBindVertexArray(hexVAO);
		glBindBuffer(GL_ARRAY_BUFFER, hexVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)* dots.size(), &dots[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glBindVertexArray(0);
	}
}

eHexRender::~eHexRender()
{
  glDeleteVertexArrays(1, &hexVAO);
  glDeleteBuffers(1, &hexVBO);
}

void eHexRender::Render(glm::mat4 mvp)
{
	glUseProgram(hex_shader.ID());
	glBindVertexArray(hexVAO);
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &mvp[0][0]);
	glUniform1f(radiusLoc, 0.57f * radius);
	glDrawArrays(GL_POINTS, 0, dots.size() ); 
	glBindVertexArray(0);
}
