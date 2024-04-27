#include "stdafx.h"
#include "CameraFreeController.h"

//-----------------------------------------------------------
CameraFreeController::CameraFreeController(Camera& _camera, bool _strafe_shift)
: m_camera(_camera)
, m_strafe_shift(_strafe_shift)
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
bool CameraFreeController::OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifiers)
{
	if ((m_strafe_shift && _modifiers == KeyModifiers::SHIFT) || !m_strafe_shift)
		m_camera.get().StrafeThresholdRef() = 5.0f;
	else
		m_camera.get().StrafeThresholdRef() = 0.0f;

	m_camera.get().mouseUpdate(glm::vec2(_x, _y));
	return true;
}

//-----------------------------------------------------------
bool CameraFreeController::OnKeyJustPressed(uint32_t _asci, KeyModifiers _modifier)
{
	switch (_asci)
	{
		case ASCII_W:	m_camera.get().moveForward();	return true;
		case ASCII_S:	m_camera.get().moveBackword();	return true;
		case ASCII_D:	m_camera.get().strafeLeft();	return true;
		case ASCII_A:	m_camera.get().strafeRight();	return true;
		case ASCII_R:	m_camera.get().moveUp();		return true;
		case ASCII_F:	m_camera.get().moveDown();		return true;
		default:										return false;
	}
	return false;
}

//-----------------------------------------------------------
bool CameraFreeController::OnKeyPress(const std::vector<bool> _keys, KeyModifiers _modifier)
{
	static std::vector<ASCII> asci{ASCII_W, ASCII_S, ASCII_D, ASCII_A, ASCII_R, ASCII_F};
	for(const auto a : asci)
	{
		if(_keys[a])
		{
			OnKeyJustPressed(a, _modifier);
		}
	}
	return true;
}

//-----------------------------------------------------------
bool CameraFreeController::OnMouseWheel(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
	if(_y > 0)
		m_camera.get().moveForward();
	else if(_y < 0)
		m_camera.get().moveBackword();
	return true;
}
