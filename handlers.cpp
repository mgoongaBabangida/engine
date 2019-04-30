#include "stdafx.h"
#include <glm\glm\glm.hpp>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm/gtc/constants.hpp>
#include "Structures.h"
//
//void dbGLWindowSDL::mouseMoveEvent(QMouseEvent* e)
//{
//	inputController.OnMouseMove(e->x(), e->y());
//	//m_camera.mouseUpdate(glm::vec2(e->x(), e->y()));
//	//m_framed = camRay.onMove(m_camera, m_Objects, e->x(), e->y());
//}
//
//void dbGLWindowSDL::mouseReleaseEvent(QMouseEvent* e)
//{
//	inputController.OnMouseRelease();
//	//camRay.release();
//}
//
//void dbGLWindowSDL::keyPressEvent(QKeyEvent * e)
//{
//	inputController.OnKeyPress(e->key());
//}
//
//void dbGLWindowSDL::mousePressEvent(QMouseEvent * e)
//{
//	inputController.OnMousePress(e->x(), e->y(), e->button() == Qt::LeftButton);
//
//	//std::cout <<"e->x()"<< e->x() << " " << e->y() << std::endl;
//	//if (e->button() == Qt::LeftButton)
//	//{
//	//	for (auto& gui : guis)
//	//	{
//	//		if (gui.isPressed(e->x(), e->y()))
//	//			gui.Perssed();
//	//	}
//
//	//	camRay.press(e->x(), e->y());
//	//	camRay.Update(m_camera, e->x(), e->y(), width(), height());
//	//	m_focused = camRay.calculateIntersaction(m_Objects);
//	//}
//	//else if (e->button() == Qt::RightButton)
//	//{
//	//	dbb::plane pl(glm::vec3(1.0f, waterHeight, 1.0f), glm::vec3(0.0f, waterHeight, 0.0f), glm::vec3(0.0f, waterHeight, 1.0f)); // arbitrary triangle on waterHeight plane
//	//	camRay.Update(m_camera, e->x(), e->y(), width(), height());
//	//	glm::vec3 target = dbb::intersection(pl, camRay.getLine());
//
//	//	if(m_focused != nullptr)
//	//	{
//	//		m_focused->getScript()->SetDestination(target);
//	//	}
//#ifdef DEBUG_HANDLERS
//		std::cout << "TARGET=" <<"x= "<< target.x << "y= " << target.y << "z= " << target.z << std::endl;
//
//		glm::vec3 position = glm::vec3(0.0f, 2.0f, 0.0f);
//		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
//		glm::vec3 target_dir = glm::normalize(target - position);
//		float angle = glm::dot(glm::normalize(target_dir), glm::normalize(direction));
//		std::cout << "in press" << std::endl;
//		std::cout << "dot= " << angle << "radians= " << glm::acos(angle) << " degrees= " << glm::degrees(glm::acos(angle)) << std::endl;
//		glm::quat rot = glm::toQuat(glm::rotate(UNIT_MATRIX, glm::acos(angle), glm::vec3(0,1,0)));
//
//		glm::vec3 ASIX = glm::normalize(glm::cross(target_dir, direction));
//		std::cout <<"Asix="<< ASIX.x << ASIX.y << ASIX.z << std::endl;
//#endif
//		//}
//}

