#pragma once

#include "sdl_assets.h"

#include <tuple>

#include <SDL/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL/include/SDL_opengl.h>

#include <base/Object.h>
#include <base/interfaces.h>
#include <base/InputController.h>

#include <map>

//---------------------------------------------------
class DLL_SDL_ASSETS eImGuiContext
{
public:
	void Init();
	void NewFrame();
	void Render();
	void CleanUp();
	static bool IsInitialized();

	static bool BlockEvents();
	static void SetBlockEvents(bool _b);

	static eImGuiContext& GetInstance(SDL_GLContext* context, SDL_Window* window);

private:
	eImGuiContext(SDL_GLContext*, SDL_Window*);
	SDL_GLContext*	context;
	SDL_Window*			window;
	static bool			is_initialized;
};

enum TypeImGui
{
	SLIDER_FLOAT,
  TEXT,
  CHECKBOX,
  MENU,
  TEXTURE,
	SLIDER_FLOAT_3,
	SLIDER_FLOAT_3_LARGE,
  SLIDER_FLOAT_3_CALLBACK,
  TEXT_INT,
	TEXT_INT32,
  COMBO_BOX,
  TEXTURE_ARRAY,
  BUTTON,
	OBJECT_REF_TRANSFORM,
	OBJECT_REF_MATERIAL,
	OBJECT_REF_RIGGER,
	OBJECT_LIST,
	SHADER,
	MENU_OPEN,
	MENU_OPEN_SCENE,
	MENU_SAVE_SCENE,
	CONSOLE,
	PARTICLE_SYSTEM,
	SLIDER_INT,
	ADD_CALLBACK,
	GAME,
	SLIDER_FLOAT_LARGE,
	SLIDER_FLOAT_NERROW,
	SLIDER_INT_NERROW,
	SPIN_BOX,
	CAMERA,
	LIGHT_TYPE_VISUAL
};

//--------------------------------------------------
class IWindowImGui : public IInputObserver
{
public:
	virtual void Render()	{}
	virtual void Add(TypeImGui, const std::string& name, void*)		{}
	virtual void SetViewportOffset(float x_offset, float y_offset) {}
	virtual void SetWindowOffset(float x_offset, float y_offset) {}
	virtual bool IsHovered() { return false; }
};

//--------------------------------------------------
class eWindowImGuiDemo : public IWindowImGui
{
public:
	virtual void Render() override;
};

//---------------------------------------------
class DLL_SDL_ASSETS eWindowImGui : public IWindowImGui
{
public:
	virtual void SetViewportOffset(float x_offset, float y_offset) override;
	virtual void SetWindowOffset(float x_offset, float y_offset) override;

	virtual bool OnMousePress(int32_t x, int32_t y, bool left, KeyModifiers _modifier) override;
	virtual bool OnMouseMove(int32_t x, int32_t y, KeyModifiers _modifier) override;
	virtual bool OnMouseWheel(int32_t x, int32_t y, KeyModifiers _modifier) override;
	virtual bool IsHovered() override;

protected:
	float								window_pos_x;
	float								window_pos_y;
	float								window_size_x;
	float								window_size_y;

	float								viewport_offset_x;
	float								viewport_offset_y;
	float								window_offset_x;
	float								window_offset_y;

	float								cursor_x;
	float								cursor_y;
};
