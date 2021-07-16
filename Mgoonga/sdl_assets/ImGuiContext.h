#pragma once

#include "sdl_assets.h"

#include <tuple>
#include <vector>

#include <SDL2-2.0.9/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL2-2.0.9/include/SDL_opengl.h>

#include <base/InputController.h>

//---------------------------------------------------
class DLL_SDL_ASSETS eImGuiContext
{
public:
	void Init();
	void NewFrame();
	void Render();
	void PreRender();
	void CleanUp();

	static eImGuiContext& GetInstance(SDL_GLContext* context, SDL_Window* window);

private:
	eImGuiContext(SDL_GLContext*, SDL_Window*);
	SDL_GLContext*	context;
	SDL_Window*		window;
};

enum TypeImGui
{
	SLIDER_FLOAT,
  TEXT,
  CHECKBOX,
  MENU,
  TEXTURE,
  SLIDER_FLOAT_3_CALLBACK,
  TEXT_INT,
  COMBO_BOX,
  TEXTURE_ARRAY,
  BUTTON,
};

//--------------------------------------------------
class IWindowImGui : public IInputObserver
{
public:
	virtual void Render()	{}
	virtual void Add(TypeImGui, const std::string& name, void*)		{}

};

//--------------------------------------------------
class eWindowImGuiDemo : public IWindowImGui
{
public:
	virtual void Render() override;
};

//---------------------------------------------
class eWindowImGui : public IWindowImGui
{
	using eItem = std::tuple<std::string, TypeImGui, void*>;
public:
  eWindowImGui(const std::string& _name);
	virtual void Render()										override;
	virtual void Add(TypeImGui, const std::string& name, void*) override;
  virtual bool OnMousePress(uint32_t x, uint32_t y, bool left);
  virtual bool OnMouseMove(uint32_t x, uint32_t y) override;

protected:
	std::vector<eItem>	lines;
	std::string			name;
  float window_pos_x;
  float window_pos_y;
  float window_size_x;
  float window_size_y;
	bool				visible = true;
};