#include "stdafx.h"
#include "MgoongaGame.h"

#include <base/InputController.h>

#include <math/ParticleSystem.h>
#include <math/ShootingParticleSystem.h>
#include <math/Rigger.h>
#include <math/RigidBdy.h>
#include <math/BoxCollider.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/Texture.h>
#include <opengl_assets/RenderManager.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ShipScript.h>
#include <game_assets/ObjectFactory.h>

//---------------------------------------------------------------------------
eMgoongaGameContext::eMgoongaGameContext(eInputController*  _input,
                                         std::vector<IWindowImGui*> _externalGui,
						                             const std::string& _modelsPath,
						                             const std::string& _assetsPath, 
						                             const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
, pipeline(width, height)
{
}

//--------------------------------------------------------------------------
eMgoongaGameContext::~eMgoongaGameContext() {}

//--------------------------------------------------------------------------
void eMgoongaGameContext::InitializeExternalGui()
{
  // Lights & Cameras
  externalGui[0]->Add(TEXT, "Light", nullptr);
  externalGui[0]->Add(SLIDER_FLOAT_3, "Light direction.", &GetMainLight().light_position);
  externalGui[0]->Add(TEXT, "Camera", nullptr);
  externalGui[0]->Add(SLIDER_FLOAT_3, "position", &GetMainCamera().PositionRef());
  externalGui[0]->Add(SLIDER_FLOAT_3, "direction", &GetMainCamera().ViewDirectionRef());
  
  //Debug
  externalGui[1]->Add(CHECKBOX, "Show bounding boxes", &pipeline.GetBoundingBoxBoolRef());
  externalGui[1]->Add(CHECKBOX, "Use Multi sampling", &pipeline.GetMultiSamplingBoolRef());
  externalGui[1]->Add(CHECKBOX, "Sky box", &pipeline.GetSkyBoxOnRef());
  externalGui[1]->Add(SLIDER_FLOAT, "Blur coefficients", &pipeline.GetBlurCoefRef());
  std::function<void()> emit_partilces_callback = [this]()
  {
    pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f),
                                                  texManager->Find("Tatlas2"),
                                                  soundManager->GetSound("shot_sound"),
                                                  texManager->Find("Tatlas2")->numberofRows));
  };
  std::function<void()> update_uniforms_callback = [this]()
  {
    pipeline.GetRenderManager().UpdateShadersInfo();
  };
  externalGui[1]->Add(BUTTON, "Emit particle system", (void*)&emit_partilces_callback);
  externalGui[1]->Add(CHECKBOX, "Debug white", &pipeline.GetDebugWhite());
  externalGui[1]->Add(CHECKBOX, "Debug Tex Coords", &pipeline.GetDebugTexCoords());

  externalGui[1]->Add(SLIDER_FLOAT, "PBR debug dist", (void*)&pipeline.MaterialMetalness());
  externalGui[1]->Add(SLIDER_FLOAT, "PBR debug intensity", (void*)&pipeline.MaterialRoughness());
  externalGui[1]->Add(SLIDER_FLOAT, "PBR debug shininess", (void*)&pipeline.MaterialShininess());
  externalGui[1]->Add(SLIDER_FLOAT, "PBR debug ao", (void*)&pipeline.MaterialAO());

  externalGui[1]->Add(TEXTURE, "Reflection buffer", (void*)pipeline.GetReflectionBufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Refraction buffer", (void*)pipeline.GetRefractionBufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Shadow buffer", (void*)pipeline.GetShadowBufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Gaussian buffer", (void*)pipeline.GetGausian2BufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Bright filter buffer", (void*)pipeline.GetBrightFilter().id);

  //Objects
  externalGui[2]->Add(OBJECT_REF, "Object", (void*)&m_focused);
  
  //Shaders
  externalGui[3]->Add(BUTTON, "Update shaders", (void*)&update_uniforms_callback);
  externalGui[3]->Add(SHADER, "Shaders", (void*)&pipeline.GetShaderInfos());

  m_focused = m_objects[0];
  OnFocusedChanged(); // make event handler
}

