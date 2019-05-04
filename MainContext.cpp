#include "stdafx.h"
#include "MainContext.h"
#include "ImGuiContext.h"
#include "GlBufferContext.h"

#include "ShipScript.h"
#include "Sound.h"
#include "Texture.h"

#include "ParticleSystem.h"
#include "ShootingParticleSystem.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

eMainContext::eMainContext(eInputController* _input,
						   IWindowImGui*	 _guiWnd,
						   const std::string& _modelsPath,
						   const std::string& _assetsPath, 
						   const std::string& _shadersPath)
: eMainContextBase(_input, _guiWnd, _modelsPath, _assetsPath, _shadersPath)
{
	_guiWnd->Add(SLIDER_FLOAT, "Ydir", m_light.light_position.y);
	_guiWnd->Add(SLIDER_FLOAT, "Zdir", m_light.light_position.z);
	_guiWnd->Add(SLIDER_FLOAT, "Xdir", m_light.light_position.x);

	//Light init!
	m_light.ambient			= vec3(0.1f, 0.1f, 0.1f);
	m_light.diffuse			= vec3(0.75f, 0.75f, 0.75f);
	m_light.specular		= vec3(0.5f, 0.5f, 0.5f);
	m_light.light_position	= vec4(1.0f, 4.0f, 1.0f, 1.0f);
}

bool eMainContext::OnMouseMove(uint32_t x, uint32_t y)
{
	m_framed = camRay.onMove(m_camera, m_Objects, x, y); 	//to draw a frame
	return true;
}

bool eMainContext::OnKeyPress(uint32_t asci)
{
	switch (asci)
	{
	case ASCII_J:
	{
		if (m_focused != nullptr)
			m_focused->MoveLeft(m_Objects);
	}
	return true;
	case ASCII_L:
	{
		if (m_focused != nullptr)
			m_focused->MoveRight(m_Objects);
	}
	return true;
	case ASCII_K:
	{
		if (m_focused != nullptr)
			m_focused->MoveBack(m_Objects);
	}
	return true;
	case ASCII_I:
	{
		if (m_focused != nullptr)
			m_focused->MoveForward(m_Objects);
	}
	return true;
	case ASCII_Z:
	{
		if (m_focused != nullptr)
			m_focused->MoveUp(m_Objects);
	}
	return true;
	case ASCII_X:
	{
		if (m_focused != nullptr)
			m_focused->MoveDown(m_Objects);
	}
	return true;
	case ASCII_C:
	{
		if (m_focused != nullptr)
			m_focused->TurnRight(m_Objects);
	}
	return true;
	case ASCII_V:
	{
		if (m_focused != nullptr)
			m_focused->TurnLeft(m_Objects);
	}
	return true;
	case ASCII_B:
	{
		if (m_focused != nullptr)
			m_focused->LeanRight(m_Objects);
	}
	return true;
	case ASCII_N:
	{
		if (m_focused != nullptr)
			m_focused->LeanLeft(m_Objects);
	}
	return true;
	case ASCII_U:
	{
		if (m_focused != nullptr)
			m_focused->LeanForward(m_Objects);
	}
	return true;
	case ASCII_H:
	{
		if (m_focused != nullptr)
			m_focused->LeanBack(m_Objects);
	}
	return true;
	case ASCII_G:
	{
		if (m_focused != nullptr)
			m_focused->getScript()->OnKeyPress(ASCII_G);
	}
	return true;
	default: return false;
	}
}

bool eMainContext::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	camRay.Update(m_camera, x, y, width, height);
	if(left)
	{
		camRay.press(x, y);
		m_focused = camRay.calculateIntersaction(m_Objects);
	}
	if(m_focused && m_focused->getScript())
	{
		m_focused->getScript()->OnMousePress(x, y, left);
	}
	return true;
}

bool eMainContext::OnMouseRelease()
{
	camRay.release();
	return true;
}

