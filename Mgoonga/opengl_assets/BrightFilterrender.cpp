#include "stdafx.h"
#include "BrightFilterrender.h"
#include "GlBufferContext.h"

//-------------------------------------------------------
eBrightFilterRender::eBrightFilterRender(GLuint				width, 
										GLuint				height, 
										const std::string&	vS,
										const std::string&	fS)
{
	shader.installShaders(vS.c_str(), fS.c_str());

	GLfloat quadVertices[] = {
		// Positions   // TexCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	// Setup screen VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	glBindVertexArray(0);
}

//-------------------------------------------------------
eBrightFilterRender::~eBrightFilterRender()
{
  glDeleteVertexArrays(1, &quadVAO);
  glDeleteBuffers(1, &quadVBO);
}

//-------------------------------------------------------
void eBrightFilterRender::Render()
{
	glUseProgram(shader.ID());
	glBindVertexArray(quadVAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	//glUniform1f(TexWidthLoc, FBO->Width());
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

}
