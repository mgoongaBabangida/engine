#include "stdafx.h"
#include "ScreenMesh.h"
#include <iostream>

eScreenMesh::eScreenMesh(Texture _textureOne, Texture _textureTwo): textureOne(_textureOne), textureTwo(_textureTwo)
{
	GLfloat quadVertices[] = {
		// Positions   // TexCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		1.0f, -1.0f,  1.0f, 0.0f,
		1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
}

eScreenMesh::~eScreenMesh()
{
  glGenVertexArrays(1, &quadVAO);
  glGenBuffers(1, &quadVBO);
}

void eScreenMesh::Draw()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureOne.id);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTwo.id);

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

std::vector<const Texture*> eScreenMesh::GetTextures() const
{
  std::vector<const Texture*> ret;
  ret.push_back(&textureOne);
  ret.push_back(&textureTwo);
  return ret;
}

eFrameMesh::eFrameMesh()
{
  frameVertices = {
    // Positions   // TexCoords
    0.0f,  0.0f,  0.0f, 1.0f,
		0.0f,  0.0f,  0.0f, 0.0f,
		0.0f,  0.0f,  1.0f, 0.0f,
		0.0f,  0.0f,  0.0f, 1.0f,
		0.0f,  0.0f,  1.0f, 0.0f,
    };

	glGenVertexArrays(1, &quadVAO_fr);
	glGenBuffers(1, &quadVBO_fr);
	glBindVertexArray(quadVAO_fr);
	
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO_fr);
	glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_DYNAMIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);
}

eFrameMesh::~eFrameMesh()
{
  glGenVertexArrays(1, &quadVAO_fr);
  glGenBuffers(1, &quadVBO_fr);
}

void eFrameMesh::Draw()
{
	glBindVertexArray(quadVAO_fr);
	glDrawArrays(GL_LINE_STRIP, 0, 5);
	glBindVertexArray(0);
}

//-----------------------------------------------------------------------------------
void eFrameMesh::UpdateFrame(float top_x, float top_y, float botom_x, float botom_y, float viewport_width, float viewport_height)
{
	top_y = viewport_height - top_y; // invert y axis
	botom_y = viewport_height - botom_y; // invert y axis
	frameVertices[0] = (top_x / viewport_width) * 2 - 1.0f;
	frameVertices[1] = (top_y / viewport_height) * 2 - 1.0f;
  frameVertices[4] = (botom_x / viewport_width) * 2 - 1.0f;
  frameVertices[5] = (top_y / viewport_height) * 2 - 1.0f;
  frameVertices[8] = (botom_x / viewport_width) * 2 - 1.0f;
  frameVertices[9] = (botom_y / viewport_height) * 2 - 1.0f;
  frameVertices[12] = (top_x / viewport_width) * 2 - 1.0f;
  frameVertices[13] = (botom_y / viewport_height) * 2 - 1.0f;
  frameVertices[16] = (top_x / viewport_width) * 2 - 1.0f;
  frameVertices[17] = (top_y / viewport_height) * 2 - 1.0f;
  glBindBuffer(GL_ARRAY_BUFFER, quadVBO_fr);
  glBufferData(GL_ARRAY_BUFFER, sizeof(frameVertices), &frameVertices, GL_DYNAMIC_DRAW);
}

