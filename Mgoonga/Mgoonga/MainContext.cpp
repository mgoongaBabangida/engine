#include "stdafx.h"
#include "MainContext.h"

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

#include "ShipScript.h"

//---------------------------------------------------------------------------
eMainContext::eMainContext(eInputController*  _input,
  std::vector<IWindowImGui*> _externalGui,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath, 
						   const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
, pipeline(m_objects, camRay, width, height)
, m_camera(width, height, nearPlane, farPlane)
{
	//Light init!
	m_light.ambient			= vec3(0.05f, 0.05f, 0.05f);
	m_light.diffuse			= vec3(0.75f, 0.75f, 0.75f);
	m_light.specular		= vec3(0.5f, 0.5f, 0.5f);
	m_light.light_position	= vec4(0.0f, 4.0f, -1.0f, -1.0f);
	m_light.type			= eLightType::DIRECTION;
  m_camera.setPosition(glm::vec3(-1.0f, 4.0f, -2.5f));
  m_camera.setDirection(glm::vec3(0.6f, -0.10f, 0.8f));
}

//--------------------------------------------------------------------------
void eMainContext::InitializeExternalGui()
{
  externalGui[0]->Add(TEXT, "Light", nullptr);
  externalGui[0]->Add(SLIDER_FLOAT, "Y direction", &m_light.light_position.y);
  externalGui[0]->Add(SLIDER_FLOAT, "Z direction", &m_light.light_position.z);
  externalGui[0]->Add(SLIDER_FLOAT, "X direction", &m_light.light_position.x);
  externalGui[0]->Add(CHECKBOX, "Show bounding boxes", &pipeline.GetBoundingBoxBoolRef());
  externalGui[0]->Add(CHECKBOX, "Use Multi sampling", &pipeline.GetMultiSamplingBoolRef());
  externalGui[0]->Add(CHECKBOX, "Sky box", &pipeline.GetSkyBoxOnRef());
  externalGui[0]->Add(SLIDER_FLOAT, "Blur coefficients", &pipeline.GetBlurCoefRef());
  externalGui[0]->Add(SLIDER_FLOAT_3_CALLBACK, "Camera pos.", &m_camera.PositionRef());
  externalGui[0]->Add(SLIDER_FLOAT_3_CALLBACK, "Camera dir.", &m_camera.ViewDirectionRef());
  emit_partilces_callback = [this]()
  {
    pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f),
                                                  texManager->Find("Tatlas2"),
                                                  soundManager->GetSound("shot_sound"),
                                                  texManager->Find("Tatlas2")->numberofRows));
  };
  externalGui[0]->Add(BUTTON, "Emit particle system", (void*)&emit_partilces_callback);
  externalGui[0]->Add(CHECKBOX, "Debug white", &pipeline.GetDebugWhite());
  externalGui[0]->Add(CHECKBOX, "Debug Tex Coords", &pipeline.GetDebugTexCoords());

  externalGui[1]->Add(TEXTURE, "Reflection buffer", (void*)pipeline.GetReflectionBufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Refraction buffer", (void*)pipeline.GetRefractionBufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Shadow buffer", (void*)pipeline.GetShadowBufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Gaussian buffer", (void*)pipeline.GetGausian2BufferTexture().id);
  externalGui[1]->Add(TEXTURE, "Bright filter buffer", (void*)pipeline.GetBrightFilter().id);

  transfer_data_position.callback = [this]()
  {
    if (m_focused)
    {
      m_focused->GetTransform()->setTranslation(glm::vec3{ transfer_data_position.data[0],
                                                           transfer_data_position.data[1],
                                                           transfer_data_position.data[2] });
    }
  };
  transfer_data_position.min = -10.0f;
  transfer_data_position.max = 10.0f;
  transfer_data_rotation.callback = [this]()
  {
    if (m_focused)
    {
      m_focused->GetTransform()->setRotation(transfer_data_rotation.data[0] * PI * 2,
                                             transfer_data_rotation.data[1] * PI * 2,
                                             transfer_data_rotation.data[2] * PI * 2);
    }
  };
  transfer_data_rotation.min = 0.0;
  transfer_data_rotation.max = 1.0f;
  transfer_data_scale.callback = [this]()
  {
    if (m_focused)
    {
      m_focused->GetTransform()->setScale(glm::vec3{ transfer_data_scale.data[0],
                                                     transfer_data_scale.data[1],
                                                     transfer_data_scale.data[2] });
    }
  };
  transfer_data_scale.min = 0.0f;
  transfer_data_scale.max = 5.0f;
  transfer_meshes.callback = [this](size_t _current)
  {
    if (m_focused)
    {
      transfer_textures = m_focused->GetModel()->GetMeshes()[_current]->GetTextures();
      if (transfer_textures.empty())
        transfer_textures = m_focused->GetModel()->GetTexturesModelLevel();
    }
  };

  transfer_animations.callback = [this](size_t _current)
  {
    cur_animation = _current - 1;
  };

  play_callback = [this]()
  {
    if(m_focused && m_focused->GetModel()->GetAnimationCount() != 0)
      m_focused->GetRigger()->Apply(cur_animation);
  };

  stop_callback = [this]()
  {
    m_focused->GetRigger()->Stop();
  };
  externalGui[2]->Add(SLIDER_FLOAT_3_CALLBACK, "Position", (void*)&transfer_data_position);
  externalGui[2]->Add(SLIDER_FLOAT_3_CALLBACK, "Rotation", (void*)&transfer_data_rotation);
  externalGui[2]->Add(SLIDER_FLOAT_3_CALLBACK, "Scale", (void*)&transfer_data_scale);
  externalGui[2]->Add(TEXT_INT, "Number of vertices ", (void*)&transfer_num_vertices);
  externalGui[2]->Add(TEXT_INT, "Number of meshes ", (void*)&transfer_num_meshes);
  externalGui[2]->Add(COMBO_BOX, "Current mesh ", (void*)&transfer_meshes);
  externalGui[2]->Add(TEXTURE_ARRAY, "Textures ", (void*)&transfer_textures);
  externalGui[2]->Add(TEXT_INT, "Number of animations ", (void*)&transfer_num_animations);
  externalGui[2]->Add(COMBO_BOX, "Current animations ", (void*)&transfer_animations);
  externalGui[2]->Add(BUTTON, "Play current animations ", (void*)&play_callback);
  externalGui[2]->Add(BUTTON, "Stop current animations ", (void*)&stop_callback);

  externalGui[0]->Add(SLIDER_FLOAT, "PBR light dist", (void*)&pipeline.MaterialMetalness());
  externalGui[0]->Add(SLIDER_FLOAT, "PBR light intensity", (void*)&pipeline.MaterialRoughness());

  m_focused = m_objects[4];
  OnFocusedChanged();
}

