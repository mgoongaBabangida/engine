#include "stdafx.h"

#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "SceneSerializer.h"
#include "ModelManagerYAML.h"
#include "AnimationManagerYAML.h"
#include "ParticleSystemToolController.h"
#include "TerrainGeneratorTool.h"
#include "BezierCurveUIController.h"

#include <base/InputController.h>
#include <tcp_lib/Network.h>
#include <tcp_lib/Server.h>
#include <tcp_lib/Client.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/Sound.h>

#include <sdl_assets/ImGuiContext.h>

#include <math/Clock.h>
#include <math/ParticleSystem.h>

#include <thread>
#include <sstream>

//-----------------------------------------------------------------
eMainContextBase::eMainContextBase(eInputController* _input,
	                               std::vector<IWindowImGui*> _externalGui,
	                               const std::string& _modelsPath,
	                               const std::string& _assetsPath,
	                               const std::string& _shadersPath)
: m_input_controller(_input)
, modelFolderPath(_modelsPath)
, assetsFolderPath(_assetsPath)
, shadersFolderPath(_shadersPath)
, texManager(new eTextureManager)
, modelManager(new ModelManagerYAML)
, animationManager(new AnimationManagerYAML)
, soundManager(new eSoundManager(_assetsPath))
, externalGui(_externalGui)
, pipeline(width, height)
{

}

//-------------------------------------------------------------------------
eMainContextBase::~eMainContextBase()
{
	if(tcpTimer)
		tcpTimer->stop();
}

//-------------------------------------------------------------------------
size_t eMainContextBase::Width() const { return width; }
//-------------------------------------------------------------------------
size_t eMainContextBase::Height()  const { return height; }

//*********************InputObserver*********************************
//--------------------------------------------------------------------------
bool eMainContextBase::OnKeyPress(uint32_t _asci)
{
	switch (_asci)
	{
		case ASCII_Q:
		{
			if (m_gizmo_type == GizmoType::TRANSLATE)
				m_gizmo_type = GizmoType::ROTATE;
			else if (m_gizmo_type == GizmoType::ROTATE)
				m_gizmo_type = GizmoType::SCALE;
			else if (m_gizmo_type == GizmoType::SCALE)
				m_gizmo_type = GizmoType::TRANSLATE;
		}
		case ASCII_L:
		{
			m_l_pressed = true;
			//pipeline.GetDefaultBufferTexture().saveToFile("PrintScreen.png");
		}
	return true;
	default: return false;
	}
}

