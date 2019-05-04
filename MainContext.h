#ifndef MAIN_CONTEXT_H
#define MAIN_CONTEXT_H

#include <glew-2.1.0\include\GL\glew.h>
#include "MainContextBase.h"
#include "Structures.h"

#include "TerrainModel.h"
#include "Camera.h"
#include "CameraRay.h"
#include "GUI.h"

class SoundContext;
class remSnd;
class IWindowImGui;

class eMainContext : public eMainContextBase
{
public:
	eMainContext(eInputController*,
				IWindowImGui*,
				const std::string& modelsPath, 
				const std::string& assetsPath, 
				const std::string& shadersPath);
	virtual ~eMainContext() = default;

	virtual void			InitializeGL()									override;
	virtual void			PaintGL()										override;

	virtual bool			OnMouseMove(uint32_t x, uint32_t y)				override;
	virtual bool			OnKeyPress(uint32_t asci)						override;
	virtual bool			OnMousePress(uint32_t x, uint32_t y, bool left) override;
	virtual bool			OnMouseRelease()								override;
	
protected:
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

	float								waterHeight = 2.0f;
	
	bool								mts			= true;
	bool								skybox		= true;
	bool								mousepress	= false; //to draw framed objects
	
	glm::mat4							viewToProjectionMatrix;
	glm::mat4							scale_bias_matrix;

protected:
	virtual void						InitializePipline()		override;
	virtual void						InitializeBuffers()		override;
	virtual void						InitializeModels()		override;
	virtual void						InitializeRenders()		override;
	virtual void						Pipeline()				override;
	void								InitializeSounds();
};

#endif