void eMainContext::InitializeGL()
{
	texManager.InitContext(assetsFolderPath);
	texManager.LoadAllTextures();
	//m_Textures.Find("Tbricks0_d")->second.saveToFile("MyTexture");  Saving texture debug

	InitializePipline();

	InitializeBuffers();

	InitializeSounds();

	modelManager.InitializePrimitives();

	InitializeModels();
	
	InitializeRenders();
	
	//Camera Ray
	camRay.init(width, height, nearPlane, farPlane);
	
	m_Objects[4]->setScript(new eShipScript(texManager.Find("TSpanishFlag0_s"),
											renderManager.ParticleRender(),
											texManager.Find("Tatlas2"),
											sound.get(),
											&camRay,
											waterHeight));
	m_focused = m_Objects[4];

	guis.push_back(GUI(0, 0, width / 4, height / 4, width, height));
	guis[0].setCommand(std::shared_ptr<ICommand>(new AnimStart(m_Objects[6])));
	guis.push_back(GUI(width / 4 * 3, height / 4 * 3, width / 4, height / 4, width, height));
	guis[1].setCommand(std::shared_ptr<ICommand>(new AnimStop(m_Objects[6])));

	inputController->AddObserver(this, STRONG);
	inputController->AddObserver(&guis[0], MONOPOLY);
	inputController->AddObserver(&guis[1], MONOPOLY);
	inputController->AddObserver(&m_camera, WEAK);
	inputController->AddObserver(&camRay, WEAK);
}

void eMainContext::InitializeSounds()
{
	context.reset(new SoundContext());
	context->init();
	context->LoadWavFile(assetsFolderPath + "Cannon+5.wav");

	sound.reset(new remSnd(context->buffers.find(assetsFolderPath + "Cannon+5.wav")->second, true));
	sound->loadListner(m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);
}

void eMainContext::InitializePipline()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);
	/*glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/

#define GLM_FORCE_RADIANS

	viewToProjectionMatrix = glm::perspective(glm::radians(60.0f), ((float)width) / height, nearPlane, farPlane);
	scale_bias_matrix = mat4(vec4(0.5f, 0.0f, 0.0f, 0.0f),
		vec4(0.0f, 0.5f, 0.0f, 0.0f),
		vec4(0.0f, 0.0f, 0.5f, 0.0f),
		vec4(0.5f, 0.5f, 0.5f, 1.0f));
}

void eMainContext::InitializeBuffers()
{
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SCREEN, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_MTS, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFLECTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_REFRACTION, width, height);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_SHADOW, width * 2, height * 2);
	eGlBufferContext::GetInstance().BufferInit(eBuffer::BUFFER_DEFFERED, width, height);
}

void eMainContext::InitializeModels()
{
	eMainContextBase::InitializeModels();

	//MODELS
	modelManager.Add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());
	modelManager.Add("boat", (GLchar*)std::string(modelFolderPath + "Medieval Boat/Medieval Boat.obj").c_str());
	//modelManager.Add("spider", (GLchar*)std::string(ModelFolderPath + "ogldev-master/Content/spider.obj").c_str());
	modelManager.Add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	modelManager.Add("guard", (GLchar*)std::string(modelFolderPath + "ogldev-master/Content/guard/boblampclean.md5mesh").c_str());
	//modelManager.Add("stairs", (GLchar*)std::string(modelFolderPath + "stairs.blend").c_str());

	//TERRAIN
	m_TerrainModel.swap(modelManager.CloneTerrain("simple"));
	m_TerrainModel->initialize(texManager.Find("Tgrass0_d"),
		texManager.Find("Tgrass0_d"),
		texManager.Find("Tblue"),
		texManager.Find("TOcean0_s"));

	//OBJECTS
	shObject wallCube = shObject(new eObject(modelManager.Find("wall_cube").get()));
	wallCube->getTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	m_Objects.push_back(wallCube);

	shObject containerCube = shObject(new eObject(modelManager.Find("container_cube").get()));
	containerCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, -3.5f));
	containerCube->getTransform()->setScale(vec3(0.2f, 0.2f, 0.2f));
	m_Objects.push_back(containerCube);

	shObject arrow = shObject(new eObject(modelManager.Find("arrow").get()));
	arrow->getTransform()->setTranslation(vec3(1.0f, 1.0f, -1.0f));
	m_Objects.push_back(arrow);

	shObject grassPlane = shObject(new eObject(modelManager.Find("grass_plane").get()));
	grassPlane->getTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_Objects.push_back(grassPlane);

	shObject nanosuit = shObject(new eObject(modelManager.Find("nanosuit").get()));
	nanosuit->getTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->getTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	m_Objects.push_back(nanosuit);

	shObject terrain = shObject(new eObject((IModel*)m_TerrainModel.get(), "Terrain"));
	terrain->getTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	terrain->getTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
	m_Objects.push_back(terrain);

	shObject wolf = shObject(new eObject(modelManager.Find("wolf").get()));
	wolf->getTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->getTransform()->setTranslation(vec3(5.0f, 3.0f, 0.0f));
	wolf->setRigger(new Rigger((Model*)modelManager.Find("wolf").get()));
	wolf->getRigger()->ChangeName(std::string(), "Running");
	m_Objects.push_back(wolf);

	shObject brickCube = shObject(new eObject(modelManager.Find("brick_cube").get()));
	brickCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	m_Objects.push_back(brickCube);

	shObject guard = shObject(new eObject(modelManager.Find("guard").get()));
	guard->getTransform()->setTranslation(vec3(2.0f, 2.0f, 0.0f));
	guard->getTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	guard->getTransform()->setScale(glm::vec3(0.03f, 0.03f, 0.03f));
	guard->setRigger(new Rigger((Model*)modelManager.Find("guard").get()));
	m_Objects.push_back(guard);

	//light_cube
	lightObject = shObject(new eObject(modelManager.Find("white_sphere").get()));
	lightObject->getTransform()->setScale(vec3(0.05f, 0.05f, 0.05f));
	lightObject->getTransform()->setTranslation(m_light.light_position);
	m_Objects.push_back(lightObject);

	/*shObject boat = shObject(new eObject(modelManager.Find("boat").get()));
	boat->getTransform()->setScale(vec3(0.0001f, 0.0001f, 0.0001f));
	m_Objects.push_back(boat);*/
}

