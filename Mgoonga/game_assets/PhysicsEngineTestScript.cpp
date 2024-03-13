#include "stdafx.h"

#include "PhysicsEngineTestScript.h"
#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "ModelManagerYAML.h"

#include <math/Geometry.h>
#include <math/GeometryFunctions.h>
#include <math/RigidBodyDBB.h>
#include <math/PhysicsSystem.h>

//------------------------------------------------------------------------------
PhysicsEngineTestScript::PhysicsEngineTestScript(eMainContextBase* _game)
  : m_game(_game)
{
}

//------------------------------------------------------------------------------
PhysicsEngineTestScript::~PhysicsEngineTestScript()
{
}

//------------------------------------------------------------------------------
bool PhysicsEngineTestScript::OnKeyPress(uint32_t _asci, KeyModifiers _modifier)
{
  switch (_asci)
  {
    case ASCII_J:
    {
      dbb::OBB obb1 = cube1->GetCollider()->GetOBB(*cube1->GetTransform()).value();
      dbb::sphere sph1 = sphere1->GetCollider()->GetSphere(*sphere1->GetTransform()).value();
      dbb::OBB obb2 = cube2->GetCollider()->GetOBB(*cube2->GetTransform()).value();
      dbb::sphere sph2 = sphere2->GetCollider()->GetSphere(*sphere2->GetTransform()).value();

      if (false)
      {
        //visualize obb
        ObjectFactoryBase factory;
        for (size_t i = 0; i < obb1.GetVertices().size(); ++i)
        {
          shObject obj = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "OBB vertex" + std::to_string(i));
          obj->GetTransform()->setTranslation(obb1.GetVertices()[i]);
          obj->GetTransform()->setScale({ 0.05f,0.05f,0.05f });
          m_visual.push_back(obj);
        }
      }

      auto manifold = dbb::FindCollision(obb1, sph1);
      manifold = dbb::FindCollision(sph1, sph2);
      manifold = dbb::FindCollision(obb1, obb2);
      if (manifold.colliding)
      {
        //visualize collision
        ObjectFactoryBase factory;
        std::vector<dbb::point> linePoints;
        std::vector <unsigned int> lineIndexes;
        for (size_t i = 0; i < manifold.contacts.size(); ++i)
        {
          shObject obj = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "OBB vertex" + std::to_string(i));
          obj->GetTransform()->setTranslation(manifold.contacts[i]);
          obj->GetTransform()->setScale({ 0.05f,0.05f,0.05f });
          m_visual.push_back(obj);

          linePoints.push_back(manifold.contacts[i]);
          linePoints.push_back(manifold.contacts[i] + (- manifold.normal * (manifold.depth/2)));
          lineIndexes.push_back(i * 2);
          lineIndexes.push_back(i * 2 + 1);
        }
        m_normal_mesh->UpdateData(linePoints,lineIndexes, { 1.0f, 1.0f ,0.0f, 1.0f });
      }
      break;
    }
    case ASCII_K:
    {
      for (auto& obj : m_game->GetObjects())
      {
        if (obj->Name().find("OBB vertex") != std::string::npos)
          m_game->DeleteObject(obj);
        m_normal_mesh->UpdateData({}, {}, { 1.0f, 1.0f ,0.0f, 1.0f });
      }
      break;
    }
    default: return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void PhysicsEngineTestScript::Update(float _tick)
{
  if (!m_visual.empty())
  {
    for (size_t i = 0; i < m_visual.size(); ++i)
    {
      m_game->AddObject(m_visual.back());
      m_visual.pop_back();
    }
  }
}

//------------------------------------------------------------------------------
void PhysicsEngineTestScript::Initialize()
{
  ObjectFactoryBase factory;
  m_normal_mesh = new LineMesh({}, {}, glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_normal_mesh), eObject::RenderType::LINES, "Normal mesh"));

  sphere1 = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR1");
  sphere1->GetTransform()->setTranslation(glm::vec3(-2.0f, 0.0f, 0.0f));
  m_game->AddObject(sphere1);

  cube1 = factory.CreateObject(m_game->GetModelManager()->Find("Chair"), eObject::RenderType::PBR, "Cube1");
  cube1->GetTransform()->setTranslation(glm::vec3(0.0f, 0.0f, 0.0f));
  m_game->AddObject(cube1);

  sphere2 = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR2");
  sphere2->GetTransform()->setTranslation(glm::vec3(-4.0f, 0.0f, 0.0f));
  m_game->AddObject(sphere2);

  cube2 = factory.CreateObject(m_game->GetModelManager()->Find("wall_cube"), eObject::RenderType::PHONG, "Cube2");
  cube2->GetTransform()->setTranslation(glm::vec3(2.0f, 0.0f, 0.0f));
  m_game->AddObject(cube2);
}
