#include "stdafx.h"

#include "ShootScript.h"
#include "ObjectFactory.h"

#include <opengl_assets/ModelManager.h>

#include <glm/glm/gtx/norm.hpp>

namespace
{
  class BulletScript : public IScript
  {
  public:
    BulletScript(IGame* _game, LineMesh* _normal_mesh) : m_game(_game), m_normal_mesh(_normal_mesh) {}
    void Update(float _tick)
    {
     /*if( shObject object = m_object.lock(); object)
      object->GetRigidBody()->Update(_tick, m_game->GetObjects());*/
    }

    virtual void	CollisionCallback(const eCollision& _collision)
    {
      if (shObject object = m_object.lock(); object)
      {
        if (_collision.collider == object.get())
        {
          glm::vec3 p1(_collision.triangle[0][0], _collision.triangle[0][1], _collision.triangle[0][2]);
          glm::vec3 p2(_collision.triangle[1][0], _collision.triangle[1][1], _collision.triangle[1][2]);
          glm::vec3 p3(_collision.triangle[2][0], _collision.triangle[2][1], _collision.triangle[2][2]);

          glm::vec3 v1 = glm::normalize(p1 - p2);
          glm::vec3 v2 = glm::normalize(p3 - p2);
          glm::vec3 normal = glm::normalize(glm::cross(v1, v2));

          //if (glm::dot(normal, object->GetRigidBody()->Velocity()) > 0.0f)
            normal = -normal;

          glm::vec3 reflected_velocity /*= glm::reflect(object->GetRigidBody()->Velocity(), normal)*/; //incident - 2.0f * glm::dot(incident, normal) * normal;
          if (m_normal_mesh)
          {
            /*m_normal_mesh->UpdateData({ _collision.intersaction,
                                        _collision.intersaction + normal,
                                        _collision.intersaction + glm::normalize((-object->GetRigidBody()->Velocity())),
                                        _collision.intersaction + glm::normalize(reflected_velocity) },*/
              /*{ 0 , 1 , 0 , 2, 0, 3 },
              { 1.0f, 1.0f ,0.0f, 1.0f });*/
          }

          //object->GetRigidBody()->SetCurrentVelocity(m_normal_mesh == nullptr ? reflected_velocity : glm::vec3{ 0,0,0 });
          ////object->GetTransform()->setTranslation(object->GetTransform()->getTranslation() += (reflected_velocity) );
        }
      }
    }
  protected:
    IGame* m_game = nullptr;
    LineMesh* m_normal_mesh = nullptr;
  };
}

//---------------------------------------
ShootScript::ShootScript(IGame* _game, eModelManager* _modelManager)
  : m_game(_game)
  , m_modelManager(_modelManager)
{
}

//---------------------------------------
ShootScript::~ShootScript()
{
}

//---------------------------------------
bool ShootScript::OnKeyJustPressed(uint32_t _asci, KeyModifiers _modifier)
{
  switch (_asci)
  {
  case ASCII_J:
  {
    ObjectFactoryBase factory;
    ++m_counter;
    shObject obj = factory.CreateObject(m_modelManager->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR" + std::to_string(m_counter));
    obj->SetScript(new ::BulletScript(m_game, m_deubg_normals ? m_normal_mesh : nullptr));
    obj->GetTransform()->setTranslation(m_game->GetMainCameraPosition());
    obj->GetTransform()->setScale({ 0.1f,0.1f,0.1f });
    //obj->GetRigidBody()->ApplyImpulse(m_game->GetMainCameraDirection(), 0.01f, 1.0f);
    //obj->SetInstancingTag("sphere");
    //m_obj_to_add->GetRigidBody()->ApplyAcceleration(s_gravity); //?
    m_obj_to_add = obj;
    break;
  }
  case ASCII_K:
  {
    if (!m_deubg_normals)
    {
      ObjectFactoryBase factory;
      m_normal_mesh = new LineMesh({}, {}, glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
      m_deubg_normals = true;
      m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_normal_mesh), eObject::RenderType::LINES, "Normal mesh"));
    }
    else
    {
      m_deubg_normals = false;
      m_normal_mesh = nullptr;
    }
    break;
  }
  case ASCII_I:
  {
    m_delete_all = true;
    break;
  }
 
  default: return false;
  }
  return true;
}

//----------------------------------------
void ShootScript::Update(float _tick)
{
  if (m_obj_to_add != nullptr)
  {
    m_game->AddObject(m_obj_to_add);
    m_bullets.push_back(m_obj_to_add);
    m_obj_to_add = nullptr;
  }

  if (m_delete_all)
  {
    for (auto& bullet : m_bullets)
    {
      m_game->DeleteObject(bullet);
    }
    m_bullets.clear();
    m_delete_all = false;
  }

  for (auto& bullet : m_bullets)
  {
    if (glm::length2(m_game->GetMainCameraPosition() - bullet->GetTransform()->getTranslation()) > 1'000.0f) // @todo camera far plane ^ 2
    {
      m_game->DeleteObject(bullet);
      //remove erase from bullets;
    }
  }
}

//------------------------------------------------
void ShootScript::Initialize()
{
  ObjectFactoryBase factory;
  m_normal_mesh = new LineMesh({}, {}, glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
  if(m_deubg_normals)
   m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_normal_mesh), eObject::RenderType::LINES, "Normal mesh"));
}
