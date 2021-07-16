#pragma once

#include <base/interfaces.h>
#include <sdl_assets/sdl_assets.h>

#include <string>
#include <memory>

class eTextureManager;
class eModelManager;
class eSoundManager;

class eInputController;
class IWindowImGui;

//-------------------------------------------------------
class DLL_SDL_ASSETS eMainContextBase : public IInputObserver
{
public:
	eMainContextBase(eInputController* _input,
    std::vector<IWindowImGui*> _externalGui,
		const std::string& _modelsPath,
		const std::string& _assetsPath,
		const std::string& _shadersPath);

	virtual ~eMainContextBase();

	virtual void		InitializeGL();
	virtual void		PaintGL();

	size_t			Width();
	size_t			Height();

protected:
	virtual void		InitializePipline() {}
	virtual void		InitializeBuffers() {}
	virtual void		InitializeModels();
	virtual void		InitializeRenders() {}
	virtual void		InitializeTextures();
	virtual void		InitializeSounds()  {}
  virtual void    InitializeExternalGui() {}
	virtual void		Pipeline()			{}

	eInputController*	inputController;
	
	std::string			modelFolderPath;
	std::string			assetsFolderPath;
	std::string			shadersFolderPath;

	//managers
	std::unique_ptr<eTextureManager>	texManager;
	std::unique_ptr<eModelManager>		modelManager;
	std::unique_ptr<eSoundManager>		soundManager;
  std::vector<IWindowImGui*>        externalGui;

	size_t			width		= 1200;
	size_t			height		= 600;
	float			nearPlane	= 0.1f;
	float			farPlane	= 20.0f;
};

//----------------------------------------------------------------------
class IGameFactory
{
public:
	virtual eMainContextBase* CreateGame(eInputController*  _input,
                                       std::vector<IWindowImGui*>		_imgui_windows) const = 0;
};
