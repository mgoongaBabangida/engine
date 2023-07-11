#include "stdafx.h"
#include "MgoongaGame.h"

#include <base/InputController.h>

#include <math/Rigger.h>
#include <math/RigidBdy.h>
#include <math/BoxCollider.h>

#include <opengl_assets/Texture.h>
#include <opengl_assets/RenderManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <game_assets/ModelManagerYAML.h>
#include <game_assets/AnimationManagerYAML.h>
#include <game_assets/CameraFreeController.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ShipScript.h>
#include <game_assets/ObjectFactory.h>
#include <game_assets/BezierCurveUIController.h>

//---------------------------------------------------------------------------
eMgoongaGameContext::eMgoongaGameContext(eInputController*  _input,
                                         std::vector<IWindowImGui*> _externalGui,
						                             const std::string& _modelsPath,
						                             const std::string& _assetsPath, 
						                             const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
{
  FocusChanged.Subscribe([this](shObject _prev, shObject _new)->void { this->OnFocusedChanged(); });
  
  if (!m_objects.empty())
  {
    m_focused = m_objects[0];
    FocusChanged.Occur(shObject{}, m_focused);
  }
}

//--------------------------------------------------------------------------
eMgoongaGameContext::~eMgoongaGameContext() {}

//--------------------------------------------------------------------------
void eMgoongaGameContext::InitializeExternalGui()
{
  eMainContextBase::InitializeExternalGui();
}

//*********************InputObserver*********************************
//--------------------------------------------------------------------------
bool eMgoongaGameContext::OnKeyPress(uint32_t asci)
{
  if (eMainContextBase::OnKeyPress(asci))
    return true;

  switch (asci)
  {
     case ASCII_J: { if (m_focused)	m_focused->GetRigidBody()->MoveLeft(m_objects); }				return true;
     case ASCII_L: { if (m_focused)	m_focused->GetRigidBody()->MoveRight(m_objects); }				return true;
     case ASCII_K: { if (m_focused)	m_focused->GetRigidBody()->MoveBack(m_objects); }				return true;
     case ASCII_I: { if (m_focused)	m_focused->GetRigidBody()->MoveForward(m_objects); }				return true;
     case ASCII_Z: { if (m_focused)	m_focused->GetRigidBody()->MoveUp(m_objects); }					return true;
     case ASCII_X: { if (m_focused)	m_focused->GetRigidBody()->MoveDown(m_objects); }				return true;
     case ASCII_C: { if (m_focused)	m_focused->GetRigidBody()->TurnRight(m_objects); }				return true;
     case ASCII_V: { if (m_focused)	m_focused->GetRigidBody()->TurnLeft(m_objects); }				return true;
     case ASCII_B: { if (m_focused)	m_focused->GetRigidBody()->LeanRight(m_objects); }				return true;
     case ASCII_N: { if (m_focused)	m_focused->GetRigidBody()->LeanLeft(m_objects); }				return true;
     case ASCII_U: { if (m_focused)	m_focused->GetRigidBody()->LeanForward(m_objects); }				return true;
     case ASCII_H: { if (m_focused)	m_focused->GetRigidBody()->LeanBack(m_objects); }				return true;
     case 27: {} return false; //ESC @todo
     //case ASCII_G:	{ if (m_focused)	m_focused->GetScript()->OnKeyPress(ASCII_G);}	return true;
     default: return false;
  }
}

//--------------------------------------------------------------------------
bool eMgoongaGameContext::OnMouseMove(int32_t x, int32_t y)
{
  if (GetMainCamera().getCameraRay().IsPressed())
  {
    if(m_input_strategy && !m_input_strategy->OnMouseMove(x,y))
    {
      // input strategy has priority over frame, @todo frmae should be inside one of input strategies
      m_framed.reset(new std::vector<shObject>(GetMainCamera().getCameraRay().onMove(GetMainCamera(), m_objects, static_cast<float>(x), static_cast<float>(y)))); 	//to draw a frame
      return true;
    }
  }
	return false;
}

//--------------------------------------------------------------------------
bool eMgoongaGameContext::OnMousePress(int32_t x, int32_t y, bool left)
{
  if (m_framed)
    m_framed->clear();

  GetMainCamera().getCameraRay().Update(GetMainCamera(), static_cast<float>(x), static_cast<float>(y), width, height);
  GetMainCamera().getCameraRay().press(x, y);
  //should be inside input strategy which needs it(frame, moveXZ)
  GetMainCamera().MovementSpeedRef() = 0.f;

  auto [new_focused, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(m_objects);

  if (new_focused != m_focused)
  {
    FocusChanged.Occur(m_focused, new_focused);
    m_focused = new_focused;
  }

  if(m_input_strategy)
    m_input_strategy->OnMousePress(x, y, left);

  // should be inside script, not here
	if(m_focused && m_focused->GetScript())
		m_focused->GetScript()->OnMousePress(x, y, left);

	return true;
}

//---------------------------------------------------------------------------------
bool eMgoongaGameContext::OnMouseRelease()
{
  GetMainCamera().getCameraRay().release();
  if(m_input_strategy)
    m_input_strategy->OnMouseRelease();
  //should be inside input strategy which needs it(frame, moveXZ)
  GetMainCamera().MovementSpeedRef() = 0.05f;
	return true;
}

//------------------------------------------------------------------------------
void eMgoongaGameContext::OnFocusedChanged()
{
}

//*********************Initialize**************************************
//-------------------------------------------------------------------------------
void eMgoongaGameContext::InitializeSounds()
{
	//sound->loadListner(GetMainCamera().getPosition().x, GetMainCamera().getPosition().y, GetMainCamera().getPosition().z); //!!!
}

//-------------------------------------------------------------------------------
void eMgoongaGameContext::InitializePipline()
{
	pipeline.Initialize();
	// call all the enable pipeline functions
}

//-----------------------------------------------------------------------------
void eMgoongaGameContext::InitializeBuffers()
{
  GetMainLight().type = eLightType::DIRECTION;
	pipeline.InitializeBuffers(GetMainLight().type == eLightType::POINT);
}

//-----------------------------------------------------------------------------
void eMgoongaGameContext::InitializeModels()
{
	eMainContextBase::InitializeModels();

	//MODELS
	modelManager->Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());
	modelManager->Add("boat", (GLchar*)std::string(modelFolderPath + "Medieval Boat/Medieval Boat.obj").c_str());
	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager->Add("guard", (GLchar*)std::string(modelFolderPath + "ogldev-master/Content/guard/boblampclean.md5mesh").c_str(), true);
  modelManager->Add("zombie", (GLchar*)std::string(modelFolderPath + "Thriller Part 3/Thriller Part 3.dae").c_str());
  //modelManager->Add("vampire", (GLchar*)std::string(modelFolderPath + "vampire/dancing_vampire.dae").c_str());

  modelManager->Add("Dying", (GLchar*)std::string(modelFolderPath + "Dying Soldier/Dying.dae").c_str());

  Material pbr1;
  pbr1.albedo_texture_id = texManager->Find("pbr1_basecolor")->id;
  pbr1.metalic_texture_id = texManager->Find("pbr1_metallic")->id;
  pbr1.normal_texture_id = texManager->Find("pbr1_normal")->id;
  pbr1.roughness_texture_id = texManager->Find("pbr1_roughness")->id;
  pbr1.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;
  pbr1.use_albedo = pbr1.use_metalic = pbr1.use_normal = pbr1.use_roughness = true;

  Material red;
  red.albedo = glm::vec3(0.9f, 0.0f, 0.0f);
  red.ao = 1.0f;
  red.roughness = 0.5;
  red.metallic = 0.5;

  Material gold;
  gold.albedo_texture_id = texManager->Find("pbr_gold_basecolor")->id;
  gold.metalic_texture_id = texManager->Find("pbr_gold_metallic")->id;
  gold.normal_texture_id = texManager->Find("pbr_gold_normal")->id;
  gold.roughness_texture_id = texManager->Find("pbr_gold_roughness")->id;
  gold.emissive_texture_id = Texture::GetTexture1x1(BLACK).id;
  gold.use_albedo = gold.use_metalic = gold.use_normal = gold.use_roughness = true;

  modelManager->Add("sphere_textured", Primitive::SPHERE, std::move(pbr1));
  modelManager->Add("sphere_red", Primitive::SPHERE, std::move(red));
  modelManager->Add("sphere_gold", Primitive::SPHERE, std::move(gold));

  //@todo separate init scene member func
  _InitMainTestSceane();
  _InitializeHexes();
  _InitializeBezier();

  m_input_strategy.reset(new InputStrategyMoveAlongXZPlane(GetMainCamera(), GetObjectsWithChildren(m_objects)));

  const Texture* tex = texManager->Find("TButton_red");
  const Texture* flag = texManager->Find("TSpanishFlag0_s");
  glm::ivec2 topLeft{ 160, 450 };
  glm::ivec2 bottomRight{ 1030, 725 };

  m_guis.emplace_back(new GUIWithAlpha(0, 0, (bottomRight.x - topLeft.x) / 4, (bottomRight.y - topLeft.y) / 4, width, height));
  m_guis[0]->SetTexture(*tex, topLeft, bottomRight);
  m_guis[0]->SetTransparent(true);
  m_guis[0]->SetChild(std::make_shared<GUIWithAlpha>(0, (bottomRight.y - topLeft.y) / 4, (bottomRight.x - topLeft.x) / 4, (bottomRight.y - topLeft.y) / 4, width, height));
  m_guis[0]->GetChildren()[0]->SetTexture(*tex, topLeft, bottomRight);
  m_guis[0]->GetChildren()[0]->SetVisible(false);
  m_guis[0]->setCommand(std::make_shared<MenuBehaviorLeanerMove>(m_guis[0]->GetChildren()[0].get(),
    math::AnimationLeaner{
      {glm::vec3(m_guis[0]->getTopLeft().x, m_guis[0]->getTopLeft().y, 0)},
      {glm::vec3(m_guis[0]->GetChildren()[0]->getTopLeft().x, m_guis[0]->GetChildren()[0]->getTopLeft().y, 0)},
      1000 }));

 /* m_guis.emplace_back(new Cursor(0, 0, (bottomRight.x - topLeft.x) / 4, (bottomRight.y - topLeft.y) / 4, width, height));
  m_guis[1]->SetTexture(*tex, topLeft, bottomRight);*/
  const Texture* cursor = texManager->Find("cursor1");
  m_guis.emplace_back(new Cursor(0, 0, 30, 30, width, height));
  m_guis[1]->SetTexture(*cursor, { 0,0 }, { cursor->mTextureWidth, cursor->mTextureHeight });
  m_guis.emplace_back(new Movable2D(400, 0, 60, 60, width, height));
  m_guis[2]->SetTexture(*flag, { 0,0 }, { flag->mTextureWidth, flag->mTextureHeight });

  m_input_controller->AddObserver(this, WEAK);
  m_input_controller->AddObserver(m_guis[0].get(), MONOPOLY);//monopoly takes only mouse
  m_input_controller->AddObserver(m_guis[1].get(), WEAK);
  m_input_controller->AddObserver(m_guis[2].get(), STRONG);

  //GLOBAL CONTROLLERS
  m_global_scripts.push_back(std::make_shared<CameraFreeController>(GetMainCamera()));

  m_input_controller->AddObserver(&*m_global_scripts.back(), WEAK);
}

//-------------------------------------------------------------------------
void eMgoongaGameContext::InitializeRenders()
{
  eMainContextBase::InitializeRenders();
	/*pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f),
                                                                   texManager->Find("Tatlas2"),
                                                                   soundManager->GetSound("shot_sound"),
                                                                   texManager->Find("Tatlas2")->numberofRows));*/
}

//-------------------------------------------------------------------------------
void eMgoongaGameContext::PaintGL()
{
	eMainContextBase::PaintGL();
}

//-------------------------------------------------------------------------------------------
void eMgoongaGameContext::_InitializeHexes()
{
  float common_height = 2.01f;
  float radius = 0.5f;
  std::vector<glm::vec3> m_dots;
  float z_move = glm::sin(glm::radians(240.0f)) * radius;
  for (int i = -6; i < 6; ++i)
    for (int j = -5; j < 5; ++j)
    {
      m_dots.emplace_back(glm::vec3{ glm::cos(glm::radians(0.0f)) * radius * i,
                                   common_height,
                                   z_move * 2 * j });
      m_dots.emplace_back(glm::vec3{ glm::cos(glm::radians(60.0f)) * radius * i * 2 + radius / 2,
                                   common_height, 
                                   z_move + z_move * 2 * j });
    }
  ObjectFactoryBase factory;
  m_objects.push_back(factory.CreateObject(std::make_shared<SimpleModel>(new SimpleGeometryMesh(m_dots, 0.5f * 0.57f, SimpleGeometryMesh::GeometryType::Hex, { 1.0f, 1.0f, 0.0f })),
    eObject::RenderType::GEOMETRY));
}

//----------------------------------------------------
void eMgoongaGameContext::_InitializeBezier()
{
  dbb::Bezier bezier;
  bezier.p0 = { 1.0f, 3.0f, 0.0f };
  bezier.p1 = { 3.0f, 3.0f, 3.0f };
  bezier.p2 = { 4.2f, 3.0f, -2.5f };
  bezier.p3 = { 8.0f, 3.0f, 1.0f };
  ObjectFactoryBase factory;
  shObject bezier_model = factory.CreateObject(std::make_shared<BezierCurveModel>(new BezierCurveMesh(bezier)), eObject::RenderType::BEZIER_CURVE);
  m_objects.push_back(bezier_model);

  for (int i = 0 ; i < 4; ++i)
  {
    shObject pbr_sphere = factory.CreateObject(modelManager->Find("sphere_red"), eObject::RenderType::PBR, "SphereBezierPBR " + std::to_string(i));
    bezier_model->GetChildrenObjects().push_back(pbr_sphere);
  }
  bezier_model->SetScript(new BezierCurveUIController(bezier_model));
}

//----------------------------------------------------
void eMgoongaGameContext::_InitMainTestSceane()
{
  //TERRAIN
  std::unique_ptr<TerrainModel> terrainModel = modelManager->CloneTerrain("simple");
  terrainModel->initialize(texManager->Find("Tgrass0_d"),
                           texManager->Find("Tgrass0_d"),
                           &Texture::GetTexture1x1(BLUE),
                           texManager->Find("TOcean0_s"),
                           false);

  //OBJECTS
  ObjectFactoryBase factory;

  shObject wallCube = factory.CreateObject(modelManager->Find("wall_cube"), eObject::RenderType::PHONG, "WallCube");
  wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
  m_objects.push_back(wallCube);

  shObject containerCube = factory.CreateObject(modelManager->Find("container_cube"), eObject::RenderType::PHONG, "ContainerCube");
  containerCube->GetTransform()->setTranslation(vec3(-2.5f, 3.0f, 3.5f));
  m_objects.push_back(containerCube);

  shObject grassPlane = factory.CreateObject(modelManager->Find("grass_plane"), eObject::RenderType::PHONG, "GrassPlane");
  grassPlane->GetTransform()->setTranslation(vec3(0.0f, 1.2f, 0.0f));
  m_objects.push_back(grassPlane);

  shObject terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()), eObject::RenderType::PHONG, "Terrain");
  terrain->SetName("Terrain");
  terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
  terrain->GetTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
  m_objects.push_back(terrain);

  shObject nanosuit = factory.CreateObject(modelManager->Find("nanosuit"), eObject::RenderType::PHONG, "Nanosuit");
  nanosuit->GetTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
  nanosuit->GetTransform()->setRotation(0.0f, glm::radians(180.0f), 0.0f);
  nanosuit->GetTransform()->setScale(vec3(0.12f, 0.12f, 0.12f));
  m_objects.push_back(nanosuit);

  nanosuit->SetScript(new eShipScript(this,
                                      texManager->Find("TSpanishFlag0_s"),
                                      pipeline,
                                      GetMainCamera(),
                                      texManager->Find("Tatlas2"),
                                      soundManager->GetSound("shot_sound"),
                                      pipeline.GetWaterHeight()));

  shObject wolf = factory.CreateObject(modelManager->Find("wolf"), eObject::RenderType::PHONG, "Wolf");
  wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
  wolf->GetTransform()->setTranslation(vec3(4.0f, 3.0f, 0.0f));
  wolf->SetRigger(new Rigger((Model*)modelManager->Find("wolf").get())); //@todo improve
  wolf->GetRigger()->ChangeName(std::string(), "Running");//@todo improve
  m_objects.push_back(wolf);

  shObject brickCube = factory.CreateObject(modelManager->Find("brick_cube"), eObject::RenderType::PHONG, "BrickCube");
  brickCube->SetModel(modelManager->Find("brick_cube"));
  brickCube->GetTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
  m_objects.push_back(brickCube);

  shObject guard = factory.CreateObject(modelManager->Find("guard"), eObject::RenderType::PHONG, "Guard");
  guard->GetTransform()->setTranslation(vec3(2.0f, 2.0f, 0.0f));
  guard->GetTransform()->setRotation(glm::radians(-90.0f), glm::radians(-90.0f), 0.0f);
  guard->GetTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
  guard->GetTransform()->setUp(glm::vec3(0.0f, 0.0f, 1.0f));
  guard->GetTransform()->setForward(glm::vec3(0.0f, 1.0f, 0.0f));
  guard->SetRigger(new Rigger((Model*)modelManager->Find("guard").get()));//@todo improve
  m_objects.push_back(guard);

  //shObject vampire = factory.CreateObject(modelManager->Find("vampire"), "Vampire");
  //vampire->GetTransform()->setTranslation(vec3(1.0f, 2.0f, 0.0f));
  //vampire->GetTransform()->setRotation(0.0f, glm::radians(180.0f), 0.0f);
  //vampire->GetTransform()->setScale(glm::vec3(0.01f, 0.01f, 0.01f));
  //vampire->SetRigger(new Rigger((Model*)modelManager->Find("vampire").get()));//@todo improve
  //m_objects.push_back(vampire);

  //light
  m_light_object = factory.CreateObject(modelManager->Find("white_sphere"), eObject::RenderType::PHONG, "WhiteSphere");
  m_light_object->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
  m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
  m_objects.push_back(m_light_object);

  shObject pbrCube = factory.CreateObject(modelManager->Find("pbr_cube"), eObject::RenderType::PBR, "CubePBR");
  pbrCube->GetTransform()->setTranslation(vec3(-4.5f, 3.5f, 1.5f));
  m_objects.push_back(pbrCube);

  shObject obj = factory.CreateObject(modelManager->Find("sphere_textured"), eObject::RenderType::PBR, "SpherePBR");
  obj->GetTransform()->setTranslation(glm::vec3(-2.0f, 3.5f, 1.5f));
  m_objects.push_back(obj);

  shObject goldsphere = factory.CreateObject(modelManager->Find("sphere_gold"), eObject::RenderType::PBR, "SpherePBRGold");
  goldsphere->GetTransform()->setTranslation(vec3(-7.0f, 3.5f, 2.0f));
  m_objects.push_back(goldsphere);

  shObject dying = factory.CreateObject(modelManager->Find("Dying"), eObject::RenderType::PHONG, "Dying");
  dying->GetTransform()->setTranslation(vec3(1.0f, 2.0f, -2.0f));
  dying->GetTransform()->setScale(vec3(0.01f, 0.01f, 0.01f));
  dying->SetRigger(new Rigger((Model*)modelManager->Find("Dying").get())); //@todo improve
  dying->GetRigger()->ChangeName(std::string(), "Dying");//@todo improve
  m_objects.push_back(dying);
}
