#include "stdafx.h"

#include "BezierCurveUIController.h"
#include <opengl_assets/MyMesh.h>

//------------------------------------------------------
BezierCurveUIController::BezierCurveUIController(const std::array<shObject, 5>& _bezier_objects)
: m_bezier_objects(_bezier_objects)
{
  m_bezier_objects[1]->GetTransform()->setScale({ 0.1f,0.1f,0.1f });
  m_bezier_objects[2]->GetTransform()->setScale({ 0.1f,0.1f,0.1f });
  m_bezier_objects[3]->GetTransform()->setScale({ 0.1f,0.1f,0.1f });
  m_bezier_objects[4]->GetTransform()->setScale({ 0.1f,0.1f,0.1f });

  m_bezier_mesh = dynamic_cast<const BezierCurveMesh*>(m_bezier_objects[0]->GetModel()->GetMeshes()[0]);
  if (m_bezier_mesh)
    m_bezier = &const_cast<BezierCurveMesh*>(m_bezier_mesh)->GetBezier();

  if (m_bezier)
  {
    m_bezier_objects[1]->GetTransform()->setTranslation(m_bezier->p0);
    m_bezier_objects[2]->GetTransform()->setTranslation(m_bezier->p1);
    m_bezier_objects[3]->GetTransform()->setTranslation(m_bezier->p2);
    m_bezier_objects[4]->GetTransform()->setTranslation(m_bezier->p3);
  }
}

//------------------------------------------------------
BezierCurveUIController::~BezierCurveUIController()
{
}

//------------------------------------------------------
void BezierCurveUIController::Update(std::vector<std::shared_ptr<eObject>> objs)
{
  if (m_bezier)
  {
    m_bezier->p0 = m_bezier_objects[1]->GetTransform()->getTranslation();
    m_bezier->p1 = m_bezier_objects[2]->GetTransform()->getTranslation();
    m_bezier->p2 = m_bezier_objects[3]->GetTransform()->getTranslation();
    m_bezier->p3 = m_bezier_objects[4]->GetTransform()->getTranslation();
    const_cast<BezierCurveMesh*>(m_bezier_mesh)->Update();
  }
}