//--------------------------------------------------------------------------
bool eMainContext::OnMouseMove(uint32_t x, uint32_t y)
{
  if (camRay.IsPressed())
    {
    m_framed.reset(new std::vector<shObject>(camRay.onMove(m_camera, m_objects, static_cast<float>(x), static_cast<float>(y)))); 	//to draw a frame
    return true;
    }
	return false;
}

//--------------------------------------------------------------------------
bool eMainContext::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	case ASCII_J:	{ if (m_focused)	m_focused->GetRigidBody()->MoveLeft(m_objects);}				return true;
	case ASCII_L:	{ if (m_focused)	m_focused->GetRigidBody()->MoveRight(m_objects);}				return true;
	case ASCII_K:	{ if (m_focused)	m_focused->GetRigidBody()->MoveBack(m_objects);}				return true;
	case ASCII_I:	{ if (m_focused)	m_focused->GetRigidBody()->MoveForward(m_objects);}				return true;
	case ASCII_Z:	{ if (m_focused)	m_focused->GetRigidBody()->MoveUp(m_objects);}					return true;
	case ASCII_X:	{ if (m_focused)	m_focused->GetRigidBody()->MoveDown(m_objects);}				return true;
	case ASCII_C:	{ if (m_focused)	m_focused->GetRigidBody()->TurnRight(m_objects);}				return true;
	case ASCII_V:	{ if (m_focused)	m_focused->GetRigidBody()->TurnLeft(m_objects);}				return true;
	case ASCII_B:	{ if (m_focused)	m_focused->GetRigidBody()->LeanRight(m_objects);}				return true;
	case ASCII_N:	{ if (m_focused)	m_focused->GetRigidBody()->LeanLeft(m_objects);}				return true;
	case ASCII_U:	{ if (m_focused)	m_focused->GetRigidBody()->LeanForward(m_objects);}				return true;
	case ASCII_H:	{ if (m_focused)	m_focused->GetRigidBody()->LeanBack(m_objects);}				return true;
	case 27: {} return false; //ESC @todo
	//case ASCII_G:	{ if (m_focused)	m_focused->GetScript()->OnKeyPress(ASCII_G);}	return true;
	default: return false;
	}
}

