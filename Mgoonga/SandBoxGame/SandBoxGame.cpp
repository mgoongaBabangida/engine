#include "stdafx.h"
#include "SandBoxGame.h"

#include <base/InputController.h>

#include <math/Rigger.h>
#include <math/BoxCollider.h>
#include <math/RigidBdy.h>
#include <math/ParticleSystem.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/GUI.h>
#include <opengl_assets/TextureManager.h>
#include <opengl_assets/ModelManager.h>
#include <opengl_assets/SoundManager.h>

#include <sdl_assets/ImGuiContext.h>

#include <game_assets/ObjectFactory.h>

#include "SandBoxScript.h"

//-------------------------------------------------------------------------
eSandBoxGame::eSandBoxGame(eInputController*  _input,
  std::vector<IWindowImGui*> _externalGui,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath,
						   const std::string& _shadersPath)
: eMainContextBase(_input, _externalGui, _modelsPath, _assetsPath, _shadersPath)
{
}

void eSandBoxGame::PaintGL()
{
	eMainContextBase::PaintGL();

	for(auto &object : m_objects)
	{
		if (object->GetScript())
			object->GetScript()->Update(m_objects);
	}

	if (m_lightObject)
	{
		m_lightObject->GetTransform()->setTranslation(GetMainLight().light_position);
		GetMainLight().light_direction = -GetMainLight().light_position;
	}

	std::map<eOpenGlRenderPipeline::RenderType, std::vector<shObject>> objects;
	objects.insert({ eOpenGlRenderPipeline::RenderType::PHONG, m_objects });
	objects.insert({ eOpenGlRenderPipeline::RenderType::OUTLINED, {} });
	objects.insert({ eOpenGlRenderPipeline::RenderType::FLAG, {} });
	objects.insert({ eOpenGlRenderPipeline::RenderType::PBR, {} });
	objects.insert({ eOpenGlRenderPipeline::RenderType::GEOMETRY, {} });
	objects.insert({ eOpenGlRenderPipeline::RenderType::BEZIER_CURVE, {} });

	pipeline.RenderFrame(objects, GetMainCamera(), GetMainLight(), guis);
}

//*********************InputObserver*********************************
bool eSandBoxGame::OnMouseMove(uint32_t x, uint32_t y)
{
	return false;
}

bool eSandBoxGame::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	case ASCII_G:
	{
		if (m_focused != nullptr)
			m_focused->GetScript()->OnKeyPress(ASCII_G);
	}
	return true;
	default: return false;
	}
}

bool eSandBoxGame::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	GetMainCamera().getCameraRay().Update(GetMainCamera(), x, y, static_cast<float>(width), static_cast<float>(height));
	if (left)
	{
		GetMainCamera().getCameraRay().press(x, y);
		m_focused = GetMainCamera().getCameraRay().calculateIntersaction(m_objects).first;
	}
	if (m_focused && m_focused->GetScript())
	{
		m_focused->GetScript()->OnMousePress(x, y, left);
	}
	return true;
}

bool eSandBoxGame::OnMouseRelease()
{
	GetMainCamera().getCameraRay().release();
	return true;
}

//*********************Initialize**************************************
void eSandBoxGame::InitializePipline()
{
	pipeline.Initialize();
	pipeline.SwitchSkyBox(false);
	pipeline.SwitchWater(false);
	pipeline.GetSkyNoiseOnRef() = false;
	pipeline.GetKernelOnRef() = false;
	this->m_use_guizmo = false;
	// call all the enable pipeline functions
}

void eSandBoxGame::InitializeBuffers()
{
	GetMainLight().type = eLightType::DIRECTION;
	pipeline.InitializeBuffers(GetMainLight().type == eLightType::POINT); //@todo add possibility to choose buffers
}