//--------------------------------------------------------------------------
bool eMainContextBase::OnMouseMove(int32_t x, int32_t y)
{
	if (GetMainCamera().getCameraRay().IsPressed())
	{
		if (m_input_strategy && !m_input_strategy->OnMouseMove(x, y) && m_framed_choice_enabled)
		{
			// input strategy has priority over frame, @todo frmae should be inside one of input strategies
			m_framed.reset(new std::vector<shObject>(GetMainCamera().getCameraRay().onMove(GetMainCamera(), m_objects, static_cast<float>(x), static_cast<float>(y)))); 	//to draw a frame
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------
bool eMainContextBase::OnMousePress(int32_t x, int32_t y, bool left)
{
	if (m_framed)
		m_framed->clear();

	GetMainCamera().getCameraRay().Update(GetMainCamera(), static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
	GetMainCamera().getCameraRay().press(x, y);

	//should be inside input strategy which needs it(frame, moveXZ)
	GetMainCamera().MovementSpeedRef() = 0.f;

	if (left)
	{
		//Get Visible and Children
		auto [picked, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(m_objects);
		if (picked != m_focused)
			ObjectPicked.Occur(picked);
	}

	if (m_input_strategy)
		m_input_strategy->OnMousePress(x, y, left);

	return false;
}

//---------------------------------------------------------------------------------
bool eMainContextBase::OnMouseRelease()
{
	GetMainCamera().getCameraRay().release();
	if (m_input_strategy)
		m_input_strategy->OnMouseRelease();
	//should be inside input strategy which needs it(frame, moveXZ)
	GetMainCamera().MovementSpeedRef() = 0.05f;
	return true;
}

//--------------------------------------------------------------------------
void eMainContextBase::InitializeGL()
{
	//init main light
	m_lights.push_back({});
	//m_lights[0].light_position = vec4(0.0f, 4.0f, -1.0f, 1.0f);
	m_lights[0].light_position = vec4(0.5f, 2.0f, -4.0f, 1.0f);
	m_lights[0].light_direction = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	m_lights[0].type = eLightType::DIRECTION;

	//init main camera
	m_cameras.push_back(Camera(width, height, nearPlane, farPlane));
	m_cameras[0].setDirection(glm::vec3(0.6f, -0.10f, 0.8f));
	m_cameras[0].setPosition(glm::vec3(0.0f, 4.0f, -4.0f));
	//Camera Ray
	GetMainCamera().getCameraRay().init(width, height, nearPlane, farPlane); //@todo direction of camera normalization

	for (auto& gui : externalGui)
		m_input_controller->AddObserver(gui, MONOPOLY);

	InitializeTextures();

	InitializePipline();

	InitializeBuffers();

	InitializeSounds();

	_PreInitModelManager();

	InitializeRenders();

	m_gameState = GameState::LOADING;

	InitializeModels();

  InitializeExternalGui();

	InitializeScripts();

	m_global_clock.start();
	
	m_gameState = GameState::LOADED;
}

//-------------------------------------------------------------------------------
void eMainContextBase::PaintGL()
{
	if (m_gameState == GameState::LOADED)
	{
		int64_t tick = m_global_clock.newFrame();
		std::map<eObject::RenderType, std::vector<shObject>> objects;
		std::vector<shObject> phong, pbr, flags, bezier, geometry;

		if(!m_texts.empty())
			m_texts[0]->content = { "FPS " + std::to_string(1000 / tick) };

		float msc = static_cast<float>(tick);
		for (auto& script : m_global_scripts)
		{
			script->Update(msc);
		}

		for (auto& object : m_objects)
		{
			if (!object->IsVisible())
				continue;

			if (object->GetScript())
				object->GetScript()->Update(msc);

			for (auto& child : object->GetChildrenObjects())
			{
				if (child->GetRenderType() == eObject::RenderType::PBR)
					pbr.push_back(child);
				else if (child->GetRenderType() == eObject::RenderType::PHONG)
					phong.push_back(child);
				else if (child->GetRenderType() == eObject::RenderType::FLAG)
					flags.push_back(child);
			}

			if (object->GetRenderType() == eObject::RenderType::BEZIER_CURVE)
				bezier.push_back(object);
			else if (object->GetRenderType() == eObject::RenderType::GEOMETRY)
				geometry.push_back(object);
			else if (object->GetRenderType() == eObject::RenderType::PBR)
				pbr.push_back(object);
			else if (object->GetRenderType() == eObject::RenderType::PHONG)
				phong.push_back(object);
			else if (object->GetRenderType() == eObject::RenderType::FLAG)
				flags.push_back(object);
		}

		if(m_input_strategy)
			m_input_strategy->UpdateInRenderThread();

		if (m_light_object)
		{
			m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
			GetMainLight().light_direction = -GetMainLight().light_position;
			phong.push_back(m_light_object);
		}

		//need better design less copying
		std::shared_ptr<std::vector<shObject>> focused_output = m_framed;
		if (!focused_output || !(focused_output->size() > 1))
			focused_output = m_focused ? std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{ m_focused })
			: std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{});

		objects.insert({ eObject::RenderType::PHONG, phong });
		objects.insert({ eObject::RenderType::OUTLINED, *focused_output });
		objects.insert({ eObject::RenderType::FLAG, flags });
		objects.insert({ eObject::RenderType::PBR, pbr });
		objects.insert({ eObject::RenderType::BEZIER_CURVE, bezier });
		objects.insert({ eObject::RenderType::GEOMETRY, geometry });

		pipeline.RenderFrame(objects, GetMainCamera(), GetMainLight(), m_guis, m_texts);

		if (m_l_pressed)
		{
			Texture t;
			t = pipeline.GetDefaultBufferTexture();
			t.saveToFile("PrintScreen.png");
			m_l_pressed = false;
		}
	}
	else if (m_gameState == GameState::LOADING)
	{
		//welcome texture(s)
	}
}

//--------------------------------------------------------------------------------
uint32_t eMainContextBase::GetFinalImageId()
{
	return pipeline.GetDefaultBufferTexture().id;
}

//---------------------------------------------------------------------------------
std::shared_ptr<eObject> eMainContextBase::GetFocusedObject()
{
	return m_focused;
}

//--------------------------------------------------------------------------------
void eMainContextBase::AddObject(std::shared_ptr<eObject> _object)
{
	ObjectBeingAddedToScene.Occur(_object);
	m_objects.push_back(_object);
}

//--------------------------------------------------------------------------------
void eMainContextBase::DeleteObject(std::shared_ptr<eObject> _object)
{
	ObjectBeingDeletedFromScene.Occur(_object); // check if its on scene
	m_objects.erase(std::remove(m_objects.begin(), m_objects.end(),(_object)));
}

//--------------------------------------------------------------------------------
void eMainContextBase::SetFocused(std::shared_ptr<eObject> _newFocused)
{
	auto old_focused = m_focused;
	m_focused = _newFocused;
	FocusChanged.Occur(old_focused, _newFocused);
}

//--------------------------------------------------------------------------------
void eMainContextBase::SetFocused(const eObject* _newFocused)
{
	for (shObject object : m_objects)
	{
		if (object.get() == _newFocused)
		{
			auto old_focused = m_focused;
			m_focused = object;
			FocusChanged.Occur(old_focused, object);
		}
	}
}

//--------------------------------------------------------------------------------
void eMainContextBase::AddInputObserver(IInputObserver* _observer, ePriority _priority)
{
	m_input_controller->AddObserver(_observer, _priority);
}

//-------------------------------------------------------------------------------
void eMainContextBase::DeleteInputObserver(IInputObserver* _observer)
{
	m_input_controller->DeleteObserver(_observer);
}

//--------------------------------------------------------------------------------
const Texture* eMainContextBase::GetTexture(const std::string& _name) const
{
	return texManager->Find(_name);
}

//--------------------------------------------------------------------------------
glm::mat4 eMainContextBase::GetMainCameraViewMatrix()
{
	return m_cameras[0].getWorldToViewMatrix();
}

//--------------------------------------------------------------------------------
glm::mat4 eMainContextBase::GetMainCameraProjectionMatrix()
{
	return m_cameras[0].getProjectionMatrix();
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializePipline()
{
	pipeline.Initialize();
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeModels()
{
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
	// set uniforms
	// exposure, shininess etc. @todo dont change every frame in render
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeTextures()
{
	texManager->InitContext(assetsFolderPath);
	texManager->LoadAllTextures();
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeScripts()
{
	std::shared_ptr<Text> t =std::make_shared<Text>();
	t->font = "ARIALN";
	t->pos_x = 25.0f;
	t->pos_y = 25.0f;
	t->scale = 1.0f;
	t->color = glm::vec3(0.8, 0.8f, 0.0f);
	t->mvp = glm::ortho(0.0f, (float)width, 0.0f, (float)height);
	m_texts.push_back(t);

	for (auto& script : m_global_scripts)
	{
		script->Initialize();
	}
	for (auto& object : m_objects)
	{
		if(object->GetScript())
			object->GetScript()->Initialize();
	}
	m_input_controller->AddObserver(this, STRONG);
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeExternalGui()
{
	// Lights & Cameras
	externalGui[0]->Add(TEXT, "Light", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light position.", &GetMainLight().light_position);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light direction.", &GetMainLight().light_direction);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light intensity.", &GetMainLight().intensity);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light ambient.", &GetMainLight().ambient);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light diffuse.", &GetMainLight().diffuse);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light specular.", &GetMainLight().specular);
	externalGui[0]->Add(SLIDER_FLOAT, "Light constant", &GetMainLight().constant);
	externalGui[0]->Add(SLIDER_FLOAT, "Light linear", &GetMainLight().linear);
	externalGui[0]->Add(SLIDER_FLOAT, "Light quadratic", &GetMainLight().quadratic);
	externalGui[0]->Add(SLIDER_FLOAT, "Light cut off", &GetMainLight().cutOff);
	externalGui[0]->Add(SLIDER_FLOAT, "Light outer cut off", &GetMainLight().outerCutOff);
	externalGui[0]->Add(TEXT, "Camera", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_3, "position", &GetMainCamera().PositionRef());
	externalGui[0]->Add(SLIDER_FLOAT_3, "direction", &GetMainCamera().ViewDirectionRef());

	//Pipeline
	externalGui[1]->Add(CHECKBOX, "Show bounding boxes", &pipeline.GetBoundingBoxBoolRef());
	externalGui[1]->Add(CHECKBOX, "Use Multi sampling", &pipeline.GetMultiSamplingBoolRef());
	externalGui[1]->Add(CHECKBOX, "Sky box", &pipeline.GetSkyBoxOnRef());
	externalGui[1]->Add(CHECKBOX, "Water", &pipeline.GetWaterOnRef());
	externalGui[1]->Add(CHECKBOX, "Hex", &pipeline.GetGeometryOnRef());
	externalGui[1]->Add(CHECKBOX, "Kernel", &pipeline.GetKernelOnRef());
	externalGui[1]->Add(CHECKBOX, "Sky noise", &pipeline.GetSkyNoiseOnRef());
	externalGui[1]->Add(CHECKBOX, "Use gizmo", &m_use_guizmo);
	externalGui[1]->Add(CHECKBOX, "Outline focused", &pipeline.GetOutlineFocusedRef());
	externalGui[1]->Add(CHECKBOX, "Gamma Correction", &pipeline.GetGammaCorrectionRef());
	externalGui[1]->Add(CHECKBOX, "Gamma Tone Mapping", &pipeline.GetToneMappingRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Gamma Exposure", &pipeline.GetExposureRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Blur coefficients", &pipeline.GetBlurCoefRef());
	externalGui[1]->Add(CHECKBOX, "SSAO", &pipeline.GetSSAOEnabledRef());
	
	std::function<void()> emit_partilces_callback = [this]()
	{
		auto sys = std::make_shared<ParticleSystem>(50, 0, 0, 10000, glm::vec3(0.0f, 3.0f, -2.5f),
																								texManager->Find("Tatlas2"),
																								soundManager->GetSound("shot_sound"),
																								texManager->Find("Tatlas2")->numberofRows);
		sys->Start();
		pipeline.AddParticleSystem(sys);
	};
	std::function<void()> emit_partilces_gpu_callback = [this]()
	{
		pipeline.AddParticleSystemGPU(glm::vec3(0.5f, 3.0f, -2.5f), texManager->Find("Tatlas2"));
	};
	std::function<void()> update_uniforms_callback = [this]()
	{
		pipeline.UpdateShadersInfo();
	};

	externalGui[1]->Add(BUTTON, "Emit particle system", (void*)&emit_partilces_callback);
	externalGui[1]->Add(BUTTON, "Emit particle system gpu", (void*)&emit_partilces_gpu_callback);
	externalGui[1]->Add(CHECKBOX, "Debug white", &pipeline.GetDebugWhite());
	externalGui[1]->Add(CHECKBOX, "Debug Tex Coords", &pipeline.GetDebugTexCoords());

	externalGui[1]->Add(SLIDER_FLOAT, "PBR debug dist", (void*)&pipeline.debug_float[0]);
	externalGui[1]->Add(SLIDER_FLOAT, "PBR debug intensity", (void*)&pipeline.debug_float[1]);
	externalGui[1]->Add(SLIDER_FLOAT, "PBR debug shininess", (void*)&pipeline.debug_float[2]);
	externalGui[1]->Add(SLIDER_FLOAT, "PBR debug ao", (void*)&pipeline.debug_float[3]);

	externalGui[1]->Add(TEXTURE, "Reflection buffer", (void*)pipeline.GetReflectionBufferTexture().id);
	externalGui[1]->Add(TEXTURE, "Refraction buffer", (void*)pipeline.GetRefractionBufferTexture().id);
	if (GetMainLight().type == eLightType::DIRECTION)
		externalGui[1]->Add(TEXTURE, "Shadow buffer directional", (void*)pipeline.GetShadowBufferTexture().id);
	else
		externalGui[1]->Add(TEXTURE, "Shadow buffer point", (void*)pipeline.GetShadowBufferTexture().id);
	externalGui[1]->Add(TEXTURE, "Gaussian buffer", (void*)pipeline.GetGausian2BufferTexture().id);
	externalGui[1]->Add(TEXTURE, "Bright filter buffer", (void*)pipeline.GetBrightFilter().id);
	externalGui[1]->Add(TEXTURE, "SSAO buffer", (void*)pipeline.GetSSAO().id);
	externalGui[1]->Add(TEXTURE, "Deffered Pos", (void*)pipeline.GetDefferedOne().id);
	externalGui[1]->Add(TEXTURE, "Deffered Norm", (void*)pipeline.GetDefferedTwo().id);

	//Objects transform
	externalGui[2]->Add(OBJECT_REF_TRANSFORM, "Transform", (void*)&m_focused);

	//Shaders
	externalGui[3]->Add(BUTTON, "Update shaders", (void*)&update_uniforms_callback);
	externalGui[3]->Add(SHADER, "Shaders", (void*)&pipeline.GetShaderInfos());

	//Main Menu
	static std::function<void(const std::string&)> add_model_callback = [this](const std::string& _path)
	{
		modelManager->Add("Name", (GLchar*)_path.c_str());//@todo parse real name
	};
	externalGui[4]->Add(MENU_OPEN, "Add model", reinterpret_cast<void*>(&add_model_callback));

	static std::function<void(const std::string&)> serealize_scene_callback = [this](const std::string& _path)
	{
		SceneSerializer serealizer(GetObjects(), *modelManager.get(), *animationManager.get());
		serealizer.Serialize(_path);
	};
	externalGui[4]->Add(MENU_SAVE_SCENE, "Serealize scene", reinterpret_cast<void*>(&serealize_scene_callback));
	
	static std::function<void(const std::string&)> deserealize_scene_callback = [this](const std::string& _path)
	{
		m_objects.clear();
		SceneSerializer serealizer(GetObjects(), *modelManager.get(), *animationManager.get());
		m_objects = serealizer.Deserialize(_path);
	};
	externalGui[4]->Add(MENU_OPEN_SCENE, "Deserealize scene", reinterpret_cast<void*>(&deserealize_scene_callback));

	//Create
	std::function<void()> create_cube_callbaack = [this]()
	{
		ObjectFactoryBase factory;
		shObject cube = factory.CreateObject(std::make_shared<MyModel>(modelManager->FindMesh("cube"), "default_cube"), eObject::RenderType::PHONG, "DefaultCube");
		m_objects.push_back(cube);
	};
	externalGui[5]->Add(BUTTON, "Cube", (void*)&create_cube_callbaack);

	std::function<void()> create_sphere_callbaack = [this]()
	{
		ObjectFactoryBase factory;
		shObject sphere = factory.CreateObject(std::make_shared<MyModel>(modelManager->FindMesh("sphere"), "default_sphere"), eObject::RenderType::PHONG, "DefaultSphere");
		m_objects.push_back(sphere);
	};
	externalGui[5]->Add(BUTTON, "Sphere", (void*)&create_sphere_callbaack);

	std::function<void()> create_plane_callbaack = [this]()
	{
		ObjectFactoryBase factory;
		shObject plane = factory.CreateObject(std::make_shared<MyModel>(modelManager->FindMesh("plane"), "default_plane"), eObject::RenderType::PHONG, "DefaultSphere");
		m_objects.push_back(plane);
	};
	externalGui[5]->Add(BUTTON, "Plane", (void*)&create_plane_callbaack);

	std::function<void()> create_bezier_callbaack = [this]()
	{
		dbb::Bezier bezier;
		bezier.p0 = { -0.85f, -0.75f, 0.0f };
		bezier.p1 = { -0.45f, -0.33f, 0.0f };
		bezier.p2 = {  0.17f,  0.31f, 0.0f };
		bezier.p3 = {  0.55f,  0.71f, 0.0f };

		ObjectFactoryBase factory;
		shObject bezier_model = factory.CreateObject(std::make_shared<BezierCurveModel>(new BezierCurveMesh(bezier, /*2d*/true)), eObject::RenderType::BEZIER_CURVE);
		m_objects.push_back(bezier_model);

		for (int i = 0; i < 4; ++i)
		{
			shObject pbr_sphere = factory.CreateObject(modelManager->Find("sphere_red"), eObject::RenderType::PBR, "SphereBezierPBR " + std::to_string(i));
			bezier_model->GetChildrenObjects().push_back(pbr_sphere);
			pbr_sphere->Set2DScreenSpace(true);
		}
		bezier_model->SetScript(new BezierCurveUIController(this, bezier_model, 0.02f, texManager->Find("pseudo_imgui")));
		m_input_strategy.reset(new InputStrategy2DMove(this));
	};
	externalGui[5]->Add(BUTTON, "Bezier Curve 2D", (void*)&create_bezier_callbaack);

	std::function<void()> create_bezier_callbaack_3d = [this]()
	{
		dbb::Bezier bezier;
		bezier.p0 = { 1.0f, 3.0f, 0.0f };
		bezier.p1 = { 3.0f, 3.0f, 3.0f };
		bezier.p2 = { 4.2f, 3.0f, -2.5f };
		bezier.p3 = { 8.0f, 3.0f, 1.0f };

		ObjectFactoryBase factory;
		shObject bezier_model = factory.CreateObject(std::make_shared<BezierCurveModel>(new BezierCurveMesh(bezier, /*2d*/false)), eObject::RenderType::BEZIER_CURVE);
		m_objects.push_back(bezier_model);

		for (int i = 0; i < 4; ++i)
		{
			shObject pbr_sphere = factory.CreateObject(modelManager->Find("sphere_red"), eObject::RenderType::PBR, "SphereBezierPBR " + std::to_string(i));
			bezier_model->GetChildrenObjects().push_back(pbr_sphere);
		}
		bezier_model->SetScript(new BezierCurveUIController(this, bezier_model, 0.1f));
		m_input_strategy.reset(new InputStrategyMoveAlongXZPlane(GetMainCamera(), GetObjectsWithChildren(m_objects)));
	};
	externalGui[5]->Add(BUTTON, "Bezier Curve 3D", (void*)&create_bezier_callbaack_3d);

	//Object List
	externalGui[6]->Add(OBJECT_LIST, "Objects List", (void*)this);

	//Objects material
	externalGui[7]->Add(OBJECT_REF_MATERIAL, "Material", (void*)&m_focused);
	//Objects rigger
	externalGui[8]->Add(OBJECT_REF_RIGGER, "Rigger", (void*)&m_focused);

	//Console
	static std::function<void(const std::string&)> console_plane_callbaack = [this](const std::string& _commandLine)
	{
		std::cout << _commandLine << std::endl;
		// Parse _commandLine and call the function (Set uniform, set script, set material etc.)
		std::string parsed, input = _commandLine;
		std::stringstream input_stringstream(input);
		std::vector<std::string> res;
		while (std::getline(input_stringstream, parsed, ' '))
		{
			res.push_back(parsed);
		}
		if(res.size() == 4)
			pipeline.SetUniformData(res[0] + " " + res[1], res[2], std::stof(res[3]));
		else if(res.size() == 5)
			pipeline.SetUniformData(res[0] + " " + res[1], res[2], glm::vec2(std::stof(res[3]), std::stof(res[4])));
		else if(res.size() == 6)
			pipeline.SetUniformData(res[0] + " " + res[1], res[2], glm::vec3(std::stof(res[3]), std::stof(res[4]), std::stof(res[5])));
	};
	externalGui[9]->Add(CONSOLE, "Console", reinterpret_cast<void*>(&console_plane_callbaack));

	m_global_scripts.push_back(std::make_shared<ParticleSystemToolController>(externalGui[10], texManager.get(), soundManager.get(), pipeline));
	m_global_scripts.push_back(std::make_shared<TerrainGeneratorTool>(externalGui[11]));
}

//------------------------------------------------------------
void eMainContextBase::_PreInitModelManager()
{
	modelManager->InitializePrimitives();
	//PRIMITIVES
	modelManager->AddPrimitive("wall_cube",
		std::make_shared<MyModel>(modelManager->FindMesh("cube"),
															"wall_cube",
															texManager->Find("Tbrickwall0_d"),
															texManager->Find("Tbrickwall0_d"),
															texManager->Find("Tbrickwall0_n"),
															&Texture::GetTexture1x1(BLACK)));
	modelManager->AddPrimitive("container_cube",
		std::make_shared<MyModel>(modelManager->FindMesh("cube"),
															"container_cube",
															texManager->Find("Tcontainer0_d"),
															texManager->Find("Tcontainer0_s"),
															&Texture::GetTexture1x1(BLUE),
															&Texture::GetTexture1x1(BLACK)));
	modelManager->AddPrimitive("arrow",
		std::make_shared<MyModel>(modelManager->FindMesh("arrow"),
															"arrow",
															texManager->Find("Tcontainer0_d"),
															texManager->Find("Tcontainer0_s"),
															&Texture::GetTexture1x1(BLUE),
															&Texture::GetTexture1x1(BLACK)));
	modelManager->AddPrimitive("grass_plane",
		std::make_shared<MyModel>(modelManager->FindMesh("plane"),
															"grass_plane",
															texManager->Find("Tgrass0_d"),
															texManager->Find("Tgrass0_d"),
															&Texture::GetTexture1x1(BLUE),
															&Texture::GetTexture1x1(BLACK)));
	modelManager->AddPrimitive("white_cube",
		std::make_shared<MyModel>(modelManager->FindMesh("cube"),
															"white_cube",
															&Texture::GetTexture1x1(WHITE)));
	modelManager->AddPrimitive("brick_square",
		std::make_shared<MyModel>(modelManager->FindMesh("square"),
															"brick_square",
															texManager->Find("Tbricks0_d"),
															texManager->Find("Tbricks0_d"),
															&Texture::GetTexture1x1(BLUE),
															&Texture::GetTexture1x1(BLACK)));
	modelManager->AddPrimitive("brick_cube",
		std::make_shared<MyModel>(modelManager->FindMesh("cube"),
															"brick_cube",
															texManager->Find("Tbricks2_d"),
															texManager->Find("Tbricks2_d"),
															texManager->Find("Tbricks2_n"),
															texManager->Find("Tbricks2_dp")));
	modelManager->AddPrimitive("pbr_cube",
		std::make_shared<MyModel>(modelManager->FindMesh("cube"),
														"pbr_cube",
														texManager->Find("pbr1_basecolor"),
														texManager->Find("pbr1_metallic"),
														texManager->Find("pbr1_normal"),
														texManager->Find("pbr1_roughness")));
	modelManager->AddPrimitive("white_sphere",
		std::make_shared<MyModel>(modelManager->FindMesh("sphere"),
														"white_sphere",
														&Texture::GetTexture1x1(WHITE)));
}

//------------------------------------------------------------
Light& eMainContextBase::GetMainLight()
{
	if (m_lights.empty())
		throw std::logic_error("main light was deleted!");

	return m_lights[0];
}

//------------------------------------------------------------------
Camera& eMainContextBase::GetMainCamera()
{
	if (m_cameras.empty())
		throw std::logic_error("main camera was deleted!");

	return m_cameras[0];
}

//--------------------------------------------------------------------------------
void eMainContextBase::InstallTcpServer()
{
	if (!tcpAgent)
	{
		if (dbb::NetWork::Initialize())
		{
			tcpAgent = std::make_unique<Server>();
			if (tcpAgent->Initialize(dbb::IPEndPoint{ "0.0.0.0", 8080 }))//134.238.94.205 //208.67.222.222
			{
				tcpTimer.reset(new math::Timer([this]()->bool
					{
						if (tcpAgent->IsConnected())
							tcpAgent->Frame();
						return true;
					}));
				tcpTimer->start(15); //~70 fps

				//this is test
				std::string msg;
				while (msg != "exit")
				{
					std::getline(std::cin, msg);
					//std::cout << "Me: " << msg << std::endl;
					tcpAgent->SendMsg(std::move(msg));
				}
			}
			dbb::NetWork::Shutdown();
		}
	}
}

//--------------------------------------------------------------------------------
void eMainContextBase::InstallTcpClient()
{
	if (!tcpAgent)
	{
		if (dbb::NetWork::Initialize())
		{
			tcpAgent = std::make_unique<Client>();
			if (tcpAgent->Initialize(dbb::IPEndPoint{ "127.0.0.1", 8080 })) //"109.95.50.27 // "192.168.2.102 //134.238.94.205 /208.67.222.222
			{
				tcpTimer.reset(new math::Timer([this]()->bool
					{
						if (tcpAgent->IsConnected())
							tcpAgent->Frame();
						return true;
					}));
				tcpTimer->start(15); //~70 fps

				//this is test
				std::string msg;
				while (msg != "exit")
				{
					std::getline(std::cin, msg);
					//std::cout << "Me: " << msg << std::endl;
					tcpAgent->SendMsg(std::move(msg));
				}
			}
			dbb::NetWork::Shutdown();
		}
	}
}

//----------------------------------------------------------------
void eMainContextBase::AddGUI(const std::shared_ptr<GUI>& _gui)
{
	m_guis.push_back(_gui);
}

//----------------------------------------------------------------
void eMainContextBase::DeleteGUI(const std::shared_ptr<GUI>& _gui)
{
	m_guis.erase(std::remove(m_guis.begin(), m_guis.end(), (_gui)));
}

//----------------------------------------------------------------
void eMainContextBase::AddText(std::shared_ptr<Text> _text)
{
	m_texts.push_back(_text);
}

//----------------------------------------------------------------
std::vector<std::shared_ptr<Text>>& eMainContextBase::GetTexts()
{
	return m_texts;
}

