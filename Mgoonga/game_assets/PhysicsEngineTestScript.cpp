#include "stdafx.h"

#include "PhysicsEngineTestScript.h"
#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "ModelManagerYAML.h"

#include <math/Geometry.h>
#include <math/GeometryFunctions.h>
#include <math/RigidBodyDBB.h>
#include <math/PhysicsSystem.h>

#include <sdl_assets/ImGuiContext.h>

//------------------------------------------------------------------------------
PhysicsEngineTestScript::PhysicsEngineTestScript(eMainContextBase* _game, IWindowImGui* _imgui)
  : m_game(_game)
  , m_imgui(_imgui)
{
  m_imgui->Add(CHECKBOX, "Add Sphere", (void*)&m_add_sphere);
  m_imgui->Add(CHECKBOX, "Add Chair", (void*)&m_add_cube1);
  m_imgui->Add(CHECKBOX, "Add Cube", (void*)&m_add_cube2);
  m_imgui->Add(SLIDER_FLOAT, "Terrain Mass", (void*)&m_terrain_mass);

  m_imgui->Add(CHECKBOX, "Linear Impulses Only", (void*)&m_linear_impulses_only);
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Coefitient of Restitution", &m_restitution);
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Friction", &m_friction);

  m_imgui->Add(SLIDER_FLOAT_NERROW, "Projection Percent", &m_projection_percent);

  static std::function<void(int)> slack_callback = [this](int _value)
  {
    m_penetration_slack = float(_value) / 100.f;
  };
  m_imgui->Add(SPIN_BOX, "Penetration Slack", &slack_callback);
  
  static std::function<void(int)> impulse_iterations_callback = [this](int _value)
  {
    m_impulse_iterations = _value;
  };
  m_imgui->Add(SPIN_BOX, "Impulse Iterations", &impulse_iterations_callback);
}

//------------------------------------------------------------------------------
PhysicsEngineTestScript::~PhysicsEngineTestScript()
{
  m_timer->stop();
}

//------------------------------------------------------------------------------
bool PhysicsEngineTestScript::OnKeyPress(uint32_t _asci, KeyModifiers _modifier)
{
  switch (_asci)
  {
    case ASCII_J:
    {
      if (false)
      {
        //visualize obb
        ObjectFactoryBase factory;
        std::optional<dbb::OBB> obb1 = cube1.second->GetCollider()->GetBox();
        if (obb1.has_value())
        {
          for (size_t i = 0; i < obb1->GetVertices().size(); ++i)
          {
            shObject obj = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "OBB vertex" + std::to_string(i));
            obj->GetTransform()->setTranslation(obb1->GetVertices()[i]);
            obj->GetTransform()->setScale({ 0.05f,0.05f,0.05f });
            m_visual.push_back(obj);
          }
        }
      }

      auto manifold = dbb::ICollider::FindCollisionFeatures(*cube1.second->GetCollider(), *sphere1.second->GetCollider());
      manifold = dbb::ICollider::FindCollisionFeatures(*sphere1.second->GetCollider(), *sphere2.second->GetCollider());
      manifold = dbb::ICollider::FindCollisionFeatures(*cube1.second->GetCollider(), *cube2.second->GetCollider());
      manifold = dbb::ICollider::FindCollisionFeatures(*grassPlane.second->GetCollider(), *cube1.second->GetCollider());
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
    case ASCII_L:
    {
      ClearVisulaDebugInfo();
      break;
    }
    case ASCII_K:
    {
      m_simulation_on = !m_simulation_on;
      break;
    }
    case ASCII_X:
    {
      m_reset = true;
      break;
    }
    case ASCII_V:
    {
      sphere2.first->GetTransform()->setTranslation(m_game->GetMainCameraPosition());
      sphere2.first->GetTransform()->setScale({ 0.1,0.1,0.1 });
      sphere2.second->AddLinearImpulse(m_game->GetMainCameraDirection() * 25.f);

      dbb::sphere sph2 = sphere2.first->GetCollider()->GetSphere(*sphere2.first->GetTransform()).value(); // get from old collider!
      dbb::ICollider* c_sph2 = new dbb::SphereCollider(sph2);
      sphere2.second->SetCollider(c_sph2);

      break;
    }
    default: return false;
  }
  return true;
}

