#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <math/timer.h>
#include <math/Camera.h>

#include <sdl_assets/sdl_assets.h>

class eTextureManager;
class eModelManager;
class eSoundManager;

class eInputController;
class IWindowImGui;
class ITcpAgent;

//-------------------------------------------------------
class DLL_GAME_ASSETS eMainContextBase : public IGame, public IInputObserver
{
public:
	eMainContextBase(eInputController* _input,
									 std::vector<IWindowImGui*> _externalGui,
									 const std::string& _modelsPath,
									 const std::string& _assetsPath,
									 const std::string& _shadersPath);

	virtual ~eMainContextBase();

	//IGame
	virtual void			InitializeGL() override;
	virtual void			PaintGL() override;
	virtual uint32_t	GetFinalImageId() override;

	virtual size_t			Width() override;
	virtual size_t			Height() override;

	void InstallTcpServer();
	void InstallTcpClient();

protected:
	virtual void		InitializePipline() {}
	virtual void		InitializeBuffers() {}
	virtual void		InitializeModels();
	virtual void		InitializeRenders() {}
	virtual void		InitializeTextures();
	virtual void		InitializeSounds()  {}
  virtual void    InitializeExternalGui() {}
	virtual void		Pipeline()			{}

	Light&					GetMainLight();
	Camera&					GetMainCamera();

	eInputController*	inputController;
	
	std::string			modelFolderPath;
	std::string			assetsFolderPath;
	std::string			shadersFolderPath;

	std::vector<Light>					m_lights;
	std::vector<Camera>					m_cameras;
	//managers
	std::unique_ptr<eTextureManager>	texManager;
	std::unique_ptr<eModelManager>		modelManager;
	std::unique_ptr<eSoundManager>		soundManager;
  std::vector<IWindowImGui*>        externalGui;

	//tcp
	std::unique_ptr <ITcpAgent>				tcpAgent;
	std::unique_ptr<math::Timer>			tcpTimer;

	//@todo delete
	size_t		width		= 1200;
	size_t		height		= 600;
	float			nearPlane	= 0.1f;
	float			farPlane	= 20.0f;
};
