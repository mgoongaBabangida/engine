#include "stdafx.h"

#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "SceneSerializer.h"
#include "ModelManagerYAML.h"
#include "AnimationManagerYAML.h"
#include "ParticleSystemToolController.h"
#include "TerrainGeneratorTool.h"
#include "BezierCurveUIController.h"
#include "CameraSecondScript.h"
#include <base/InputController.h>

#include <tcp_lib/Network.h>
#include <tcp_lib/Server.h>
#include <tcp_lib/Client.h>

#include <opengl_assets/TextureManager.h>
#include <opengl_assets/SoundManager.h>
#include <opengl_assets/Sound.h>

#include <sdl_assets/ImGuiContext.h>

#include <math/Clock.h>
#include <math/Rigger.h>
#include <math/ParticleSystem.h>

#include <thread>
#include <sstream>
#include <fstream>

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
, pipeline(1200, 600) //@todo should get from outside
{
	m_cameras.reserve(8); //@todo redesign
}

//-------------------------------------------------------------------------
eMainContextBase::~eMainContextBase()
{
	m_global_scripts.clear();
}

//-------------------------------------------------------------------------
uint32_t eMainContextBase::Width() const { return pipeline.Width(); }
//-------------------------------------------------------------------------
uint32_t eMainContextBase::Height()  const { return pipeline.Height(); }

//*********************InputObserver*********************************
//--------------------------------------------------------------------------
bool eMainContextBase::OnKeyPress(uint32_t _asci, KeyModifiers _modifier)
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
			return true;
		}
		case ASCII_L:
		{
			m_l_pressed = true;
			//pipeline.GetDefaultBufferTexture().saveToFile("PrintScreen.png");
			return true;
		}
	default: return false;
	}
}