//------------------------------------------------------------------------------
void PhysicsEngineTestScript::Update(float _tick)
{
  if(m_reset)
    Reset();

  if (m_simulation_on)
  {
    m_physics_system->Update(_tick);
  }

  {
    if (!m_simulation_on) // preparation
    {
      dbb::OBB obb1 = cube1.first->GetCollider()->GetOBB(*cube1.first->GetTransform()).value(); // get obb from old collider!
      dbb::ICollider* c_obb1 = new dbb::OBBCollider(obb1);
      cube1.second->SetCollider(c_obb1);
    }

    //update transforms with the collider data to visualize correctily
    dbb::sphere s1 =      sphere1.second->GetCollider()->GetSphere().value();
    dbb::OBB c1 =         cube1.second->GetCollider()->GetBox().value();
    dbb::sphere s2 =      sphere2.second->GetCollider()->GetSphere().value();
    dbb::OBB c2 =         cube2.second->GetCollider()->GetBox().value();
    dbb::OBB grass_obb =  grassPlane.second->GetCollider()->GetBox().value();

    m_normal_mesh->UpdateData(dbb::Unite(c1.GetVertices(), grass_obb.GetVertices()),
      { 6,1, 6,3, 6,4, 2,7, 2,5, 2,0, 0,1, 0,3, 7,1, 7,4, 4,5, 5,3,
        6+8, 1 + 8, 6 + 8,3 + 8, 6 + 8,4 + 8, 2 + 8,7 + 8, 2 + 8,5 + 8, 2 + 8,0 + 8, 0 + 8,1 + 8, 0 + 8,3 + 8, 7 + 8,1 + 8, 7 + 8,4 + 8, 4 + 8,5 + 8, 5 + 8,3 + 8 },
      { 1.0f, 1.0f ,0.0f, 1.0f });

    if (m_simulation_on)
    {
      cube1.first->GetTransform()->setRotation(glm::toQuat(c1.orientation)); //works incorrectly!
      cube1.first->GetTransform()->setTranslation(c1.origin - (cube1.first->GetTransform()->getRotation() * cube1.first->GetCollider()->GetCenter()));

      sphere1.first->GetTransform()->setTranslation(s1.position - sphere1.first->GetCollider()->GetCenter());

      cube2.first->GetTransform()->setRotation(glm::toQuat(c2.orientation)); //works incorrectly!
      cube2.first->GetTransform()->setTranslation(c2.origin - (cube2.first->GetTransform()->getRotation() * cube2.first->GetCollider()->GetCenter()));

      sphere2.first->GetTransform()->setTranslation(s2.position - sphere2.first->GetCollider()->GetCenter());

      //grassPlane.first->GetTransform()->setTranslation(grass_obb.origin - grassPlane.first->GetCollider()->GetCenter());
    }
  }

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
  m_physics_system = std::make_unique<dbb::PhysicsSystem>();
  m_physics_system->CollisionOccured.Subscribe([this](const dbb::CollisionPair& _c) { OnCollisionOccured(_c); });

  static math::eClock s_clock;
  s_clock.start();

  m_timer.reset(new math::Timer([this]()->bool
    {
      float tick = s_clock.newFrame();
      if (m_simulation_on)
      {
        m_physics_system->SetLinearProjectionPercent(m_projection_percent);
        m_physics_system->SetPenetrationSlack(m_penetration_slack);
        m_physics_system->SetImpulseIteration(m_impulse_iterations);
        m_physics_system->SetLinearImpulsesOnly(m_linear_impulses_only);
        m_physics_system->UpdateAsync(tick);
      }
      return true;
    }));
  m_timer->start(33); //~30 fps

  ObjectFactoryBase factory;
  m_normal_mesh = new LineMesh({}, {}, glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f });
  m_game->AddObject(factory.CreateObject(std::make_shared<SimpleModel>(m_normal_mesh), eObject::RenderType::LINES, "Normal mesh"));

  // sphere 1
  {
    sphere1.first = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR1");
    sphere1.first->GetTransform()->setTranslation(glm::vec3(-3.0f, 3.0f, 0.0f));
    m_game->AddObject(sphere1.first);

    dbb::sphere sph1 = sphere1.first->GetCollider()->GetSphere(*sphere1.first->GetTransform()).value();
    dbb::ICollider* c_sph1 = new dbb::SphereCollider(sph1);
    sphere1.second = std::make_shared<dbb::RigidBody>(c_sph1);
    sphere1.second->SetCoefOfRestitution(m_restitution);
  }

  //cube 1
  {
    cube1.first = factory.CreateObject(m_game->GetModelManager()->Find("Chair"), eObject::RenderType::PBR, "Cube1");
    cube1.first->GetTransform()->setTranslation(glm::vec3(-3.0f, 5.0f, 0.0f));
    m_game->AddObject(cube1.first);

    dbb::OBB obb1 = cube1.first->GetCollider()->GetOBB(*cube1.first->GetTransform()).value(); // get obb from old collider!
    dbb::ICollider* c_obb1 = new dbb::OBBCollider(obb1);
    cube1.second = std::make_shared<dbb::RigidBody>(c_obb1);
    cube1.second->SetCoefOfRestitution(m_restitution);

    m_imgui->Add(SLIDER_FLOAT_NERROW, "Damping Chair", (void*)&cube1.second->GetDamping());
    m_imgui->Add(SLIDER_FLOAT_NERROW, "Angular Velocity Damper", (void*)&cube1.second->g_angular_vel_damper);
  }

  //sphere 2
  {
    sphere2.first = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR2");
    sphere2.first->GetTransform()->setTranslation(glm::vec3(-5.5f, 3.0f, 0.0f));
    m_game->AddObject(sphere2.first);

    dbb::sphere sph2 = sphere2.first->GetCollider()->GetSphere(*sphere2.first->GetTransform()).value();
    dbb::ICollider* c_sph2 =new dbb::SphereCollider(sph2);
    sphere2.second = std::make_shared<dbb::RigidBody>(c_sph2);
  }

  //cube 2
  {
    cube2.first = factory.CreateObject(m_game->GetModelManager()->Find("wall_cube"), eObject::RenderType::PHONG, "Cube2");
    cube2.first->GetTransform()->setTranslation(glm::vec3(3.0f, 3.0f, 0.0f));
    m_game->AddObject(cube2.first);

    dbb::OBB obb2 = cube2.first->GetCollider()->GetOBB(*cube2.first->GetTransform()).value();
    dbb::ICollider* c_obb2 = new dbb::OBBCollider(obb2);
    cube2.second = std::make_shared<dbb::RigidBody>(c_obb2);
  }

  // plane
  {
    grassPlane.first = factory.CreateObject(m_game->GetModelManager()->Find("wall_cube"), eObject::RenderType::PHONG, "GrassPlane");
    grassPlane.first->GetTransform()->setTranslation(glm::vec3(0.0f, -2.0f, 0.0f));
    grassPlane.first->GetTransform()->setScale(glm::vec3(10.0f, 0.5f, 10.0f));
    m_game->AddObject(grassPlane.first);

    dbb::OBB grass_obb = grassPlane.first->GetCollider()->GetOBB(*grassPlane.first->GetTransform()).value();
    dbb::ICollider* c_grass = new dbb::OBBCollider(grass_obb);
    grassPlane.second = std::make_shared<dbb::RigidBody>(c_grass);
    grassPlane.second->SetMass(m_terrain_mass);
    grassPlane.second->SetCoefOfRestitution(m_restitution);
    grassPlane.second->SetGravityApplicable(false);
  }

  if (m_add_cube1)
    m_physics_system->AddRigidbody(cube1.second);
  if (m_add_sphere)
    m_physics_system->AddRigidbody(sphere1.second);
  if (m_add_cube2)
    m_physics_system->AddRigidbody(cube2.second);
  m_physics_system->AddRigidbody(grassPlane.second);
  //m_physics_system->AddConstraint(grassPlane.first->GetCollider()->GetOBB(*grassPlane.first->GetTransform()).value());
}

