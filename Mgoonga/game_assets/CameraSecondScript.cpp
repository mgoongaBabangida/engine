#include "stdafx.h"
#include "CameraSecondScript.h"

//----------------------------------------------------
CameraSecondScript::CameraSecondScript(Camera* _camera, IGame* _game)
  : m_camera(_camera)
  , m_game(_game)
{

}

//----------------------------------------------------
void	CameraSecondScript::Update(float _tick)
{
  if (shObject object = m_object.lock(); object)
  {
    m_camera->setPosition(object->GetTransform()->getTranslation());
    m_camera->setDirection(object->GetTransform()->getRotationVector());
    m_camera->UpdateProjectionMatrix();
    m_game->SetFramed(m_camera->getCameraRay().FrustumCull(m_game->GetObjects()));
  }
}