//--------------------------------------------------------------------------
bool eMainContext::OnMousePress(uint32_t x, uint32_t y, bool left)
{
  if (m_framed)
    m_framed->clear();

	camRay.Update(m_camera, x, y, width, height);
	if(left)
	{
		camRay.press(x, y);
		auto new_focused = camRay.calculateIntersaction(m_objects);
    if (new_focused != m_focused)
    {
      m_focused = new_focused;
      OnFocusedChanged();
    }
	}

	if(m_focused && m_focused->GetScript())
		m_focused->GetScript()->OnMousePress(x, y, left);

	return true;
}

//---------------------------------------------------------------------------------
bool eMainContext::OnMouseRelease()
{
	camRay.release();
	return true;
}

//------------------------------------------------------------------------------
void eMainContext::OnFocusedChanged()
{
  if (m_focused)
  {
    transfer_data_position.data[0] = m_focused->GetTransform()->getTranslation().x;
    transfer_data_position.data[1] = m_focused->GetTransform()->getTranslation().y;
    transfer_data_position.data[2] = m_focused->GetTransform()->getTranslation().z;
    glm::vec3 rot = glm::eulerAngles(m_focused->GetTransform()->getRotation());
    transfer_data_rotation.data[0] = rot.x / (PI * 2);
    transfer_data_rotation.data[1] = rot.y / (PI * 2);
    transfer_data_rotation.data[2] = rot.z / (PI * 2);
    transfer_data_scale.data[0] = m_focused->GetTransform()->getScaleAsVector().x;
    transfer_data_scale.data[1] = m_focused->GetTransform()->getScaleAsVector().y;
    transfer_data_scale.data[2] = m_focused->GetTransform()->getScaleAsVector().z;
    transfer_num_vertices = m_focused->GetModel()->GetVertexCount();
    transfer_num_meshes = m_focused->GetModel()->GetMeshCount();
    transfer_meshes.data.clear();
    for (int i = 0; i < transfer_num_meshes; ++i)
      transfer_meshes.data.push_back(std::to_string(i+1));

    transfer_num_animations = m_focused->GetModel()->GetAnimationCount();
    transfer_animations.data.clear();
    for (int i = 0; i < transfer_num_animations; ++i)
      transfer_animations.data.push_back(std::to_string(i + 1));
    transfer_textures = m_focused->GetModel()->GetMeshes()[0]->GetTextures();
    if (transfer_textures.empty())
      transfer_textures = m_focused->GetModel()->GetTexturesModelLevel();
    current_animations = m_focused->GetModel()->GetAnimations();
  }
  else
  {
    transfer_data_position.data[0] = 0.0f;
    transfer_data_position.data[1] = 0.0f;
    transfer_data_position.data[2] = 0.0f;
    transfer_data_rotation.data[0] = 0.0f;
    transfer_data_rotation.data[1] = 0.0f;
    transfer_data_rotation.data[2] = 0.0f;
    transfer_data_scale.data[0] = 0.0f;
    transfer_data_scale.data[1] = 0.0f;
    transfer_data_scale.data[2] = 0.0f;
  }
}