//--------------------------------------------------------------------------
bool eMgoongaGameContext::OnKeyPress(uint32_t asci)
{
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
bool eMgoongaGameContext::OnMouseMove(uint32_t x, uint32_t y)
{
  if (GetMainCamera().getCameraRay().IsPressed())
  {
    if (m_grab_camera_line != std::nullopt) // right button is pressed @todo explicit right button
    {
      GetMainCamera().getCameraRay().Update(GetMainCamera(), static_cast<float>(x), static_cast<float>(y), width, height);
      GetMainCamera().getCameraRay().press(x, y);

      dbb::plane pl(m_intersaction,
                    glm::vec3(0.0f, m_intersaction.y, 1.0f),
                    glm::vec3(1.0f, m_intersaction.y, 0.0f)); // arbitrary triangle on xz plane
      glm::vec3 new_intersaction = dbb::intersection(pl, GetMainCamera().getCameraRay().getLine());
      m_translation_vector = new_intersaction - m_intersaction;
    }
    else
    {
      m_framed.reset(new std::vector<shObject>(GetMainCamera().getCameraRay().onMove(GetMainCamera(), m_objects, static_cast<float>(x), static_cast<float>(y)))); 	//to draw a frame
      return true;
    }
  }
	return false;
}

//--------------------------------------------------------------------------
bool eMgoongaGameContext::OnMousePress(uint32_t x, uint32_t y, bool left)
{
  if (m_framed)
    m_framed->clear();

  GetMainCamera().getCameraRay().Update(GetMainCamera(), static_cast<float>(x), static_cast<float>(y), width, height);
  GetMainCamera().getCameraRay().press(x, y);
  GetMainCamera().MovementSpeedRef() = 0.f;
  auto objects = m_objects;
  objects.insert(objects.end(), m_pbr_objs.begin(), m_pbr_objs.end());
  auto [new_focused, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(objects);

	if(left)
	{
    if (new_focused != m_focused)
    {
      m_focused = new_focused;
      OnFocusedChanged();
    }
	}
  else // right click
  {
    if (new_focused) // right click on object
    {
      m_focused = new_focused;
      OnFocusedChanged();
      m_grab_translation = new_focused->GetTransform()->getTranslation();
      m_intersaction = intersaction;
      m_grab_camera_line = GetMainCamera().getCameraRay().getLine();
    }
  }

	if(m_focused && m_focused->GetScript())
		m_focused->GetScript()->OnMousePress(x, y, left);

	return true;
}

//---------------------------------------------------------------------------------
bool eMgoongaGameContext::OnMouseRelease()
{
  GetMainCamera().getCameraRay().release();
  m_grab_camera_line = std::nullopt;
  m_translation_vector = vec3{ 0.0f, 0.0f, 0.0f };
  GetMainCamera().MovementSpeedRef() = 0.05f;
	return true;
}

//------------------------------------------------------------------------------
void eMgoongaGameContext::OnFocusedChanged()
{
  if (m_focused)
  {

  }
  else
  {
  }
}

//-------------------------------------------------------------------------------
void eMgoongaGameContext::InitializeGL()
{
	eMainContextBase::InitializeGL();
	
  Texture* tex = texManager->Find("TButton_red");
  Texture* flag = texManager->Find("TSpanishFlag0_s");
  glm::ivec2 topLeft{ 160,450 };
  glm::ivec2 bottomRight{ 1030, 725 };

	guis.emplace_back(new GUIWithAlpha(0, 0, (bottomRight.x - topLeft.x)/4, (bottomRight.y - topLeft.y)/4, width, height));
  guis[0]->SetTexture(*tex, topLeft, bottomRight);
  guis[0]->SetChild(std::make_shared<GUIWithAlpha>(0, (bottomRight.y - topLeft.y)/4, (bottomRight.x - topLeft.x)/4, (bottomRight.y - topLeft.y)/4, width, height));
  guis[0]->GetChildren()[0]->SetTexture(*tex, topLeft, bottomRight);
  guis[0]->GetChildren()[0]->SetVisible(false);
  guis[0]->setCommand(std::make_shared<MenuBehaviorLeanerMove>(guis[0]->GetChildren()[0].get(),
    math::AnimationLeaner{ 
      {glm::vec3(guis[0]->getTopLeft().x, guis[0]->getTopLeft().y, 0)},
      {glm::vec3(guis[0]->GetChildren()[0]->getTopLeft().x, guis[0]->GetChildren()[0]->getTopLeft().y, 0)},
      1000 }));

  guis.emplace_back(new Cursor(0, 0, 30, 30, width, height));
  guis[1]->SetTexture(*flag, { 0,0 }, { flag->mTextureWidth, flag->mTextureHeight});
  guis.emplace_back(new Movable2D(400, 0, 60, 60, width, height));
  guis[2]->SetTexture(*flag, { 0,0 }, { flag->mTextureWidth, flag->mTextureHeight });

	inputController->AddObserver(this, WEAK);
  inputController->AddObserver(&GetMainCamera().getCameraRay(), WEAK);
  inputController->AddObserver(&GetMainCamera(), WEAK);
  inputController->AddObserver(guis[0].get(), MONOPOLY);//monopoly takes only mouse
  inputController->AddObserver(guis[1].get(), WEAK);
  inputController->AddObserver(guis[2].get(), STRONG);
  inputController->AddObserver(externalGui[0], MONOPOLY);
  inputController->AddObserver(externalGui[1], MONOPOLY);
  inputController->AddObserver(externalGui[2], MONOPOLY);
  inputController->AddObserver(externalGui[3], MONOPOLY);
}

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
	pipeline.InitializeBuffers(true);
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

  std::vector<const Texture*> textures{ texManager->Find("pbr1_basecolor"),
                                        texManager->Find("pbr1_normal"),
                                        texManager->Find("pbr1_metallic"),
                                        texManager->Find("pbr1_roughness") };
  modelManager->Add("sphere_textured", textures /*std::vector<const Texture*>{}*/); // or textures

	//TERRAIN
	std::unique_ptr<TerrainModel> terrainModel = modelManager->CloneTerrain("simple");
  terrainModel->initialize(texManager->Find("Tgrass0_d"),
							             texManager->Find("Tgrass0_d"),
							             texManager->Find("Tblue"),
							             texManager->Find("TOcean0_s"),
                           false);
	//OBJECTS
  ObjectFactoryBase factory;
  shObject wallCube = factory.CreateObject(modelManager->Find("wall_cube"));
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	m_objects.push_back(wallCube);

	shObject containerCube = factory.CreateObject(modelManager->Find("container_cube"));
	containerCube->GetTransform()->setTranslation(vec3(-2.5f, 3.0f, 3.5f));
	m_objects.push_back(containerCube);

  shObject grassPlane = factory.CreateObject(modelManager->Find("grass_plane"));
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, 1.5f, 0.0f));
	m_objects.push_back(grassPlane);

	shObject nanosuit = factory.CreateObject(modelManager->Find("nanosuit"));
	nanosuit->GetTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
  nanosuit->GetTransform()->setRotation(0.0f, glm::radians(180.0f), 0.0f);
	nanosuit->GetTransform()->setScale(vec3(0.12f, 0.12f, 0.12f));
	m_objects.push_back(nanosuit);

  nanosuit->SetScript(new eShipScript(texManager->Find("TSpanishFlag0_s"),
                                      pipeline.GetRenderManager(),
                                      GetMainCamera(),
                                      texManager->Find("Tatlas2"),
                                      soundManager->GetSound("shot_sound"),
                                      &GetMainCamera().getCameraRay(),
                                      pipeline.GetWaterHeight()));

  shObject terrain = factory.CreateObject(std::shared_ptr<IModel>(terrainModel.release()));
  terrain->SetName("Terrain");
  terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
  terrain->GetTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
  m_objects.push_back(terrain);

	shObject wolf = factory.CreateObject(modelManager->Find("wolf"));
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setTranslation(vec3(4.0f, 3.0f, 0.0f));
	wolf->SetRigger(new Rigger((Model*)modelManager->Find("wolf").get())); //@todo improve
	wolf->GetRigger()->ChangeName(std::string(), "Running");//@todo improve
	m_objects.push_back(wolf);

	shObject brickCube = factory.CreateObject(modelManager->Find("brick_cube"));
	brickCube->SetModel(modelManager->Find("brick_cube"));
	brickCube->GetTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	m_objects.push_back(brickCube);

  shObject guard = factory.CreateObject(modelManager->Find("guard"));
  guard->GetTransform()->setTranslation(vec3(2.0f, 2.0f, 0.0f));
  guard->GetTransform()->setRotation(glm::radians(-90.0f), glm::radians(-90.0f), 0.0f);
  guard->GetTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
  guard->GetTransform()->setUp(glm::vec3(0.0f,0.0f,1.0f));
  guard->GetTransform()->setForward(glm::vec3(0.0f, 1.0f, 0.0f));
  guard->SetRigger(new Rigger((Model*)modelManager->Find("guard").get()));//@todo improve
  m_objects.push_back(guard);

  shObject zombie = factory.CreateObject(modelManager->Find("zombie"));
  zombie->GetTransform()->setTranslation(vec3(1.0f, 2.0f, 0.0f));
  zombie->GetTransform()->setRotation(0.0f, glm::radians(180.0f), 0.0f);
  zombie->GetTransform()->setScale(glm::vec3(0.01f, 0.01f, 0.01f));
  zombie->SetRigger(new Rigger((Model*)modelManager->Find("zombie").get()));//@todo improve
  m_objects.push_back(zombie);

	//light_cube
	lightObject = factory.CreateObject(modelManager->Find("white_sphere"));
	lightObject->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	lightObject->GetTransform()->setTranslation(GetMainLight().light_position);
	m_objects.push_back(lightObject);

  shObject obj = factory.CreateObject(modelManager->Find("sphere_textured"));
  obj->GetTransform()->setTranslation(glm::vec3(-2.0f,3.5f,1.5f));
  m_pbr_objs.push_back(obj);
}

