#include "stdafx.h"

#include "BezierCurveUIController.h"
#include <opengl_assets/MyMesh.h>

//------------------------------------------------------
BezierCurveUIController::BezierCurveUIController(shObject _bezier_object)
: m_bezier_object(_bezier_object)
{
  auto bezier_objects = m_bezier_object->GetChildrenObjects();

  bezier_objects[0]->GetTransform()->setScale({ 0.02f, 0.02f, 0.02f });
  bezier_objects[1]->GetTransform()->setScale({ 0.02f, 0.02f, 0.02f });
  bezier_objects[2]->GetTransform()->setScale({ 0.02f, 0.02f, 0.02f });
  bezier_objects[3]->GetTransform()->setScale({ 0.02f, 0.02f, 0.02f });

  m_bezier_mesh = dynamic_cast<const BezierCurveMesh*>(m_bezier_object->GetModel()->GetMeshes()[0]);
  if (m_bezier_mesh)
    m_bezier = &const_cast<BezierCurveMesh*>(m_bezier_mesh)->GetBezier();

  if (m_bezier)
  {
    bezier_objects[0]->GetTransform()->setTranslation(m_bezier->p0);
    bezier_objects[1]->GetTransform()->setTranslation(m_bezier->p1);
    bezier_objects[2]->GetTransform()->setTranslation(m_bezier->p2);
    bezier_objects[3]->GetTransform()->setTranslation(m_bezier->p3);
  }
}

//------------------------------------------------------
BezierCurveUIController::~BezierCurveUIController()
{
}

//------------------------------------------------------
void BezierCurveUIController::Update(float _tick)
{
  if (m_bezier)
  {
    auto bezier_objects = m_bezier_object->GetChildrenObjects();
    m_bezier->p0 = bezier_objects[0]->GetTransform()->getTranslation();
    m_bezier->p1 = bezier_objects[1]->GetTransform()->getTranslation();
    m_bezier->p2 = bezier_objects[2]->GetTransform()->getTranslation();
    m_bezier->p3 = bezier_objects[3]->GetTransform()->getTranslation();
    const_cast<BezierCurveMesh*>(m_bezier_mesh)->Update();
  }
}
