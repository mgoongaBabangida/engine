#ifndef MAIN_CONTEXT_H
#define MAIN_CONTEXT_H

#include <glew-2.0.0\include\GL\glew.h>
#include "Structures.h"
#include "TerrainModel.h"
#include "Camera.h"
#include "CameraRay.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "RenderManager.h"
#include "GUI.h"
#include "InputController.h"

class SoundContext;
class remSnd;

class eMainContext : public IInputObserver
{
public:
	eMainContext(eInputController*, const std::string& modelsPath, const std::string& assetsPath, const std::string& shadersPath);
	virtual ~eMainContext() = default;

	void					InitializeGL();
	void					PaintGL();
	void					UpdateLight(uint32_t x, uint32_t y, uint32_t z);

	virtual bool			OnMouseMove(uint32_t x, uint32_t y)				override;
	virtual bool			OnKeyPress(uint32_t asci)						override;
	virtual bool			OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual bool			OnMouseRelease()								override;

	uint32_t				Width()				{ return width;			}
	uint32_t				Height()			{ return height;		}

private:
	eInputController*					inputController;
	Camera								m_camera;
	dbb::CameraRay						camRay;
	
	shObject							m_focused;
	std::vector<shObject>				m_Objects;
	std::vector<shObject>				m_framed;
	Light								m_light;
	shObject							lightObject; //debuging
	std::unique_ptr<TerrainModel>		m_TerrainModel;
	
	std::unique_ptr<SoundContext>		context; //test
	std::unique_ptr<remSnd>				sound;  //test
	std::vector<GUI>					guis;
	//managers
	TextureManager						texManager;
	ModelManager						modelManager;
	eRenderManager						renderManager;

	float								waterHeight = 2.0f;
	uint32_t							width		= 1200;
	uint32_t							height		= 600;
	float								nearPlane   = 0.1f;
	float								farPlane    = 20.0f;
	
	bool								mts			= true;
	bool								mousepress	= false; //to draw framed objects
	
	std::string							modelFolderPath;
	std::string							assetsFolderPath;
	std::string							shadersFolderPath;
	
	mat4								viewToProjectionMatrix;
	mat4								scale_bias_matrix;

protected:
	void								InitializeBuffers();
	void								InitializeModels();
	void								InitializeRenders();
	void								Pipeline();
};

#endif
