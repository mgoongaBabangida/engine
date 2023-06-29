#pragma once

#include "game_assets.h"

#include <base/interfaces.h>
#include <base/Event.h>

#include <math/timer.h>
#include <math/Camera.h>

#include <sdl_assets/sdl_assets.h>

#include <opengl_assets/OpenGlRenderPipeline.h>

#include "InputStrategy.h"

class eTextureManager;
class ModelManagerYAML;
class AnimationManagerYAML;
class eSoundManager;

class eInputController;
class IWindowImGui;
class ITcpAgent;

//-------------------------------------------------------
class DLL_GAME_ASSETS eMainContextBase : public IGame, public IInputObserver
{
public:
	enum class GizmoType // to be compatible with ImGuizmo
	{
		TRANSLATE = 7,
		ROTATE = 120,
		SCALE =896
	};

	enum class GameState
	{
		UNINITIALIZED,
		LOADING,
		LOADED
	};

	eMainContextBase(eInputController* _input,
									 std::vector<IWindowImGui*> _externalGui,
									 const std::string& _modelsPath,
									 const std::string& _assetsPath,
									 const std::string& _shadersPath);

	virtual ~eMainContextBase();

	Event<std::function<void(shObject)>> ObjectPicked;
	Event<std::function<void(shObject, shObject)>> FocusChanged;
	Event<std::function<void(shObject)>> ObjectBeingAddedToScene;
	Event<std::function<void(shObject)>> ObjectBeingDeletedFromScene;

	//IInputObserver
	virtual bool			OnKeyPress(uint32_t asci)		override;
	
	//IGame
	void			InitializeGL() final;

	virtual void			PaintGL() override;

	virtual void	AddObject(std::shared_ptr<eObject> _object) override;
	virtual void	SetFocused(std::shared_ptr<eObject>) override;
	void					SetFocused(const eObject* _newFocused);

	virtual void AddInputObserver(IInputObserver* _observer, ePriority _priority) override;

	virtual uint32_t																GetFinalImageId() override;
	virtual std::shared_ptr<eObject>								GetFocusedObject() override;
	virtual std::vector<std::shared_ptr<eObject>>		GetObjects() override { return m_objects; }

	virtual const Texture* GetTexture(const std::string& _name) const override;

	virtual Light& GetMainLight() override;

	Camera& GetMainCamera();
	virtual glm::mat4 GetMainCameraViewMatrix() override;
	virtual glm::mat4 GetMainCameraProjectionMatrix() override;
	
	virtual bool			UseGizmo() override { return m_use_guizmo; }
	virtual uint32_t	CurGizmoType() override { return (uint32_t)m_gizmo_type; }

	virtual size_t			Width() const override;
	virtual size_t			Height()  const override;

	void InstallTcpServer();
	void InstallTcpClient();

	virtual void AddGUI(const std::shared_ptr<GUI>&);
	virtual void AddText(std::shared_ptr<Text>);
	virtual std::vector<std::shared_ptr<Text>>& GetTexts();

protected:
	virtual void		InitializePipline();
	virtual void		InitializeBuffers() {}
	virtual void		InitializeModels();
	virtual void		InitializeRenders();
	virtual void		InitializeTextures();
	virtual void		InitializeSounds()  {}
	virtual void		InitializeScripts();
	virtual void		InitializeExternalGui();
	virtual void		Pipeline()			{}

	void						_PreInitModelManager();

	GameState								m_gameState = GameState::UNINITIALIZED;

	eInputController*				m_input_controller;
	
	std::string							modelFolderPath;
	std::string							assetsFolderPath;
	std::string							shadersFolderPath;

	std::vector<Light>					m_lights;
	std::vector<Camera>					m_cameras;

	math::eClock								m_global_clock;

	shObject																m_focused;
	std::vector<shObject>										m_objects;
	std::shared_ptr<std::vector<shObject>>	m_framed;
	std::vector<std::shared_ptr<GUI>>				m_guis;
	std::vector<std::shared_ptr<Text>>			m_texts;

	std::vector <std::shared_ptr<IScript>>	m_global_scripts;

	std::unique_ptr<InputStrategy>					m_input_strategy;
	//debuging
	shObject																m_light_object;

	bool												m_use_guizmo = true;
	GizmoType										m_gizmo_type = GizmoType::TRANSLATE;
	
	//managers
	std::unique_ptr<eTextureManager>					texManager;
	std::unique_ptr<ModelManagerYAML>					modelManager;
	std::unique_ptr<AnimationManagerYAML>			animationManager;
	std::unique_ptr<eSoundManager>						soundManager;
  std::vector<IWindowImGui*>								externalGui;

	//tcp
	std::unique_ptr <ITcpAgent>				tcpAgent;
	std::unique_ptr<math::Timer>			tcpTimer;

	//@todo delete
	size_t		width		= 1200;
	size_t		height		= 600;
	float			nearPlane	= 0.1f;
	float			farPlane	= 20.0f;
	bool m_l_pressed = false;

	eOpenGlRenderPipeline							pipeline;
};
