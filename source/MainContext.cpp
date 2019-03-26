#include "stdafx.h"
#include "MainContext.h"
#include "GlBufferContext.h"
#include "ParticleSystem.h"
#include "ShootingParticleSystem.h"
#include "ShipScript.h"
#include "Sound.h"
#include "InterfacesDB.h"
#include "Texture.h"

#include <iostream>

using glm::vec3;
using glm::vec4;
using glm::mat4;

eMainContext::eMainContext(eInputController* _input, 
						   const std::string& _modelsPath,
						   const std::string& _assetsPath, 
						   const std::string& _shadersPath)
:inputController(_input), modelFolderPath(_modelsPath), assetsFolderPath(_assetsPath), shadersFolderPath(_shadersPath){}

void eMainContext::UpdateLight(uint32_t x, uint32_t y, uint32_t z)
{
	lightObject->getTransform()->setTranslation(glm::vec3(x , y, z));
	m_light.light_vector.x = (float)x; 
	m_light.light_vector.y = (float)y; 
	m_light.light_vector.z = (float)z;
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
		m_focused->getScript()->OnMousePress(x,y,left);
	}
	return true;
#ifdef DEBUG_HANDLERS
		std::cout << "TARGET=" << "x= " << target.x << "y= " << target.y << "z= " << target.z << std::endl;

		glm::vec3 position = glm::vec3(0.0f, 2.0f, 0.0f);
		glm::vec3 direction = glm::vec3(0.0f, 0.0f, 1.0f);
		glm::vec3 target_dir = glm::normalize(target - position);
		float angle = glm::dot(glm::normalize(target_dir), glm::normalize(direction));
		std::cout << "in press" << std::endl;
		std::cout << "dot= " << angle << "radians= " << glm::acos(angle) << " degrees= " << glm::degrees(glm::acos(angle)) << std::endl;
		glm::quat rot = glm::toQuat(glm::rotate(glm::mat4(), glm::acos(angle), glm::vec3(0, 1, 0)));

		glm::vec3 ASIX = glm::normalize(glm::cross(target_dir, direction));
		std::cout << "Asix=" << ASIX.x << ASIX.y << ASIX.z << std::endl;
#endif
}

bool eMainContext::OnMouseRelease()
{
	camRay.release();
	return true;
}