//-------------------------------------------------------------------------------
void eMainContext::InitializeGL()
{
	eMainContextBase::InitializeGL();
	
	//Camera Ray
	camRay.init(width, height, nearPlane, farPlane);
	
	m_objects[4]->SetScript(new eShipScript(texManager->Find("TSpanishFlag0_s"),
											pipeline.GetRenderManager(),
                      m_camera,
											texManager->Find("Tatlas2"),
											soundManager->GetSound("shot_sound"),
											&camRay,
                      pipeline.GetWaterHeight()));

	guis.emplace_back(GUI(0, 0, width / 4, height / 4, width, height));
	//guis[0].setCommand(std::make_shared<AnimStart>(AnimStart(m_objects[6])));
	guis.emplace_back(GUI(width / 4 * 3, height / 4 * 3, width / 4, height / 4, width, height));
	//guis[1].setCommand(std::make_shared<AnimStop>(AnimStop(m_objects[6])));

	inputController->AddObserver(this, MONOPOLY);
  inputController->AddObserver(&camRay, WEAK);
  inputController->AddObserver(&m_camera, WEAK);
  inputController->AddObserver(&guis[0], MONOPOLY);//monopoly takes only mouse
  inputController->AddObserver(&guis[1], MONOPOLY);
  inputController->AddObserver(externalGui[0], MONOPOLY);
  inputController->AddObserver(externalGui[1], MONOPOLY);
  inputController->AddObserver(externalGui[2], MONOPOLY);
}

//-------------------------------------------------------------------------------
void eMainContext::InitializeSounds()
{
	//sound->loadListner(m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z); //!!!
}

//-------------------------------------------------------------------------------
void eMainContext::InitializePipline()
{
	pipeline.Initialize();
	// call all the enable pipeline functions
}

//-----------------------------------------------------------------------------
void eMainContext::InitializeBuffers()
{
	pipeline.InitializeBuffers(m_light.type == eLightType::POINT);
}

#include <opengl_assets/ShpereTexturedModel.h>