//-------------------------------------------------------------------------
void eMgoongaGameContext::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
	/*pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f),
                                                                   texManager->Find("Tatlas2"),
                                                                   soundManager->GetSound("shot_sound"),
                                                                   texManager->Find("Tatlas2")->numberofRows));*/
}

//-------------------------------------------------------------------------------
void eMgoongaGameContext::PaintGL()
{
	eMainContextBase::PaintGL();

	std::vector<shObject> flags;
	for (auto &object : m_objects)
	{
		if (object->GetScript())
		{
			object->GetScript()->Update(m_objects);
			eShipScript* script = dynamic_cast<eShipScript*>(object->GetScript());
			if(script)
        flags.push_back(script->GetChildrenObjects()[0]);
		}
	}
  if (m_grab_camera_line != std::nullopt && m_translation_vector != glm::vec3{0.f,0.f,0.0f})
    m_focused->GetTransform()->setTranslation(m_grab_translation + m_translation_vector);

  lightObject->GetTransform()->setTranslation(GetMainLight().light_position);
	GetMainLight().light_direction = { -GetMainLight().light_position };
  
  //need better design less copying
  std::shared_ptr<std::vector<shObject>> focused_output = m_framed;
  if (!focused_output || !(focused_output->size() > 1))
    focused_output = m_focused ? std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{ m_focused })
                               : std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{});

  std::map<eOpenGlRenderPipeline::RenderType, std::vector<shObject>> objects;
  objects.insert({ eOpenGlRenderPipeline::RenderType::MAIN, m_objects});
  objects.insert({ eOpenGlRenderPipeline::RenderType::OUTLINED, *focused_output });
  objects.insert({ eOpenGlRenderPipeline::RenderType::FLAG, flags });
  objects.insert({ eOpenGlRenderPipeline::RenderType::PBR, m_pbr_objs });

	pipeline.RenderFrame(objects, GetMainCamera(), GetMainLight(), guis);
}

//-------------------------------------------------------------------------------
uint32_t eMgoongaGameContext::GetFinalImageId()
{
  return pipeline.GetDefaultBufferTexture().id;
}