void eMainContext::InitializeGL()
{
	texManager.InitContext(assetsFolderPath);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_LINE_SMOOTH);

	InitializeBuffers();

	//initialize sound
	context.reset(new SoundContext());
	context->init();
	context->LoadWavFile("Cannon+5.wav");

	modelManager.initializePrimitives();

	//Light init!
	m_light.ambient = vec3(0.1f, 0.1f, 0.1f);
	m_light.diffuse = vec3(0.75f, 0.75f, 0.75f);
	m_light.specular = vec3(0.5f, 0.5f, 0.5f);
	m_light.light_vector = vec4(1.0f, 2.0f, 1.0f, 1.0f);

	texManager.loadAllTextures();
	//m_Textures.find("Tbricks0_d")->second.saveToFile("MyTexture");  Saving texture debug
	GLuint t = (*(texManager.find("Tpink"))).id;

	InitializeModels();

	//light_cube
	lightObject = shObject(new eObject(modelManager.find("white_cube").get()));  //std::shared_ptr<Object>
	lightObject->getTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	lightObject->getTransform()->setTranslation(m_light.light_vector);

	//Camera Ray
	camRay.init(width, height, 0.1f, 20.0f);

	m_focused = m_Objects[4];

	InitializeRenders();

	guis.push_back(GUI(0, 0, width / 4, height / 4, width, height));
	guis[0].setCommand(std::shared_ptr<ICommand>(new AnimStart(m_Objects[6])));
	guis.push_back(GUI(width / 4 * 3, height / 4 * 3, width / 4, height / 4, width, height));
	guis[1].setCommand(std::shared_ptr<ICommand>(new AnimStop(m_Objects[6])));

	inputController->AddObserver(this, STRONG);
	inputController->AddObserver(&guis[0], MONOPOLY);
	inputController->AddObserver(&guis[1], MONOPOLY);
	inputController->AddObserver(&m_camera, WEAK);
	inputController->AddObserver(&camRay, WEAK);

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
	//PRIMITIVES
	modelManager.addPrimitive("wall_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("cube"),
			texManager.find("Tbrickwall0_d"),
			texManager.find("Tbrickwall0_d"),
			texManager.find("Tbrickwall0_n"),
			texManager.find("Tblack"))));
	modelManager.addPrimitive("container_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("cube"),
			texManager.find("Tcontainer0_d"),
			texManager.find("Tcontainer0_s"),
			texManager.find("Tblue"),
			texManager.find("Tblack"))));
	modelManager.addPrimitive("arrow",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("arrow"),
			texManager.find("Tcontainer0_d"),
			texManager.find("Tcontainer0_s"),
			texManager.find("Tblue"),
			texManager.find("Tblack"))));
	modelManager.addPrimitive("grass_plane",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("plane"),
			texManager.find("Tgrass0_d"),
			texManager.find("Tgrass0_d"),
			texManager.find("Tblue"),
			texManager.find("Tblack"))));
	modelManager.addPrimitive("white_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("cube"),
			texManager.find("Twhite"))));
	modelManager.addPrimitive("brick_square",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("square"),
			texManager.find("Tbricks0_d"),
			texManager.find("Tbricks0_d"),
			texManager.find("Tblue"),
			texManager.find("Tblack"))));
	modelManager.addPrimitive("brick_cube",
		std::shared_ptr<MyModel>(new MyModel(modelManager.findMesh("cube"),
			texManager.find("Tbricks2_d"),
			texManager.find("Tbricks2_d"),
			texManager.find("Tbricks2_n"),
			texManager.find("Tbricks2_dp"))));

	//MODELS
	modelManager.add("nanosuit", (GLchar*)std::string(modelFolderPath + "nanosuit/nanosuit.obj").c_str());
	//modelManager.add("boat", (GLchar*)std::string(ModelFolderPath + "Medieval Boat/Medieval Boat.obj").c_str());
	//modelManager.add("spider", (GLchar*)std::string(ModelFolderPath + "ogldev-master/Content/spider.obj").c_str());
	modelManager.add("wolf", (GLchar*)std::string(modelFolderPath + "Wolf Rigged and Game Ready/Wolf_dae.dae").c_str());
	//(GLchar*)ModelFolderPath.append("Wolf Rigged and Game Ready/Wolf_One_obj.obj").c_str()
	//modelManager.add("guard", (GLchar*)ModelFolderPath.append("ogldev-master/Content/guard/boblampclean.md5mesh").c_str());
	modelManager.add("stairs", (GLchar*)std::string(modelFolderPath + "stairs.blend").c_str());

	//TERRAIN
	m_TerrainModel.swap(modelManager.cloneTerrain("simple"));
	m_TerrainModel->initialize(texManager.find("Tgrass0_d"),
		texManager.find("Tgrass0_d"),
		texManager.find("Tblue"),
		texManager.find("TOcean0_s"));

	//OBJECTS
	shObject wallCube = shObject(new eObject(modelManager.find("wall_cube").get()));
	wallCube->getTransform()->setTranslation(vec3(3.0f, 3.0f, 3.0f));
	m_Objects.push_back(wallCube);

	shObject containerCube = shObject(new eObject(modelManager.find("container_cube").get()));
	containerCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, -3.5f));
	containerCube->getTransform()->setScale(vec3(0.2f, 0.2f, 0.2f));
	m_Objects.push_back(containerCube);

	shObject arrow = shObject(new eObject(modelManager.find("arrow").get()));
	arrow->getTransform()->setTranslation(vec3(1.0f, 1.0f, -1.0f));
	m_Objects.push_back(arrow);

	shObject grassPlane = shObject(new eObject(modelManager.find("grass_plane").get()));
	grassPlane->getTransform()->setTranslation(vec3(0.0f, -2.0f, 0.0f));
	m_Objects.push_back(grassPlane);

	shObject nanosuit = shObject(new eObject(modelManager.find("nanosuit").get()));
	nanosuit->getTransform()->setTranslation(vec3(0.0f, 2.0f, 0.0f));
	nanosuit->getTransform()->setScale(vec3(0.1f, 0.1f, 0.1f));
	m_Objects.push_back(nanosuit);

	shObject terrain = shObject(new eObject((IModel*)m_TerrainModel.get(),"Terrain"));
	terrain->getTransform()->setScale(vec3(0.3f, 0.3f, 0.3f));
	terrain->getTransform()->setTranslation(vec3(0.0f, 1.8f, 0.0f));
	m_Objects.push_back(terrain);

	shObject wolf = shObject(new eObject(modelManager.find("wolf").get()));
	wolf->getTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	wolf->getTransform()->setTranslation(vec3(5.0f, 3.0f, 0.0f));
	wolf->setRigger(new Rigger((Model*)modelManager.find("wolf").get()));
	wolf->getRigger()->ChangeName(std::string(), "Running");
	m_Objects.push_back(wolf);

	shObject brickCube = shObject(new eObject(modelManager.find("brick_cube").get()));
	brickCube->getTransform()->setTranslation(vec3(0.5f, 3.0f, 3.5f));
	m_Objects.push_back(brickCube);

	shObject stairs = shObject(new eObject(modelManager.find("stairs").get()));
	stairs->getTransform()->setTranslation(vec3(4.5f, 3.0f, 5.5f));
	stairs->getTransform()->setRotation(glm::radians(-90.0f), 0.0f, 0.0f);
	m_Objects.push_back(stairs);
}