//--------------------------------------------------------------------------
bool eMainContextBase::OnMouseMove(int32_t x, int32_t y, KeyModifiers _modifier)
{
	if (m_update_hovered)
	{
		GetMainCamera().getCameraRay().Update(static_cast<float>(x), static_cast<float>(y));
		GetMainCamera().getCameraRay().press((float)x, (float)y);
		auto [picked, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(m_objects);
		m_hovered = picked;
	}

	if (GetMainCamera().getCameraRay().IsPressed())
	{
		if ((!m_input_strategy || (m_input_strategy && !m_input_strategy->OnMouseMove(x, y))) && m_framed_choice_enabled)
		{
			// input strategy has priority over frame, @todo frmae should be inside one of input strategies
			m_framed = std::make_shared<std::vector<shObject>>(GetMainCamera().getCameraRay().onMove(m_objects, static_cast<float>(x), static_cast<float>(y))); 	//to draw a frame
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------
bool eMainContextBase::OnMousePress(int32_t x, int32_t y, bool _left, KeyModifiers _modifier)
{
	if (_modifier == KeyModifiers::SHIFT)
		return true;

	if (m_framed)
		m_framed->clear();

	GetMainCamera().getCameraRay().Update(static_cast<float>(x), static_cast<float>(y));
	GetMainCamera().getCameraRay().press(x, y);

	//should be inside input strategy which needs it(frame, moveXZ)
	GetMainCamera().MovementSpeedRef() = 0.f;

	//Get Visible and Children
	auto [picked, intersaction] = GetMainCamera().getCameraRay().calculateIntersaction(m_objects);
	if (picked != m_focused)
		ObjectPicked.Occur(picked, _left);

	if (m_input_strategy)
		m_input_strategy->OnMousePress(x, y, _left);

	return false;
}

//---------------------------------------------------------------------------------
bool eMainContextBase::OnMouseRelease(KeyModifiers _modifier)
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
	{
		//init main light
		m_lights.push_back({});
		//m_lights[0].light_position = vec4(0.0f, 4.0f, -1.0f, 1.0f);
		m_lights[0].light_position = vec4(0.5f, 2.0f, -4.0f, 1.0f);
		m_lights[0].light_direction = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		m_lights[0].type = eLightType::DIRECTION;

		//init main camera
		m_cameras.emplace_back(pipeline.Width(), pipeline.Height(), 0.1f, 20.0f);
		m_cameras[0].setDirection(glm::vec3(0.6f, -0.10f, 0.8f));
		m_cameras[0].setPosition(glm::vec3(0.0f, 4.0f, -4.0f));
	}

	for (auto& gui : externalGui)
		m_input_controller->AddObserver(gui, MONOPOLY);

	InitializeTextures();

	InitializeBuffers();

	InitializeSounds();

	_PreInitModelManager();

	InitializeRenders();

	InitializePipline();

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
		std::vector<shObject> phong, pbr, flags, bezier, geometry, lines, arealighted;

		if(!m_texts.empty() && m_show_fps)
			m_texts[0]->content = { "FPS " + std::to_string(1000 / tick) };

		float msc = static_cast<float>(tick);
		for (auto script : m_global_scripts)
		{
			script->Update(msc);
		}

		//@todo this will be slow when we have a lot of objects
		for (shObject object : m_objects)
		{
			if(object.get())
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
				else if (object->GetRenderType() == eObject::RenderType::LINES)
					lines.push_back(object);
				else if(object->GetRenderType() == eObject::RenderType::AREA_LIGHT_ONLY)
					arealighted.push_back(object);
			}
		}

		if(m_input_strategy)
			m_input_strategy->UpdateInRenderThread();

		//@todo to be transfered to some other update
		if (m_light_object)
		{
			m_light_object->GetTransform()->setTranslation(GetMainLight().light_position);
			phong.push_back(m_light_object);
		}

		if (GetMainLight().type == eLightType::DIRECTION || GetMainLight().type == eLightType::CSM)
			GetMainLight().light_direction = -GetMainLight().light_position;

		//@todo need better design less copying
		std::shared_ptr<std::vector<shObject>> focused_output = m_framed;
		if (!focused_output)
			focused_output = m_focused ? std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{ m_focused })
			: std::shared_ptr<std::vector<shObject>>(new std::vector<shObject>{});

		objects.insert({ eObject::RenderType::PHONG, phong });
		objects.insert({ eObject::RenderType::OUTLINED, *focused_output });
		objects.insert({ eObject::RenderType::FLAG, flags });
		objects.insert({ eObject::RenderType::PBR, pbr });
		objects.insert({ eObject::RenderType::BEZIER_CURVE, bezier });
		objects.insert({ eObject::RenderType::GEOMETRY, geometry });
		objects.insert({ eObject::RenderType::LINES, lines });
		objects.insert({ eObject::RenderType::AREA_LIGHT_ONLY, arealighted });

		pipeline.RenderFrame(objects, m_cameras, GetMainLight(), m_guis, m_texts);

		if (m_debug_csm)
		{
			pipeline.DumpCSMTextures();
			if (m_l_pressed) //@todo improve print screen
			{
				Texture t;
				t = pipeline.GetDefaultBufferTexture();
				t.saveToFile("PrintScreen.png");
				m_l_pressed = false;
			}
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
std::shared_ptr<eObject> eMainContextBase::GetHoveredObject()
{
	return m_hovered;
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
	if (auto it = std::remove(m_objects.begin(), m_objects.end(), _object); it != m_objects.end())
	{
		ObjectBeingDeletedFromScene.Occur(_object);
		m_objects.erase(it);
	}
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
void eMainContextBase::SetFramed(const std::vector<shObject>& _framed)
{
	m_framed = std::make_shared<std::vector<shObject>>(_framed);
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
	return m_cameras[m_cur_camera].getWorldToViewMatrix();
}

//--------------------------------------------------------------------------------
glm::mat4 eMainContextBase::GetMainCameraProjectionMatrix()
{
	return m_cameras[m_cur_camera].getProjectionMatrix();
}

//--------------------------------------------------------------------------------
glm::vec3 eMainContextBase::GetMainCameraPosition() const
{
	return m_cameras[m_cur_camera].getPosition();
}

//-------------------------------------------------------------------------------
glm::vec3 eMainContextBase::GetMainCameraDirection() const
{
	return m_cameras[m_cur_camera].getDirection();
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializePipline()
{
	pipeline.Initialize();

	std::ifstream infile("pipeline.ini");
	if (infile.is_open())
	{
		std::stringstream sstream;
		std::copy(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sstream));

		std::string name, value, end;
		while (!sstream.eof())
		{
			sstream >> name;
			sstream >> value;
			sstream >> end;

			if (name == "SwitchSkyBox")
				value == "true" ? pipeline.SwitchSkyBox(true) : pipeline.SwitchSkyBox(false);
			else if (name == "SwitchWater")
				value == "true" ? pipeline.SwitchWater(true) : pipeline.SwitchWater(false);
			else if (name == "SkyNoiseOn")
				value == "true" ? pipeline.GetSkyNoiseOnRef() = true : pipeline.GetSkyNoiseOnRef() = false;
			else if (name == "KernelOn")
				value == "true" ? pipeline.GetKernelOnRef() = true : pipeline.GetKernelOnRef() = false;
			else if (name == "UseGuizmo")
				value == "true" ? m_use_guizmo = true : m_use_guizmo = false;
			else if(name == "RotateSkyBox")
				value == "true" ? pipeline.GetRotateSkyBoxRef() = true : pipeline.GetRotateSkyBoxRef() = false;
		}
	}
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeBuffers()
{
	pipeline.InitializeBuffers();
}

//--------------------------------------------------------------------------------
void eMainContextBase::InitializeModels()
{
	// Camera should be here in editor mode @todo ifdef EDITOR
	modelManager->Add("Camera", (GLchar*)std::string(modelFolderPath + "Camera_v2/Camera_v2.obj").c_str());

	std::ifstream infile("models.ini");
	if (infile.is_open())
	{
		std::stringstream sstream;
		std::copy(std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>(),
			std::ostreambuf_iterator<char>(sstream));

		std::string file_name, name, end;
		while (!sstream.eof())
		{
			sstream >> file_name;
			sstream >> name;
			sstream >> end;
			if (end == "true")
			{
				modelManager->Add(name, (GLchar*)std::string(modelFolderPath + file_name).c_str(), true);
				sstream >> end;
			}
			else
				modelManager->Add(name, (GLchar*)std::string(modelFolderPath + file_name).c_str());
		}
	}
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
	texManager->Initialize();
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
	t->mvp = glm::ortho(0.0f, (float)pipeline.Width(), 0.0f, (float)pipeline.Height());
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

	std::function<void(size_t)> light_type_callback = [this](size_t _index) 
	{
		if(_index == 0)
			GetMainLight().type = eLightType::DIRECTION;
		else if(_index == 1)
			GetMainLight().type = eLightType::POINT;
		else if(_index == 2)
			GetMainLight().type = eLightType::SPOT;
		else if (_index == 3)
			GetMainLight().type = eLightType::CSM;
	};
	static eVectorStringsCallback light_types{ {"directional", "point", "cut-off", "csm"}, light_type_callback};
	externalGui[0]->Add(COMBO_BOX, "Light type.", &light_types);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light position.", &GetMainLight().light_position);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light direction.", &GetMainLight().light_direction);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light intensity.", &GetMainLight().intensity);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light ambient.", &GetMainLight().ambient);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light diffuse.", &GetMainLight().diffuse);
	externalGui[0]->Add(SLIDER_FLOAT_3, "Light specular.", &GetMainLight().specular);
	externalGui[0]->Add(TEXT, "Light constant", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_NERROW, "Constant", &GetMainLight().constant);
	externalGui[0]->Add(TEXT, "Light linear", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_NERROW, "Linear", &GetMainLight().linear);
	externalGui[0]->Add(TEXT, "Light quadratic", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_NERROW, "Quadratic", &GetMainLight().quadratic);
	externalGui[0]->Add(TEXT, "Light cut off", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_NERROW, "Cut off", &GetMainLight().cutOff);
	externalGui[0]->Add(TEXT, "Light outer cut off", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_NERROW, "Outer cut off", &GetMainLight().outerCutOff);
	externalGui[0]->Add(TEXT, "Camera", nullptr);
	externalGui[0]->Add(SLIDER_FLOAT_3, "position", &GetMainCamera().PositionRef());
	externalGui[0]->Add(SLIDER_FLOAT_3, "direction", &GetMainCamera().ViewDirectionRef());
	std::function<void()> add_camera_callback = [this]()
	{
		m_cameras.emplace_back(pipeline.Width(), pipeline.Height(), 0.1f, 10.0f);
		m_cameras.back().SetVisualiseFrustum(true);
		externalGui[0]->Add(GAME, "Game", (void*)&(*this));
		externalGui[0]->Add(CAMERA, "Camera second", &m_cameras.back());
		std::function<void()> switch_camera_callback = [this]()
		{
			if (m_cur_camera == 0)
				m_cur_camera = 1;
			else
				m_cur_camera = 0;
		};
		externalGui[0]->Add(BUTTON, "Switch Camera", &switch_camera_callback);
		Material material;
		material.albedo = glm::vec3(0.8f, 0.0f, 0.0f);
		material.ao = 1.0f;
		material.roughness = 0.5;
		material.metallic = 0.5;
		material.emissive_texture_id = Texture::GetTexture1x1(TColor::BLACK).id;
		material.use_albedo = false;
		material.use_metalic = false;
		material.use_roughness = false;
		material.use_normal = true;
		ObjectFactoryBase factory(animationManager.get());
		m_camera_obj = factory.CreateObject(modelManager->Find("Camera"), eObject::RenderType::PBR, "Camera1");
		m_camera_obj->SetScript(new CameraSecondScript(&m_cameras.back(), this));
		m_camera_obj->GetTransform()->setScale(glm::vec3{ 0.01f, 0.01f, 0.01f });
		for (auto& mesh : m_camera_obj->GetModel()->Get3DMeshes())
			const_cast<I3DMesh*>(mesh)->SetMaterial(material);
		m_objects.push_back(m_camera_obj);
	};
	externalGui[0]->Add(BUTTON, "Add camera", &add_camera_callback);

	//Pipeline
	externalGui[1]->Add(CHECKBOX, "Show bounding boxes", &pipeline.GetBoundingBoxBoolRef());
	externalGui[1]->Add(CHECKBOX, "Use Multi sampling", &pipeline.GetMultiSamplingBoolRef());
	externalGui[1]->Add(CHECKBOX, "Sky box on", &pipeline.GetSkyBoxOnRef());
	externalGui[1]->Add(CHECKBOX, "Rotate Sky box", &pipeline.GetRotateSkyBoxRef());
	static std::function<void(int)> change_skybox_callback = [this](int _skybox)
	{
		if (texManager->GetCubeMapIds().size() > _skybox)
		{
			const Texture* skybox = texManager->FindByID(texManager->GetCubeMapIds()[_skybox]);
			pipeline.SetSkyBoxTexture(skybox);
		}
	};
	externalGui[1]->Add(SPIN_BOX, "Sky box", (void*)&change_skybox_callback);
	static std::function<void(int)> change_ibl_callback = [this](int _ibl)
	{
		if (texManager->GetIBLIds().size() > _ibl)
		{
			auto[irr, prefilter] = texManager->GetIBLIds()[_ibl];
			pipeline.SetSkyIBL(irr, prefilter);
		}
	};
	externalGui[1]->Add(SPIN_BOX, "IBL Map", (void*)&change_ibl_callback);
	externalGui[1]->Add(CHECKBOX, "Water", &pipeline.GetWaterOnRef());
	externalGui[1]->Add(CHECKBOX, "Hex", &pipeline.GetGeometryOnRef());
	externalGui[1]->Add(CHECKBOX, "Kernel", &pipeline.GetKernelOnRef());
	externalGui[1]->Add(CHECKBOX, "Sky noise", &pipeline.GetSkyNoiseOnRef());
	externalGui[1]->Add(CHECKBOX, "Use gizmo", &m_use_guizmo);
	externalGui[1]->Add(CHECKBOX, "Outline focused", &pipeline.GetOutlineFocusedRef());
	externalGui[1]->Add(CHECKBOX, "Gamma Correction", &pipeline.GetGammaCorrectionRef());
	externalGui[1]->Add(CHECKBOX, "Gamma Tone Mapping", &pipeline.GetToneMappingRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Gamma Exposure", &pipeline.GetExposureRef());
	externalGui[1]->Add(SLIDER_FLOAT_NERROW, "Blur coefficients", &pipeline.GetBlurCoefRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Emission Strength", &pipeline.GetEmissionStrengthRef());
	externalGui[1]->Add(CHECKBOX, "SSAO", &pipeline.GetSSAOEnabledRef());
	externalGui[1]->Add(SLIDER_FLOAT, "SSAO Threshold", &pipeline.GetSaoThresholdRef());
	externalGui[1]->Add(SLIDER_FLOAT, "SSAO Strength", &pipeline.GetSaoStrengthRef());
	externalGui[1]->Add(CHECKBOX, "SSR", &pipeline.GetSSREnabledRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Step", &pipeline.Step());
	externalGui[1]->Add(SLIDER_FLOAT, "MinRayStep", &pipeline.MinRayStep());
	externalGui[1]->Add(SLIDER_FLOAT, "Metallic", &pipeline.Metallic());
	externalGui[1]->Add(SLIDER_INT_NERROW, "NumBinarySearchSteps", &pipeline.NumBinarySearchSteps());
	externalGui[1]->Add(SLIDER_FLOAT, "ReflectionSpecularFalloffExponent", &pipeline.ReflectionSpecularFalloffExponent());
	externalGui[1]->Add(SLIDER_FLOAT_LARGE, "K", &pipeline.K());
	externalGui[1]->Add(CHECKBOX, "Shadows", &pipeline.ShadowingRef());

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
	externalGui[1]->Add(TEXTURE, "Gaussian buffer", (void*)pipeline.GetGausian2BufferTexture().id);
	externalGui[1]->Add(TEXTURE, "Bright filter buffer", (void*)pipeline.GetBrightFilter().id);
	externalGui[1]->Add(TEXTURE, "SSAO buffer", (void*)pipeline.GetSSAO().id);
	externalGui[1]->Add(TEXTURE, "SSR buffer", (void*)pipeline.GetSSRTexture().id);
	externalGui[1]->Add(TEXTURE, "SSR buffer blur", (void*)pipeline.GetSSRWithScreenTexture().id);
	externalGui[1]->Add(TEXTURE, "SSR Mask", (void*)pipeline.GetSSRTextureScreenMask().id);
	externalGui[1]->Add(TEXTURE, "Deffered Pos", (void*)pipeline.GetDefferedOne().id);
	externalGui[1]->Add(TEXTURE, "Deffered Norm", (void*)pipeline.GetDefferedTwo().id);
	externalGui[1]->Add(TEXTURE, "LUT", (void*)pipeline.GetLUT().id);
	if (GetMainLight().type == eLightType::DIRECTION)
		externalGui[1]->Add(TEXTURE, "Shadow buffer directional", (void*)pipeline.GetShadowBufferTexture().id);
	else
		externalGui[1]->Add(TEXTURE, "Shadow buffer point", (void*)pipeline.GetShadowBufferTexture().id);
	if (m_debug_csm)
	{
		pipeline.DumpCSMTextures();
		externalGui[1]->Add(TEXTURE, "CSM 1", (void*)pipeline.GetCSMMapLayer1().id);
		externalGui[1]->Add(TEXTURE, "CSM 2", (void*)pipeline.GetCSMMapLayer2().id);
		externalGui[1]->Add(TEXTURE, "CSM 3", (void*)pipeline.GetCSMMapLayer3().id);
		externalGui[1]->Add(TEXTURE, "CSM 4", (void*)pipeline.GetCSMMapLayer4().id);
		externalGui[1]->Add(TEXTURE, "CSM 5", (void*)pipeline.GetCSMMapLayer5().id);
		externalGui[1]->Add(SLIDER_FLOAT, "Z mult", (void*)&pipeline.ZMult());
	}
	externalGui[1]->Add(CHECKBOX, "Physicly Based Bloom", &pipeline.PBBloomRef());
	externalGui[1]->Add(TEXTURE, "Bloom", (void*)pipeline.GetBloomTexture().id);

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

	//bezier 2d
	std::function<void()> create_bezier_callback = [this]()
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
	};
	externalGui[5]->Add(BUTTON, "Bezier Curve 2D", (void*)&create_bezier_callback);

	//bezier 3d
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
	};
	externalGui[5]->Add(BUTTON, "Bezier Curve 3D", (void*)&create_bezier_callbaack_3d);

	//Object List
	externalGui[6]->Add(OBJECT_LIST, "Objects List", (void*)this);

	//Objects material
	externalGui[7]->Add(OBJECT_REF_MATERIAL, "Material", (void*)&m_focused);

	//Objects rigger
	externalGui[8]->Add(GAME, "Game", (void*)&(*this));
	externalGui[8]->Add(OBJECT_REF_RIGGER, "Rigger", (void*)&m_focused);
	static std::function<void(shObject, const std::string&)> load_rigger = [this](shObject obj, const std::string& _path)
	{
		IRigger* rigger = animationManager->DeserializeRigger(_path);
		obj->SetRigger(rigger);
	};
	externalGui[8]->Add(ADD_CALLBACK, "Load Rigger", reinterpret_cast<void*>(&load_rigger));
	static std::function<void(shObject, const std::string&)> save_rigger = [this](shObject obj, const std::string& _path)
	{
		IRigger* rigger = obj->GetRigger();
		animationManager->SerializeRigger(dynamic_cast<const Rigger*>(rigger), _path);
	};
	externalGui[8]->Add(ADD_CALLBACK, "Save Rigger", reinterpret_cast<void*>(&save_rigger));

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

	m_global_scripts.push_back(std::make_shared<ParticleSystemToolController>(this, externalGui[10], modelManager.get(),texManager.get(), soundManager.get(), pipeline));
	//m_global_scripts.push_back(std::make_shared<TerrainGeneratorTool>(this, modelManager.get(), texManager.get(), pipeline, externalGui[11]));
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
	modelManager->AddPrimitive("white_quad",
		std::make_shared<MyModel>(modelManager->FindMesh("quad"),
															"white_quad",
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

	return m_cameras[m_cur_camera];
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
