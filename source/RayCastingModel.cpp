#include "RayCastingModel.h"
//#include "Structures.h"
#include "MyMesh.h"
#include "Texture.h"
#include <iostream>
#include <Vertex.h>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\transform.hpp>
#include <map>
#define PI 3.14159265359
//#define NUM_ARRAY_ELEMENTS(a) sizeof(a) / sizeof(*a)



RayCastingModel::RayCastingModel(float w, float h,Texture* t)//:MyModel(nullptr,t) //!!!!!!!!!!!!!!!!!!
{
	m_translation = glm::vec3(); 
	m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
	this->width = w;
	this->height = h;
	texture = t;

	float widthP = 2 * tan(glm::radians(60.0f)) * 0.1f;
	float widthF = 2 * tan(glm::radians(60.0f)) * 20.0f;//need to be paramete!!!
	float heightP = widthP* (h / w);
	float heightF = widthF* (h / w);
	GLfloat VerticesP[] = {		       //color     //normals
		-widthP / 2,  heightP / 2,  0,    0,0, 1,    0, 0, 1,
		widthP / 2,  heightP / 2,   0,    0, 0, 1,   0, 0, 1,
		-widthP / 2, -heightP / 2,  0,    0, 0, 1,   0, 0, 1,
		widthP / 2,  -heightP / 2,  0,    0, 0, 1,   0, 0, 1,
		//
		-widthF / 2,  heightF / 2, -10,  0, 1, 1,    0, 0, 1,
		widthF / 2,  heightF / 2,  -10,  0, 1, 1,   0, 0, 1,
		-widthF / 2, -heightF / 2, -10,  0, 1, 1,   0, 0, 1,
		widthF / 2,  -heightF / 2, -10,  0, 1, 1,   0, 0, 1,
		//line
		0, 0, 0,                         1, 1, 1,   0, 0, 1,
		0, 0, -10,                        1, 1, 1,   0, 0, 1
	};

	glm::vec3 d1 =  glm::vec3(VerticesP[9 * 8], VerticesP[9 * 8 + 1], VerticesP[9 * 8 + 2]);  // no transformations
	glm::vec3 d2 =  glm::vec3(VerticesP[9 * 9], VerticesP[9 * 9 + 1], VerticesP[9 * 9 + 2]);
	glm::vec3 dir = d1 - d2;  //2-1?
	updateLine(d1, dir);

	GLshort ElementsP[] = {
		0, 1, 2,
		1, 3, 2,
		4, 5, 6,
		5, 7, 6,
		8, 9
	};


	glGenVertexArrays(1, &MyplaneVertexArrayObjectID);
	glGenBuffers(1, &MyPlaneBufferID);
	glGenBuffers(1, &MyPlaneIndexBufferID);

	glBindVertexArray(MyplaneVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, MyPlaneBufferID);

	glBufferData(GL_ARRAY_BUFFER, 10 * sizeof(GLfloat) * 9,   //sizeof(Vertex),
		&VerticesP[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MyPlaneIndexBufferID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 14 * sizeof(GLshort),
		&ElementsP[0], GL_STATIC_DRAW);

	glBindVertexArray(MyplaneVertexArrayObjectID);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, MyPlaneBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(sizeof(GLfloat) * 3));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(sizeof(GLfloat) * 6));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MyPlaneIndexBufferID);

}

void RayCastingModel::updatePlane(float width, float height,glm::vec3 direction, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	
	// Rotation aroun Y
	float projZ = glm::dot(glm::normalize(direction), glm::vec3(0.0f,0.0f,1.0f));
	float projX = glm::dot(glm::normalize(direction), glm::vec3(1.0f, 0.0f, 0.0f));
	glm::vec3 projXZ = glm::normalize(glm::vec3(projX, 0, projZ));
	float Yrot = direction.x > 0.0f ? acos(glm::dot(projXZ, glm::vec3(0.0f, 0.0f, 1.0f))): (float)PI * 2 - acos(glm::dot(projXZ, glm::vec3(0.0f, 0.0f, 1.0f)));
	// Rotation aroun X
	float dotY = glm::dot(glm::normalize(direction), glm::vec3(0.0f, 1.0f,0.0f));
	float Yangle = direction.z >0 ? acos(dotY) : (float)PI * 2 - acos(dotY);
	float Xrot = (Yangle -PI / 2);
	Xrot = glm::dot(glm::normalize(direction), glm::vec3(0.0f, 1.0f, 0.0f)) > 0 ? -(PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f)))) :																				  -(PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f))));
	-(PI / 2 - acos(glm::dot(direction, glm::vec3(0.0f, 1.0f, 0.0f))));

	//if(position!=glm::vec3(0,0,0))
	  setTranslation(position); //should be the camera position	
	setRotation(Xrot, Yrot, 0);

}

