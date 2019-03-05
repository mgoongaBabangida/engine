#pragma once
#include "Object.h"
#include "PlaneLine.h"
#include"Structures.h"

struct Texture;


class RayCastingModel :public eObject
{
	float Width;
	float Height;
	dbb::line m_line;
	void updateLine(glm::vec3 dot, glm::vec3 direction);
	Texture* texture;
public:
	RayCastingModel(float Width,float Height,Texture* t);
	void updatePlane(float Width, float Height, glm::vec3 direction, glm::vec3 position);
	void updateRay(float x, float y, glm::vec3 pos);

	shObject calculateIntersaction(std::vector<shObject> objects);
	void Draw(GLuint shader);
	virtual ~RayCastingModel() {}
	dbb::line getRay() { return m_line; }
	GLuint MyplaneVertexArrayObjectID;
	GLuint MyPlaneBufferID;
	GLuint MyPlaneIndexBufferID;

};



//
//class PlaneIniter
//{
//public:
//	static void InitMyPlaneArrays(GLuint &MyplaneVertexArrayObjectIDF, GLuint &MyPlaneBufferIDF, GLuint &MyPlaneIndexBufferIDF, float w, float h)
//	{
//		float widthP = 2 * tan(glm::radians(60.0f)) * 0.2;
//		float widthF = 2 * tan(glm::radians(60.0f)) * 10;//real is 20
//		float heightP = widthP* (h / w);
//		float heightF = widthF* (h / w);
//		GLfloat VerticesP[] = {		       //color     //normals
//			-widthP / 2, heightP / 2, 0,    0,0, 1,    0, 0, 1,
//			widthP / 2, heightP / 2,  0,    0, 0, 1,   0, 0, 1,
//			-widthP / 2,-heightP / 2, 0,    0, 0, 1,   0, 0, 1,
//			widthP / 2, -heightP / 2, 0,    0, 0, 1,   0, 0, 1,
//			//
//			-widthF / 2, heightF / 2, -20,  0,1, 1,    0, 0, 1,
//			widthF / 2, heightF / 2,  -20,  0, 1, 1,   0, 0, 1,
//			-widthF / 2,-heightF / 2, -20,  0, 1, 1,   0, 0, 1,
//			widthF / 2, -heightF / 2, -20,  0, 1, 1,   0, 0, 1,
//			//line
//			0, 0, 0,                         1, 1, 1,   0, 0, 1,
//			0, 0, -5,                        1, 1, 1,   0, 0, 1
//		};
//
//		/*while (true) {
//		static float i = 0.0f; ++i;
//		std::cout <<tan(glm::radians(i))<<" "<<i<<std::endl;
//		if (i == 360.0f) break;
//		}*/
//
//		/*std::cout <<" widthP="<<widthP << std::endl;
//		std::cout << heightP << std::endl;
//		std::cout << (this->Height() / this->Width()) << std::endl;*/
//
//		GLshort ElementsP[] = {
//			0, 1, 2,
//			1, 3, 2,
//			4, 5, 6,
//			5, 7, 6,
//			8, 9
//		};
//
//		glGenVertexArrays(1, &MyplaneVertexArrayObjectIDF);
//		glGenBuffers(1, &MyPlaneBufferIDF);
//		glGenBuffers(1, &MyPlaneIndexBufferIDF);
//
//		glBindVertexArray(MyplaneVertexArrayObjectIDF);
//		glBindBuffer(GL_ARRAY_BUFFER, MyPlaneBufferIDF);
//
//		glBufferData(GL_ARRAY_BUFFER, 10 * sizeof(GLfloat) * 9,   //sizeof(Vertex),
//			&VerticesP[0], GL_STATIC_DRAW);
//
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MyPlaneIndexBufferIDF);
//		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 14 * sizeof(GLshort),
//			&ElementsP[0], GL_STATIC_DRAW);
//
//
//		glBindVertexArray(MyplaneVertexArrayObjectIDF);
//		glEnableVertexAttribArray(0);
//		glEnableVertexAttribArray(1);
//		glEnableVertexAttribArray(2);
//		glBindBuffer(GL_ARRAY_BUFFER, MyPlaneBufferIDF);
//		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, 0);
//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(sizeof(GLfloat) * 3));
//		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (void*)(sizeof(GLfloat) * 6));
//		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, MyPlaneIndexBufferIDF);
//	}
//};