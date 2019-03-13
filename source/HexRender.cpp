#include "stdafx.h"
#include "HexRender.h"

const float dist = 1.75f;
const float Height = 2.0f;

eHexRender::eHexRender() :dots{ glm::vec3(glm::sin(glm::radians(30.0f)) * dist,Height, glm::cos(glm::radians(30.0f)) * dist),
						glm::vec3(glm::sin(glm::radians(90.0f)) * dist, Height,glm::cos(glm::radians(90.0f)) * dist),
						glm::vec3(glm::sin(glm::radians(150.0f)) * dist, Height, glm::cos(glm::radians(150.0f)) * dist),
						glm::vec3(glm::sin(glm::radians(210.0f)) * dist, Height, glm::cos(glm::radians(210.0f)) * dist),
						glm::vec3(glm::sin(glm::radians(270.0f)) * dist, Height,glm::cos(glm::radians(270.0f)) * dist),
						glm::vec3(glm::sin(glm::radians(330.0f)) * dist,Height, glm::cos(glm::radians(330.0f)) * dist),
						glm::vec3(glm::sin(glm::radians(30.0f)) * dist*2,Height, glm::cos(glm::radians(30.0f)) * dist*2),
						glm::vec3(0, Height, 0) }
{
	hex_shader.installShaders("VertexShades.glsl", "StencilFragmentShader.glsl", "HexGeometry.glsl");

	MVPLoc = glGetUniformLocation(hex_shader.ID, "MVP");

	// Setup hex VAO
	glGenVertexArrays(1, &hexVAO);
	glGenBuffers(1, &hexVBO);
	glBindVertexArray(hexVAO);
	glBindBuffer(GL_ARRAY_BUFFER, hexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*dots.size(), &dots[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glBindVertexArray(0);
}

void eHexRender::Render(glm::mat4 mvp)
{
	glUseProgram(hex_shader.ID);
	glBindVertexArray(hexVAO);
	glUniformMatrix4fv(MVPLoc, 1, GL_FALSE, &mvp[0][0]);
	glDrawArrays(GL_POINTS, 0, dots.size() ); 
	glBindVertexArray(0);
}