//-----------------------------------------------------------------------------
void eMainContext::InitializeModels()
{
	eMainContextBase::InitializeModels();

	//MODELS
	modelManager->Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());
	modelManager->Add("boat", (GLchar*)std::string(modelFolderPath + "Medieval Boat/Medieval Boat.obj").c_str());
	//modelManager.Add("spider", (GLchar*)std::string(ModelFolderPath + "ogldev-master/Content/spider.obj").c_str());
	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager->Add("guard", (GLchar*)std::string(modelFolderPath + "ogldev-master/Content/guard/boblampclean.md5mesh").c_str(), true);
	//modelManager.Add("stairs", (GLchar*)std::string(modelFolderPath + "stairs.blend").c_str());
  modelManager->Add("zombie", (GLchar*)std::string(modelFolderPath + "Thriller Part 3/Thriller Part 3.dae").c_str());

	//TERRAIN
	m_TerrainModel.swap(modelManager->CloneTerrain("simple"));
	m_TerrainModel->initialize(texManager->Find("Tgrass0_d"),
							   texManager->Find("Tgrass0_d"),
							   texManager->Find("Tblue"),
							   texManager->Find("TOcean0_s"),
                 true);
	//OBJECTS
	shObject wallCube = shObject(new eObject);
	wallCube->SetModel(modelManager->Find("wall_cube").get());
	wallCube->SetTransform(new Transform);
	wallCube->SetCollider(new BoxCollider);
	wallCube->SetRigidBody(new eRigidBody);
	wallCube->GetRigidBody()->SetObject(wallCube.get());
	wallCube->GetCollider()->CalculateExtremDots(wallCube->GetModel()->GetPositions());
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	m_objects.push_back(wallCube);

	shObject containerCube = shObject(new eObject);
	containerCube->SetModel(modelManager->Find("container_cube").get());
	containerCube->SetTransform(new Transform);
	containerCube->SetCollider(new BoxCollider);
	containerCube->SetRigidBody(new eRigidBody);
	containerCube->GetRigidBody()->SetObject(containerCube.get());
	containerCube->GetCollider()->CalculateExtremDots(containerCube->GetModel()->GetPositions());
	containerCube->GetTransform()->setTranslation(vec3(-2.5f, 3.0f, 3.5f));
	m_objects.push_back(containerCube);

	shObject arrow = shObject(new eObject);
	arrow->SetModel(modelManager->Find("arrow").get());
	arrow->SetName("arrow");
	arrow->SetTransform(new Transform);
	arrow->SetCollider(new BoxCollider);
	arrow->SetRigidBody(new eRigidBody);
	arrow->GetRigidBody()->SetObject(arrow.get());
	arrow->GetCollider()->CalculateExtremDots(arrow->GetModel()->GetPositions());
	arrow->GetTransform()->setTranslation(vec3(1.0f, 1.0f, -1.0f));
	m_objects.push_back(arrow);

	shObject grassPlane = shObject(new eObject);
	grassPlane->SetModel(modelManager->Find("grass_plane").get());
	grassPlane->SetTransform(new Transform);
	grassPlane->SetCollider(new BoxCollider);
	grassPlane->SetRigidBody(new eRigidBody);
	grassPlane->GetRigidBody()->SetObject(grassPlane.get());
	grassPlane->GetCollider()->CalculateExtremDots(grassPlane->GetModel()->GetPositions());
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_objects.push_back(grassPlane);

	shObject nanosuit = shObject(new eObject);
	nanosuit->SetModel(modelManager->Find("nanosuit").get());
	nanosuit->SetTransform(new Transform);
	nanosuit->SetCollider(new BoxCollider);
	nanosuit->SetRigidBody(new eRigidBody);
	nanosuit->GetRigidBody()->SetObject(nanosuit.get());
	nanosuit->GetCollider()->CalculateExtremDots(nanosuit->GetModel()->GetPositions());
	nanosuit->GetTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->GetTransform()->setScale(vec3(0.12f, 0.12f, 0.12f));
	m_objects.push_back(nanosuit);

  shObject terrain = shObject(new eObject);
  terrain->SetModel((IModel*)m_TerrainModel.get());
  terrain->SetTransform(new Transform);
  terrain->SetCollider(new BoxCollider);
  terrain->SetRigidBody(new eRigidBody);
  terrain->GetRigidBody()->SetObject(terrain.get());
  terrain->GetCollider()->CalculateExtremDots(terrain->GetModel()->GetPositions());
  terrain->SetName("Terrain");
  terrain->GetTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
  terrain->GetTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
  m_objects.push_back(terrain);

	shObject wolf = shObject(new eObject);
	wolf->SetModel(modelManager->Find("wolf").get());
	wolf->SetTransform(new Transform);
	wolf->SetCollider(new BoxCollider);
	wolf->SetRigidBody(new eRigidBody);
	wolf->GetRigidBody()->SetObject(wolf.get());
	wolf->GetCollider()->CalculateExtremDots(wolf->GetModel()->GetPositions());
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setTranslation(vec3(4.0f, 3.0f, 0.0f));
	wolf->SetRigger(new Rigger((Model*)modelManager->Find("wolf").get()));
	wolf->GetRigger()->ChangeName(std::string(), "Running");
	m_objects.push_back(wolf);

	shObject brickCube = shObject(new eObject);
	brickCube->SetModel(modelManager->Find("brick_cube").get());
	brickCube->SetTransform(new Transform);
	brickCube->SetCollider(new BoxCollider);
	brickCube->SetRigidBody(new eRigidBody);
	brickCube->GetRigidBody()->SetObject(brickCube.get());
	brickCube->GetCollider()->CalculateExtremDots(brickCube->GetModel()->GetPositions());
	brickCube->GetTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	m_objects.push_back(brickCube);

  shObject guard = shObject(new eObject);
  guard->SetModel(modelManager->Find("guard").get());
  guard->SetTransform(new Transform);
  guard->SetCollider(new BoxCollider);
  guard->SetRigidBody(new eRigidBody);
  guard->GetRigidBody()->SetObject(guard.get());
  guard->GetCollider()->CalculateExtremDots(guard->GetModel()->GetPositions());
  guard->GetTransform()->setTranslation(vec3(2.0f, 2.0f, 0.0f));
  guard->GetTransform()->setRotation(glm::radians(-90.0f), glm::radians(-90.0f), 0.0f);
  guard->GetTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
  guard->SetRigger(new Rigger((Model*)modelManager->Find("guard").get()));
  m_objects.push_back(guard);

  shObject zombie = shObject(new eObject);
  zombie->SetModel(modelManager->Find("zombie").get());
  zombie->SetTransform(new Transform);
  zombie->SetCollider(new BoxCollider);
  zombie->SetRigidBody(new eRigidBody);
  zombie->GetRigidBody()->SetObject(zombie.get());
  zombie->GetCollider()->CalculateExtremDots(zombie->GetModel()->GetPositions());
  zombie->GetTransform()->setTranslation(vec3(1.0f, 2.0f, 0.0f));
  zombie->GetTransform()->setScale(glm::vec3(0.01f, 0.01f, 0.01f));
  zombie->SetRigger(new Rigger((Model*)modelManager->Find("zombie").get()));
  m_objects.push_back(zombie);

	//light_cube
	lightObject = shObject(new eObject);
	lightObject->SetModel(modelManager->Find("white_sphere").get());
	lightObject->SetTransform(new Transform);
	lightObject->SetCollider(new BoxCollider);
	lightObject->SetRigidBody(new eRigidBody);
	lightObject->GetRigidBody()->SetObject(lightObject.get());
	lightObject->GetCollider()->CalculateExtremDots(lightObject->GetModel()->GetPositions());
	lightObject->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	lightObject->GetTransform()->setTranslation(m_light.light_position);
	m_objects.push_back(lightObject);

	/*shObject boat = shObject(new eObject(modelManager.Find("boat").get()));
	boat->GetTransform()->setScale(vec3(0.0001f, 0.0001f, 0.0001f));
	m_objects.push_back(boat);*/
}

//-------------------------------------------------------------------------
void eMainContext::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
	pipeline.GetRenderManager().AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f),
                                                                   texManager->Find("Tatlas2"),
                                                                   soundManager->GetSound("shot_sound"),
                                                                   texManager->Find("Tatlas2")->numberofRows));
}

//-------------------------------------------------------------------------------
void eMainContext::PaintGL()
{
	eMainContextBase::PaintGL();

	std::vector<eObject*> flags;
	for (auto &object : m_objects)
	{
		if (object->GetScript())
		{
			object->GetScript()->Update(m_objects);
			eShipScript* script = dynamic_cast<eShipScript*>(object->GetScript());
			if(script) flags.push_back(script->GetChildrenObjects()[0]);
		}
	}

  lightObject->GetTransform()->setTranslation(m_light.light_position);
	m_light.light_direction = { -m_light.light_position };
  
  //need better design less copying
  std::shared_ptr<std::vector<shObject>> focused_output = m_framed;
  if (!focused_output || !(focused_output->size() > 1))
    focused_output = m_focused ? std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{ m_focused })
                               : std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{});

	pipeline.RenderFrame(m_camera, m_light, guis, *focused_output, flags); //better design impl new standard
}
