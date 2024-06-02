#include "stdafx.h"

#include "BezierCurveUIController.h"
#include "MainContextBase.h"
#include "InputStrategy.h"

#include <opengl_assets/MyMesh.h>
#include <opengl_assets/GUI.h>
#include <math/Rect.h>

//------------------------------------------------------
BezierCurveUIController::BezierCurveUIController(eMainContextBase* _game, shObject _bezier_object, float _control_point_size, const Texture* _window_texture)
: m_game(_game)
{
  auto bezier_objects = _bezier_object->GetChildrenObjects();

  bezier_objects[0]->GetTransform()->setScale({ _control_point_size, _control_point_size, _control_point_size });
  bezier_objects[1]->GetTransform()->setScale({ _control_point_size, _control_point_size, _control_point_size });
  bezier_objects[2]->GetTransform()->setScale({ _control_point_size, _control_point_size, _control_point_size });
  bezier_objects[3]->GetTransform()->setScale({ _control_point_size, _control_point_size, _control_point_size });

  m_bezier_mesh = dynamic_cast<const BezierCurveMesh*>(_bezier_object->GetModel()->GetMeshes()[0]);
  if (m_bezier_mesh)
    m_bezier = &const_cast<BezierCurveMesh*>(m_bezier_mesh)->GetBezier();

  if (m_bezier)
  {
    bezier_objects[0]->GetTransform()->setTranslation(m_bezier->p0);
    bezier_objects[1]->GetTransform()->setTranslation(m_bezier->p1);
    bezier_objects[2]->GetTransform()->setTranslation(m_bezier->p2);
    bezier_objects[3]->GetTransform()->setTranslation(m_bezier->p3);
  }

  if (_window_texture)
  {
    dbb::Rect window_rect;
    window_rect.m_top_left = { 50, 50 };
    window_rect.m_size = { 900, 500 };
    m_window = std::make_shared<GUI>(window_rect, m_game->Width(), m_game->Height());
    m_window->SetTexture(*_window_texture, { 0,0 }, { _window_texture->m_width, _window_texture->m_height });
    m_window->SetTransparent(true);
    m_window->SetTakeMouseEvents(true);
    m_window->SetVisible(true);

    m_window->setCommand(std::make_shared<GUICommand>([this]()
      {
        dbb::Rect close_button_rect;
        close_button_rect.m_top_left = { 850 + 50, 550 }; // inverted y check
        close_button_rect.m_size = { 50, 50 };
        if (close_button_rect.IsInside({ m_cursor_x , m_cursor_y }))
        {
          //clean up
          m_game->DeleteInputObserver(m_window.get());
          m_game->DeleteGUI(m_window);
          ToolFinished.Occur(*m_bezier);
          m_game->SetInputStrategy(nullptr);
          m_game->DeleteInputObserver(this);
          m_closed = true;
        }
      }));

    m_game->AddGUI(m_window);
    m_game->AddInputObserver(m_window.get(), STRONG);
    m_game->AddInputObserver(this, WEAK);
    m_game->SetInputStrategy(new InputStrategy2DMove(m_game));
  }
  else
    m_game->SetInputStrategy(new InputStrategyMoveAlongXZPlane(m_game->GetMainCamera(), GetObjectsWithChildren(m_game->GetObjects()))); //3d
}

//------------------------------------------------------
BezierCurveUIController::~BezierCurveUIController()
{
  m_game->SetInputStrategy(nullptr);
}

//------------------------------------------------------
void BezierCurveUIController::Update(float _tick)
{
  if (m_bezier)
  {
    if (auto obj = m_object.lock(); obj)
    {
      if (!m_closed)
      {
        auto bezier_objects = obj->GetChildrenObjects();
        m_bezier->p0 = bezier_objects[0]->GetTransform()->getTranslation();
        m_bezier->p1 = bezier_objects[1]->GetTransform()->getTranslation();
        m_bezier->p2 = bezier_objects[2]->GetTransform()->getTranslation();
        m_bezier->p3 = bezier_objects[3]->GetTransform()->getTranslation();
        const_cast<BezierCurveMesh*>(m_bezier_mesh)->Update();
      }
      else
      {
        m_game->DeleteObject(obj);
      }
    }
  }
}

//------------------------------------------------------
bool BezierCurveUIController::OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifiers)
{
  m_cursor_x = (float)_x;
  m_cursor_y = (float)(m_game->Height() - _y);
  return false;
}