void RayCastingModel::updateRay(float x, float y,glm::vec3 pos)
{
	float far = 20.0f; float near = 0.1f;
	float XOffsetCoef = x / width;
	float YOffsetCoef = y / height;
	float widthN = 2 * tan(glm::radians(60.0f)) * near;
	float widthF = 2 * tan(glm::radians(60.0f)) * far; //real 20
	float heightN = widthN* (height / width);
	float heightF = widthF* (height / width);
	float Ray[] = {
		-widthN / 2 + XOffsetCoef*widthN, heightN / 2 - YOffsetCoef*heightN, 0,    0,1,1,  0, 0,1,
		-widthF / 2 + XOffsetCoef*widthF, heightF / 2 - YOffsetCoef*heightF, -far,  0,1,1,  0, 0,1
	};

	glm::vec3 d1 = glm::vec3(Ray[9 * 0], Ray[9 * 0 + 1], Ray[9 * 0 + 2]);  // no transformations
	glm::vec3 d2 = glm::vec3(Ray[9 * 1], Ray[9 * 1 + 1], Ray[9 * 1 + 2]);
	glm::vec3 dir = d2 - d1; 
	updateLine(d1, dir);

	glBindVertexArray(MyplaneVertexArrayObjectID);
	glBindBuffer(GL_ARRAY_BUFFER, MyPlaneBufferID);
	glBufferSubData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat) * 9, sizeof(Ray)/*size*/, &Ray/*data*/);

}

void RayCastingModel::updateLine(glm::vec3 dot, glm::vec3 direction)
{
	m_line.M = getModelMatrix() * vec4(dot, 1.0f); //origin
	m_line.p = getModelMatrix() * -glm::normalize(vec4(direction, 1.0f));  //direction
}

shObject RayCastingModel::calculateIntersaction(std::vector<shObject> objects)
{
	std::cout << "----Try-----------" << std::endl;
	std::multimap<shObject, glm::vec3> intersections;
	int debug_counter = 0;
	for (auto &obj : objects) {
		std::vector<glm::mat3> boundings = obj->getBoundingTriangles();
		dbb::line line;
		line.p =/* getModelMatrix() **/ glm::vec4(m_line.p, 1.0f);
		line.M = /*getModelMatrix() **/ glm::vec4(m_line.M, 1.0f);
		  
		//Debug
		/*for (auto &mat : boundings) {
			std::cout << "----Triangle--------" << std::endl;
			std::cout << " " << mat[0][0] << " " << mat[0][1] << " " << mat[0][2] << std::endl;
			std::cout << " " << mat[1][0] << " " << mat[1][1] << " " << mat[1][2] << std::endl;
			std::cout << " " << mat[2][0] << " " << mat[2][1] << " " << mat[2][2] << std::endl;
			std::cout << "---------------" << std::endl;
		  }
		std::cout << "----Line--------" << std::endl;
		std::cout << "M= " << line.M.x << " " << line.M.y << " " << line.M.z << std::endl;
		std::cout << "p= " << line.p.x << " " << line.p.y << " " << line.p.z << std::endl;
		std::cout << "---------------" << std::endl;*/
		  //End Debug

		for (auto& triangle : boundings) {
			dbb::plane plane(triangle);
			glm::vec3 inters = dbb::intersection(plane, line);
			if (dbb::IsInside(triangle, inters) && glm::dot(line.p , glm::vec3(inters-line.M))> 0.0f ) {
				intersections.insert(std::pair<shObject, glm::vec3>(obj, inters));
				std::cout << "Object= " << debug_counter << std::endl;
				/*std::cout << "intersaction added!" << std::endl;
				std::cout << "----Triangle--I--------" << std::endl;
				std::cout << " " << triangle[0][0] << " " << triangle[0][1] << " " << triangle[0][2] << std::endl;
				std::cout << " " << triangle[1][0] << " " << triangle[1][1] << " " << triangle[1][2] << std::endl;
				std::cout << " " << triangle[2][0] << " " << triangle[2][1] << " " << triangle[2][2] << std::endl;
				std::cout << "----Triangle--Intersaction------" << std::endl;
				std::cout << " " << inters.x << " " << inters.y << " " << inters.z << std::endl;
				std::cout << "intersaction finished!" << std::endl;
				std::cout << "---------------" << std::endl;*/
			}
		}
		++debug_counter;
	}
	if (intersections.empty() ) {
		std::cout << "No intersactions found!" << std::endl;
		return nullptr;
	}
	else {
		std::cout << "Intersactions found!!!" << std::endl;
		float length = 1000.0f;  //should be in line with far plane!!!!!!!
		shObject obj = nullptr;
		for (auto &inter : intersections) {
			if (glm::length(/*getModelMatrix() **/ m_line.M - inter.second) < length) { // 
				length = glm::length(/*getModelMatrix() **/ m_line.M - inter.second);
				obj = inter.first;
				std::cout << "length=" << length << std::endl;
			}
		}
		return obj;
	}
}

void RayCastingModel::Draw(GLuint shader)
{
	// Draw mesh
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glUniform1i(glGetUniformLocation(shader, "texture_diffuse1"), 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glUniform1i(glGetUniformLocation(shader, "texture_specular1"), 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glUniform1i(glGetUniformLocation(shader, "texture_normal1"), 4);

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, texture->id);
	glUniform1i(glGetUniformLocation(shader, "texture_fourth1"), 5);

	glBindVertexArray(MyplaneVertexArrayObjectID);
	glDrawElements(GL_TRIANGLES,12, GL_UNSIGNED_SHORT, 0);//12!!!
	glDrawElements(GL_LINES, 2, GL_UNSIGNED_SHORT, (void*)(sizeof(GLushort) * 12));
	glBindVertexArray(0);
}