void eSandBoxGame::InitializeModels()
{
	eMainContextBase::InitializeModels();
	
	//MODELS
	modelManager->Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	
	modelManager->Add("Firing", (GLchar*)std::string(modelFolderPath + "Firing Rifle Soldier/Firing Rifle.dae").c_str());
	modelManager->Add("Dying", (GLchar*)std::string(modelFolderPath + "Dying Soldier/Dying.dae").c_str());
	modelManager->Add("Walking", (GLchar*)std::string(modelFolderPath + "Walking Soldier/Walking.dae").c_str());

	//OBJECTS
	ObjectFactoryBase factory;

	shObject wallCube = factory.CreateObject(modelManager->Find("wall_cube"), "WallCube");
	wallCube->GetTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	wallCube->SetScript(new eSandBoxScript());
	m_objects.push_back(wallCube);
	
	shObject grassPlane = factory.CreateObject(modelManager->Find("grass_plane"), "GrassPlane");
	grassPlane->GetTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_objects.push_back(grassPlane);

	shObject wolf = factory.CreateObject(modelManager->Find("wolf"), "Wolf");
	wolf->GetTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->GetTransform()->setTranslation(vec3(3.0f, -2.0f, 0.0f));
	wolf->GetTransform()->setScale(vec3(1.5f, 1.5f, 1.5f));
	wolf->SetRigger(new Rigger((Model*)modelManager->Find("wolf").get())); //@todo improve
	wolf->GetRigger()->ChangeName(std::string(), "Running");//@todo improve
	m_objects.push_back(wolf);

	//shObject firing = factory.CreateObject(modelManager->Find("Firing"), "Firing");
	//firing->GetTransform()->setTranslation(vec3(2.0f, -2.0f, 0.0f));
	//firing->SetRigger(new Rigger((Model*)modelManager->Find("Firing").get())); //@todo improve
	//firing->GetRigger()->ChangeName(std::string(), "Firing");//@todo improve
	//m_objects.push_back(firing);

	shObject dying = factory.CreateObject(modelManager->Find("Dying"), "Dying");
	dying->GetTransform()->setTranslation(vec3(1.0f, -2.0f, 0.0f));
	dying->GetTransform()->setScale(vec3(0.01f, 0.01f, 0.01f));
	Rigger* rigger = new Rigger((Model*)modelManager->Find("Dying").get());
	rigger->ChangeName(std::string(), "Dying");//@todo improve
	rigger->AddAnimations(dynamic_cast<eAnimatedModel*>(modelManager->Find("Firing").get())->Animations());
	rigger->ChangeName(std::string(), "Firing");
	rigger->AddAnimations(dynamic_cast<eAnimatedModel*>(modelManager->Find("Walking").get())->Animations());
	rigger->ChangeName(std::string(), "Walking");
	dying->SetRigger(rigger); //@todo improve
	m_objects.push_back(dying);

		//light
	m_lightObject = factory.CreateObject(modelManager->Find("white_sphere"), "WhiteSphere");
	m_lightObject->GetTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	m_lightObject->GetTransform()->setTranslation(GetMainLight().light_position);
	m_objects.push_back(m_lightObject);

	Texture* flag = texManager->Find("TSpanishFlag0_s");
	guis.emplace_back(new Cursor(0, 0, 30, 30, width, height));
	guis[0]->SetTexture(*flag, { 0,0 }, { flag->mTextureWidth, flag->mTextureHeight });

	inputController->AddObserver(guis[0].get(), WEAK);
	inputController->AddObserver(this, WEAK);
	inputController->AddObserver(&GetMainCamera().getCameraRay(), WEAK);
	inputController->AddObserver(&GetMainCamera(), WEAK);
	inputController->AddObserver(externalGui[0], MONOPOLY);
	inputController->AddObserver(externalGui[1], MONOPOLY);
	inputController->AddObserver(externalGui[2], MONOPOLY);
	inputController->AddObserver(externalGui[3], MONOPOLY);
}

void eSandBoxGame::InitializeRenders()
{
	pipeline.InitializeRenders(*modelManager.get(), *texManager.get(), shadersFolderPath);
}

void eSandBoxGame::InitializeSounds()
{
}

//**********************************External Gui*****************************
void eSandBoxGame::InitializeExternalGui()
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
	externalGui[1]->Add(CHECKBOX, "Gamma Correction", &pipeline.GetGammaCorrectionRef());
	externalGui[1]->Add(CHECKBOX, "Gamma Tone Mapping", &pipeline.GetToneMappingRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Gamma Exposure", &pipeline.GetExposureRef());
	externalGui[1]->Add(SLIDER_FLOAT, "Blur coefficients", &pipeline.GetBlurCoefRef());
	std::function<void()> emit_partilces_callback = [this]()
	{
		pipeline.AddParticleSystem(new ParticleSystem(50, 0, 0, 10000, glm::vec3(0.0f, 3.0f, -2.5f),
																									texManager->Find("Tatlas2"),
																									soundManager->GetSound("shot_sound"),
																									texManager->Find("Tatlas2")->numberofRows));
	};
	std::function<void()> emit_partilces_gpu_callback = [this]()
	{
		pipeline.AddParticleSystemGPU(glm::vec3(0.5f, 3.0f, -2.5f), texManager->Find("Tatlas2"));
	};
	std::function<void()> update_uniforms_callback = [this]()
	{
		pipeline.UpdateShadersInfo();
	};
	static std::function<void(const std::string&)> add_model_callback = [this](const std::string& _path)
	{
		modelManager->Add("Name", (GLchar*)_path.c_str());//@todo parse real name
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

	//Objects
	externalGui[2]->Add(OBJECT_REF, "Object", (void*)&m_focused);

	//Shaders
	externalGui[3]->Add(BUTTON, "Update shaders", (void*)&update_uniforms_callback);
	externalGui[3]->Add(SHADER, "Shaders", (void*)&pipeline.GetShaderInfos());

	//Main Menu
	externalGui[4]->Add(MENU_OPEN, "Add model", reinterpret_cast<void*>(&add_model_callback));
}