//------------------------------------------------------------------------------
void PhysicsEngineTestScript::ClearVisulaDebugInfo()
{
  for (auto& obj : m_game->GetObjects())
  {
    if (obj->Name().find("OBB vertex") != std::string::npos)
      m_game->DeleteObject(obj);
    m_normal_mesh->UpdateData({}, {}, { 1.0f, 1.0f ,0.0f, 1.0f });
  }
}

//------------------------------------------------------------------------------
void PhysicsEngineTestScript::Reset()
{
  m_simulation_on = false;
  m_physics_system->ClearRigidbodys();
  ClearVisulaDebugInfo();

  sphere1.first->GetTransform()->setTranslation(glm::vec3(-3.0f, 3.0f, 0.0f));
  sphere1.first->GetTransform()->setRotation(0,0,0);
  dbb::sphere sph1 = sphere1.first->GetCollider()->GetSphere(*sphere1.first->GetTransform()).value();
  dbb::ICollider* c_sph1 = new dbb::SphereCollider(sph1);
  sphere1.second = std::make_shared<dbb::RigidBody>(c_sph1);
  sphere1.second->SetCoefOfRestitution(m_restitution);
  sphere1.second->SetFriction(m_friction);

  sphere2.first->GetTransform()->setTranslation(glm::vec3(-5.5f, 3.0f, 0.0f));
  sphere2.first->GetTransform()->setRotation(0, 0, 0);
  dbb::sphere sph2 = sphere2.first->GetCollider()->GetSphere(*sphere2.first->GetTransform()).value();
  dbb::ICollider* c_sph2 = new dbb::SphereCollider(sph2);
  sphere2.second = std::make_shared<dbb::RigidBody>(c_sph2);
  sphere2.second->SetCoefOfRestitution(m_restitution);
  sphere2.second->SetFriction(m_friction);

  cube1.first->GetTransform()->setTranslation(glm::vec3(-3.0f, 5.0f, 0.0f));
  cube1.first->GetTransform()->setRotation(0,0,0);
  dbb::OBB obb1 = cube1.first->GetCollider()->GetOBB(*cube1.first->GetTransform()).value(); // get obb from old collider!
  dbb::ICollider* c_obb1 = new dbb::OBBCollider(obb1); // incorrect rotations
  cube1.second = std::make_shared<dbb::RigidBody>(c_obb1);
  cube1.second->SetCoefOfRestitution(m_restitution);
  cube1.second->SetFriction(m_friction);
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Damping Chair", (void*)&cube1.second->GetDamping());
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Angular Velocity Damper", (void*)&cube1.second->g_angular_vel_damper);

  cube2.first->GetTransform()->setTranslation(glm::vec3(3.0f, 3.0f, 0.0f));
  cube2.first->GetTransform()->setRotation(0, 0, 0);
  dbb::OBB obb2 = cube2.first->GetCollider()->GetOBB(*cube2.first->GetTransform()).value(); // get obb from old collider!
  dbb::ICollider* c_obb2 = new dbb::OBBCollider(obb2); // incorrect rotations
  cube2.second = std::make_shared<dbb::RigidBody>(c_obb2);
  cube2.second->SetCoefOfRestitution(m_restitution);
  cube2.second->SetFriction(m_friction);

  dbb::OBB grass_obb = grassPlane.first->GetCollider()->GetOBB(*grassPlane.first->GetTransform()).value();
  dbb::ICollider* c_grass = new dbb::OBBCollider(grass_obb);
  grassPlane.second = std::make_shared<dbb::RigidBody>(c_grass);
  grassPlane.second->SetMass(m_terrain_mass);
  grassPlane.second->SetCoefOfRestitution(m_restitution);
  grassPlane.second->SetGravityApplicable(false);

  if (m_add_cube1)
    m_physics_system->AddRigidbody(cube1.second);
  if(m_add_sphere)
   m_physics_system->AddRigidbody(sphere1.second);
  if (m_add_cube2)
    m_physics_system->AddRigidbody(cube2.second);

  m_physics_system->AddRigidbody(sphere2.second);
  m_physics_system->AddRigidbody(grassPlane.second);
  //m_physics_system->AddConstraint(grassPlane.first->GetCollider()->GetOBB(*grassPlane.first->GetTransform()).value());
  m_reset = false;
}

//-------------------------------------------------------------------------------------------
void PhysicsEngineTestScript::OnCollisionOccured(const dbb::CollisionPair& _collision_feedback)
{
  ClearVisulaDebugInfo();
  //visualize collision
  ObjectFactoryBase factory;
  std::vector<dbb::point> linePoints;
  std::vector <unsigned int> lineIndexes;
  for (size_t i = 0; i < _collision_feedback.m_result.contacts.size(); ++i)
  {
    shObject obj = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "OBB vertex" + std::to_string(i));
    obj->GetTransform()->setTranslation(_collision_feedback.m_result.contacts[i]);
    obj->GetTransform()->setScale({ 0.05f,0.05f,0.05f });
    m_visual.push_back(obj);

   /* linePoints.push_back(_collision_feedback.m_result.contacts[i]);
    linePoints.push_back(_collision_feedback.m_result.contacts[i] + (-_collision_feedback.m_result.normal * (_collision_feedback.m_result.depth / 2)));
    lineIndexes.push_back(i * 2);
    lineIndexes.push_back(i * 2 + 1);*/
  }
  /*m_normal_mesh->UpdateData(linePoints, lineIndexes, { 1.0f, 1.0f ,0.0f, 1.0f });*/
}