void eMainContext::InitializeRenders()
{
	sound.reset(new remSnd(context->buffers.find(assetsFolderPath + "Cannon+5.wav")->second, true));
	sound->loadListner(m_camera.getPosition().x, m_camera.getPosition().y, m_camera.getPosition().z);

	renderManager.Initialize(modelManager, texManager, shadersFolderPath);

	renderManager.ParticleRender()->AddParticleSystem(new ParticleSystem(10, 0, 0, 10000, glm::vec3(0.0f, 4.0f, -0.5f), texManager.find("Tatlas2"), sound.get()));

	m_Objects[4]->setScript(new eShipScript(texManager.find("TSpanishFlag0_s"), 
											renderManager.ParticleRender(), 
											texManager.find("Tatlas2"),
											sound.get(),
											&camRay,
											waterHeight));
}

void eMainContext::PaintGL()
{
	for(auto &object : m_Objects)
	{
		if(object->getScript())
			object->getScript()->Update(m_Objects);
	}
	Pipeline();
}

void eMainContext::Pipeline()
{
	glViewport(0, 0, width, height); 
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
	mat4 worldToViewMatrix = glm::lookAt(glm::vec3(m_light.light_vector), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	mat4 shadow_matrix = scale_bias_matrix * viewToProjectionMatrix * worldToViewMatrix;

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

	//sky noise
	/* glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	m_skynoiseRender->render(viewToProjectionMatrix, m_camera);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);*/

	//4. Rendering to main FBO with stencil
	mts ? eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_MTS)
		: eGlBufferContext::GetInstance().EnableWrittingBuffer(eBuffer::BUFFER_DEFAULT);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glStencilFunc(GL_ALWAYS, 1, 0xFF);
	glStencilMask(0xFF);
	if(m_focused)
	{
		renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, std::vector<shObject>{m_focused });
	}
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glStencilMask(0x00);
	glDisable(GL_STENCIL_TEST);
	renderManager.MainRender()->Render(viewToProjectionMatrix, m_camera, m_light, shadow_matrix, m_Objects);
	glEnable(GL_STENCIL_TEST);

	//5. Rendering Stencil Outlineing
	if (m_focused != nullptr)
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
	renderManager.SkyBoxRender()->Render(viewToProjectionMatrix, m_camera);

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
	if(mts)
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
	//if(mousepress)
	//{m_screenRender->renderFrame();}
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