void eMainContext::InitializeRenders()
{
	eMainContextBase::InitializeRenders();

	renderManager.ParticleRender()->AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f), texManager.Find("Tatlas2"), sound.get()));
}

void eMainContext::PaintGL()
{
	for (auto &object : m_Objects)
	{
		if (object->getScript())
			object->getScript()->Update(m_Objects);
	}
	lightObject->getTransform()->setTranslation(m_light.light_position);
	
	Pipeline();
}

void eMainContext::Pipeline()
{
	//1 Shadow Render Pass
	glViewport(0, 0, width * 2, height * 2);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	// Bind the "depth only" FBO and set the viewport to the size of the depth texture 
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_SHADOW);
	// Clear
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glClearDepth(1.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
	// Enable polygon offset to resolve depth-fighting isuses 
	//glEnable(GL_POLYGON_OFFSET_FILL);
	//glPolygonOffset(2.0f, -2000.0f);
	// Draw from the light’s point of view DrawScene(true);

	renderManager.ShadowRender()->Render(viewToProjectionMatrix, m_camera, m_light, m_Objects);

	glDisable(GL_POLYGON_OFFSET_FILL);
	glCullFace(GL_BACK);

	glViewport(0, 0, width, height);

	eGlBufferContext::GetInstance().EnableReadingBuffer(eBuffer::BUFFER_SHADOW, GL_TEXTURE1);
	
	//shadow
	mat4 worldToViewMatrix = glm::lookAt(glm::vec3(m_light.light_position), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 shadow_matrix = scale_bias_matrix * viewToProjectionMatrix * worldToViewMatrix;
	
	//$todo optimize
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//3 Rendering reflaction and refraction to Textures
	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFLECTION);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDepthFunc(GL_LEQUAL);

	renderManager.SkyBoxRender()->Render(viewToProjectionMatrix, m_camera);

	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	renderManager.MainRender()->SetClipPlane(-waterHeight);

	Camera tem_cam = m_camera;
	m_camera.setPosition(glm::vec3(tem_cam.getPosition().x, 2 * (tem_cam.getPosition().y - waterHeight), tem_cam.getPosition().z));
	m_camera.setDirection(glm::reflect(m_camera.getDirection(), vec3(0, 1, 0)));  //water normal

	renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, m_Objects);
	m_camera = tem_cam;

	eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_REFRACTION);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	renderManager.MainRender()->SetClipPlane(waterHeight);
	renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, m_Objects);
	renderManager.MainRender()->SetClipPlane(10);

	//glDisable(GL_CLIP_DISTANCE0);

	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
	
	//sky noise
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	renderManager.SkyNoiseRender()->Render(viewToProjectionMatrix, m_camera);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);

	//4. Rendering to main FBO with stencil
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	if (m_focused)
	{
		renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, std::vector<shObject>{m_focused });
	}
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
	renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, m_Objects);
	glEnable(GL_STENCIL_TEST);

	//5. Rendering Stencil Outlineing
	if(m_focused != nullptr)
	{
		renderManager.OutlineRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, std::vector<shObject> {m_focused});
	}
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	glClear(GL_STENCIL_BUFFER_BIT);

	// Render Flags
	eShipScript* script = dynamic_cast<eShipScript*>(m_Objects[4]->getScript()); //magic number to change
	renderManager.WaveRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, std::vector<Flag>{script->GetFlag(m_camera)});

	//6. Rendering WaterQuad
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	glDisable(GL_CULL_FACE);
	renderManager.WaterRender()->Render(viewToProjectionMatrix, m_camera, m_light);
	glEnable(GL_CULL_FACE);

	//Hexes
	mat4 MVP = viewToProjectionMatrix * m_camera.getWorldToViewMatrix();
	renderManager.HexRender()->Render(MVP);

	//2  Draw skybox firs
	glDepthFunc(GL_LEQUAL);
	if(skybox)
	{
		renderManager.SkyBoxRender()->Render(viewToProjectionMatrix, m_camera);
	}

	//7  Particles
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);
	glCullFace(GL_TRUE);
	renderManager.ParticleRender()->Render(viewToProjectionMatrix, m_camera);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	//**************************MTS CODE*************************
	if (mts)
	{
		eGlBufferContext::GetInstance().ResolveMtsToScreen();
		renderManager.BrightFilterRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
		eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_BRIGHT_FILTER);
		glViewport(0, 0, width, height);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		renderManager.BrightFilterRender()->Render();
		renderManager.GaussianBlurRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_BRIGHT_FILTER));
		renderManager.GaussianBlurRender()->Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
		glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		renderManager.ScreenRender()->SetTexture(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_SCREEN));
		renderManager.ScreenRender()->SetTextureContrast(eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_GAUSSIAN_TWO));
		renderManager.ScreenRender()->Render(viewToProjectionMatrix, m_camera);
	}
	//***********************************************************

	//8.1 Texture visualization
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	
	if(mousepress)
	{renderManager.ScreenRender()->RenderFrame();}

	guis[0].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFLECTION));
	glViewport(guis[0].getViewPort().x, guis[0].getViewPort().y, guis[0].getViewPort().z, guis[0].getViewPort().w);
	renderManager.ScreenRender()->SetTexture(*(guis[0].GetTexture())); //copy texture
	renderManager.ScreenRender()->Render(viewToProjectionMatrix, m_camera);
	//8.2 Second quad
	guis[1].SetTexture(&eGlBufferContext::GetInstance().GetTexture(eBuffer::BUFFER_REFRACTION));
	glViewport(guis[1].getViewPort().x, guis[1].getViewPort().y, guis[1].getViewPort().z, guis[1].getViewPort().w);
	renderManager.ScreenRender()->SetTexture(*(guis[1].GetTexture()));
	renderManager.ScreenRender()->Render(viewToProjectionMatrix, m_camera);
}
