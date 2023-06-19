#include "stdafx.h"

#include "CameraFreeController.h"

//-----------------------------------------------------------
CameraFreeController::CameraFreeController(Camera& _camera)
: m_camera(_camera)
{

}

//-----------------------------------------------------------
CameraFreeController::~CameraFreeController()
{
}

//----------------------------------------------------------
void	CameraFreeController::Update(float _tick)
{
}

//-----------------------------------------------------------
bool CameraFreeController::OnMouseMove(int32_t _x, int32_t _y)
{
	m_camera.get().mouseUpdate(glm::vec2(_x, _y));
	return true;
}

//-----------------------------------------------------------
bool CameraFreeController::OnKeyPress(uint32_t _asci)
{
	switch (_asci)
	{
	case ASCII_W: m_camera.get().moveForward();  return true;
	case ASCII_S: m_camera.get().moveBackword(); return true;
	case ASCII_D: m_camera.get().strafeLeft();   return true;
	case ASCII_A: m_camera.get().strafeRight();  return true;
	case ASCII_R: m_camera.get().moveUp();		  return true;
	case ASCII_F: m_camera.get().moveDown();	  return true;
	default:					  return false;
	}
	return false;
}

//-----------------------------------------------------------
bool CameraFreeController::OnMouseWheel(int32_t _x, int32_t _y)
{
	if(_y > 0)
		m_camera.get().moveForward();
	else if(_y < 0)
		m_camera.get().moveBackword();
	return true;
}
