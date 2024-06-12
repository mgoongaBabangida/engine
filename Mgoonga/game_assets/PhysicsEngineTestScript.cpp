#include "stdafx.h"

#include "PhysicsEngineTestScript.h"
#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "ModelManagerYAML.h"

#include <math/Geometry.h>
#include <math/GeometryFunctions.h>
#include <math/RigidBody.h>
#include <math/PhysicsSystem.h>
#include <math/Colliders.h>

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
  m_imgui->Add(CHECKBOX, "Correct All Objects", (void*)&m_correct_all_objects);
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Coefitient of Restitution", &m_restitution);
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Friction", &m_friction);

  m_imgui->Add(SLIDER_FLOAT_NERROW, "Projection Percent", &m_projection_percent);

  static std::function<void(int, int*&)> slack_callback = [this](int _value, int*& _val) //@todo test default
  {
    static bool first_call = true;
    if (first_call)
    {
      *_val = int(m_penetration_slack * 100);
      first_call = false;
      return;
    }
    m_penetration_slack = float(_value) / 100.f;
  };
  m_imgui->Add(SPIN_BOX, "Penetration Slack", &slack_callback);
  
  static std::function<void(int, int*&)> impulse_iterations_callback = [this](int _value, int*& _val)
  {
    static bool first_call = true;
    if (first_call)
    {
      *_val = m_impulse_iterations;
      first_call = false;
      return;
    }
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
bool PhysicsEngineTestScript::OnKeyJustPressed(uint32_t _asci, KeyModifiers _modifier)
{
  switch (_asci)
  {
    case ASCII_J:
    {
      if (false)
      {
        //visualize obb
        ObjectFactoryBase factory;
        dbb::OBB obb1 = dynamic_cast<dbb::OBBCollider*>(cube1->GetRigidBody()->GetCollider())->GetBox();

        for (size_t i = 0; i < obb1.GetVertices().size(); ++i)
        {
          shObject obj = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "OBB vertex" + std::to_string(i));
          obj->GetTransform()->setTranslation(obb1.GetVertices()[i]);
          obj->GetTransform()->setScale({ 0.05f,0.05f,0.05f });
          m_visual.push_back(obj);
        }
      }

      auto manifold = dbb::RigidBody::FindCollisionFeatures(dynamic_cast<dbb::RigidBody&>(*cube1->GetRigidBody()), dynamic_cast<dbb::RigidBody&>(*sphere1->GetRigidBody()));
      manifold = dbb::RigidBody::FindCollisionFeatures(dynamic_cast<dbb::RigidBody&>(*sphere1->GetRigidBody()), dynamic_cast<dbb::RigidBody&>(*sphere2->GetRigidBody()));
      manifold = dbb::RigidBody::FindCollisionFeatures(dynamic_cast<dbb::RigidBody&>(*cube1->GetRigidBody()), dynamic_cast<dbb::RigidBody&>(*cube2->GetRigidBody()));
      manifold = dbb::RigidBody::FindCollisionFeatures(dynamic_cast<dbb::RigidBody&>(*grassPlane->GetRigidBody()), dynamic_cast<dbb::RigidBody&>(*cube1->GetRigidBody()));
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
      sphere2->GetTransform()->setTranslation(m_game->GetMainCameraPosition());
      sphere2->GetTransform()->setScale({ 0.1,0.1,0.1 });
      sphere2->GetRigidBody()->AddLinearImpulse(m_game->GetMainCameraDirection() * 25.f);

      ICollider* c_sph2 = dbb::SphereCollider::CreateFrom(dynamic_cast<dbb::SphereCollider&>(*sphere2->GetCollider()), *sphere2->GetTransform());
      sphere2->SetCollider(c_sph2);

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
      cube1->GetCollider()->SetFrom(*cube1->GetTransform());
      cube1->GetRigidBody()->SetCollider(cube1->GetCollider()); // update position and rotation of rigid body from collider
      cube2->GetCollider()->SetFrom(*cube2->GetTransform());
      cube2->GetRigidBody()->SetCollider(cube2->GetCollider());
      sphere1->GetCollider()->SetFrom(*sphere1->GetTransform());
      sphere1->GetRigidBody()->SetCollider(sphere1->GetCollider());
      sphere2->GetCollider()->SetFrom(*sphere2->GetTransform());
      sphere2->GetRigidBody()->SetCollider(sphere2->GetCollider());
    }

    dbb::OBB c1 = dynamic_cast<dbb::OBBCollider*>(cube1->GetRigidBody()->GetCollider())->GetBox();
    dbb::OBB grass_obb = dynamic_cast<dbb::OBBCollider*>(grassPlane->GetRigidBody()->GetCollider())->GetBox();

    m_normal_mesh->UpdateData(dbb::Unite(c1.GetVertices(), grass_obb.GetVertices()),
      { 6,1, 6,3, 6,4, 2,7, 2,5, 2,0, 0,1, 0,3, 7,1, 7,4, 4,5, 5,3,
        6+8, 1 + 8, 6 + 8,3 + 8, 6 + 8,4 + 8, 2 + 8,7 + 8, 2 + 8,5 + 8, 2 + 8,0 + 8, 0 + 8,1 + 8, 0 + 8,3 + 8, 7 + 8,1 + 8, 7 + 8,4 + 8, 4 + 8,5 + 8, 5 + 8,3 + 8 },
      { 1.0f, 1.0f ,0.0f, 1.0f });

    //update transforms with the collider data to visualize correctily //@todo incorrect rotations for now -:(
    if (m_simulation_on)
    {
      cube1->GetRigidBody()->GetCollider()->SetTo(*cube1->GetTransform());
      cube2->GetRigidBody()->GetCollider()->SetTo(*cube2->GetTransform());
      sphere1->GetRigidBody()->GetCollider()->SetTo(*sphere1->GetTransform());
      sphere2->GetRigidBody()->GetCollider()->SetTo(*sphere2->GetTransform());
      //grassPlane->GetTransform()->setTranslation(grass_obb.origin - grassPlane->GetCollider()->GetCenter());
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
        m_physics_system->SetCorrectAllObjects(m_correct_all_objects);
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
    sphere1 = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR1");
    sphere1->GetTransform()->setTranslation(glm::vec3(-3.0f, 3.0f, 0.0f));
    m_game->AddObject(sphere1);

    ICollider* c_sph1 = dbb::SphereCollider::CreateFrom(dynamic_cast<BoxCollider&>(*sphere1->GetCollider()), *sphere1->GetTransform());
    sphere1->SetCollider(c_sph1);
    sphere1->GetRigidBody()->SetCoefOfRestitution(m_restitution);
  }

  //cube 1
  {
    cube1 = factory.CreateObject(m_game->GetModelManager()->Find("Chair"), eObject::RenderType::PBR, "Cube1");
    cube1->GetTransform()->setTranslation(glm::vec3(-3.0f, 5.0f, 0.0f));
    m_game->AddObject(cube1);

    ICollider* c_obb1 = dbb::OBBCollider::CreateFrom(dynamic_cast<BoxCollider&>(*cube1->GetCollider()), *cube1->GetTransform());
    cube1->SetCollider(c_obb1);
    cube1->GetRigidBody()->SetCoefOfRestitution(m_restitution);

    m_imgui->Add(SLIDER_FLOAT_NERROW, "Damping Chair", (void*)&cube1->GetRigidBody()->GetDamping());
  }

  //sphere 2
  {
    sphere2 = factory.CreateObject(m_game->GetModelManager()->Find("sphere_red"), eObject::RenderType::PBR, "SpherePBR2");
    sphere2->GetTransform()->setTranslation(glm::vec3(-5.5f, 3.0f, 0.0f));
    m_game->AddObject(sphere2);

    ICollider* c_sph2 = dbb::SphereCollider::CreateFrom(dynamic_cast<BoxCollider&>(*sphere2->GetCollider()), *sphere2->GetTransform());
    sphere2->SetCollider(c_sph2);
  }

  //cube 2
  {
    cube2 = factory.CreateObject(m_game->GetModelManager()->Find("wall_cube"), eObject::RenderType::PHONG, "Cube2");
    cube2->GetTransform()->setTranslation(glm::vec3(3.0f, 3.0f, 0.0f));
    m_game->AddObject(cube2);

    ICollider* c_obb2 = dbb::OBBCollider::CreateFrom(dynamic_cast<BoxCollider&>(*cube2->GetCollider()), *cube2->GetTransform());
    cube2->SetCollider(c_obb2);
  }

  // plane
  {
    grassPlane = factory.CreateObject(m_game->GetModelManager()->Find("wall_cube"), eObject::RenderType::PHONG, "GrassPlane");
    grassPlane->GetTransform()->setTranslation(glm::vec3(0.0f, -2.0f, 0.0f));
    grassPlane->GetTransform()->setScale(glm::vec3(10.0f, 0.5f, 10.0f));
    m_game->AddObject(grassPlane);

    ICollider* c_grass = dbb::OBBCollider::CreateFrom(dynamic_cast<BoxCollider&>(*grassPlane->GetCollider()), *grassPlane->GetTransform());
    grassPlane->SetCollider(c_grass);
    grassPlane->GetRigidBody()->SetMass(m_terrain_mass);
    grassPlane->GetRigidBody()->SetCoefOfRestitution(m_restitution);
    grassPlane->GetRigidBody()->SetGravityApplicable(false);
  }

  if (m_add_cube1)
    m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(cube1->GetRigidBody()));
  if (m_add_sphere)
    m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(sphere1->GetRigidBody()));
  if (m_add_cube2)
    m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(cube2->GetRigidBody()));
  m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(sphere2->GetRigidBody()));
  m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(grassPlane->GetRigidBody()));
  //m_physics_system->AddConstraint(grassPlane->GetCollider()->GetOBB(*grassPlane->GetTransform()).value());
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

  sphere1->GetTransform()->setTranslation(glm::vec3(-3.0f, 3.0f, 0.0f));
  sphere1->GetTransform()->setRotation(0,0,0);
 /* ICollider* c_sph1 = dbb::SphereCollider::CreateFrom(dynamic_cast<BoxCollider&>(*sphere1->GetCollider()), *sphere1->GetTransform());
  sphere1->SetCollider(c_sph1);*/
  sphere1->GetCollider()->SetFrom(*sphere1->GetTransform());
  sphere1->GetRigidBody()->SetCoefOfRestitution(m_restitution);
  sphere1->GetRigidBody()->SetFriction(m_friction);

  sphere2->GetTransform()->setTranslation(glm::vec3(-5.5f, 3.0f, 0.0f));
  sphere2->GetTransform()->setRotation(0, 0, 0);
  /*ICollider* c_sph2 = dbb::SphereCollider::CreateFrom(dynamic_cast<BoxCollider&>(*sphere2->GetCollider()), *sphere2->GetTransform());
  sphere2->SetCollider(c_sph2);*/
  sphere2->GetCollider()->SetFrom(*sphere2->GetTransform());
  sphere2->GetRigidBody()->SetCoefOfRestitution(m_restitution);
  sphere2->GetRigidBody()->SetFriction(m_friction);

  cube1->GetTransform()->setTranslation(glm::vec3(-3.0f, 5.0f, 0.0f));
  cube1->GetTransform()->setRotation(0,0,0);
  // get obb from old collider!
  //ICollider* c_obb1 = dbb::OBBCollider::CreateFrom(dynamic_cast<BoxCollider&>(*cube1->GetCollider()), *cube1->GetTransform()); // incorrect rotations
  //cube1->SetCollider(c_obb1);
  cube1->GetCollider()->SetFrom(*cube1->GetTransform());
  cube1->GetRigidBody()->SetCoefOfRestitution(m_restitution);
  cube1->GetRigidBody()->SetFriction(m_friction);
  m_imgui->Add(SLIDER_FLOAT_NERROW, "Damping Chair", (void*)&cube1->GetRigidBody()->GetDamping());

  cube2->GetTransform()->setTranslation(glm::vec3(3.0f, 3.0f, 0.0f));
  cube2->GetTransform()->setRotation(0, 0, 0);
  // get obb from old collider!
  //ICollider* c_obb2 = dbb::OBBCollider::CreateFrom(dynamic_cast<BoxCollider&>(*cube2->GetCollider()), *cube2->GetTransform()); // incorrect rotations
  //cube2->SetCollider(c_obb2);
  cube2->GetCollider()->SetFrom(*cube2->GetTransform());
  cube2->GetRigidBody()->SetCoefOfRestitution(m_restitution);
  cube2->GetRigidBody()->SetFriction(m_friction);

  ICollider* c_grass = dbb::OBBCollider::CreateFrom(dynamic_cast<BoxCollider&>(*grassPlane->GetCollider()), *grassPlane->GetTransform());
  grassPlane->SetCollider(c_grass);
  grassPlane->GetRigidBody()->SetMass(m_terrain_mass);
  grassPlane->GetRigidBody()->SetCoefOfRestitution(m_restitution);
  grassPlane->GetRigidBody()->SetGravityApplicable(false);

  if (m_add_cube1)
    m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(cube1->GetRigidBody()));
  if (m_add_sphere)
    m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(sphere1->GetRigidBody()));
  if (m_add_cube2)
    m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(cube2->GetRigidBody()));
  m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(sphere2->GetRigidBody()));
  m_physics_system->AddRigidbody(std::dynamic_pointer_cast<dbb::RigidBody>(grassPlane->GetRigidBody()));
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
