#ifndef MAIN_CONTEXT_BASE_H
#define MAIN_CONTEXT_BASE_H

#include "InterfacesDB.h"
#include "InputController.h"
#include "TextureManager.h"
#include "ModelManager.h"
#include "RenderManager.h"

class IWindowImGui;

class eMainContextBase : public IInputObserver
{
public:
	eMainContextBase(eInputController*,
					IWindowImGui*,
					const std::string& modelsPath,
					const std::string& assetsPath,
					const std::string& shadersPath);
	virtual ~eMainContextBase() = default;

	virtual void		InitializeGL();
	virtual void		PaintGL();

	uint32_t			Width()	 { return width; }
	uint32_t			Height() { return height; }
protected:
	virtual void		InitializePipline() {}
	virtual void		InitializeBuffers() {}
	virtual void		InitializeModels();
	virtual void		InitializeRenders();
	virtual void		Pipeline()			{}

	eInputController*	inputController;
	
	std::string			modelFolderPath;
	std::string			assetsFolderPath;
	std::string			shadersFolderPath;

	//managers
	eTextureManager		texManager;
	eModelManager		modelManager;
	eRenderManager		renderManager;

	uint32_t			width		= 1200;
	uint32_t			height		= 600;
	float				nearPlane	= 0.1f;
	float				farPlane	= 20.0f;
};

#endif
