#pragma once

#include "sdl_assets.h"

#include <tuple>

#include <SDL/include/SDL.h>
#include <glew-2.1.0/include/GL/glew.h>
#include <SDL/include/SDL_opengl.h>

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
	SLIDER_FLOAT_3,
  SLIDER_FLOAT_3_CALLBACK,
  TEXT_INT,
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
	MENU_SAVE_SCENE
};

//--------------------------------------------------
class IWindowImGui : public IInputObserver
{
public:
	virtual void Render()	{}
	virtual void Add(TypeImGui, const std::string& name, void*)		{}
	virtual void SetViewportOffset(float x_offset, float y_offset) {}
	virtual void SetWindowOffset(float x_offset, float y_offset) {}
};

//--------------------------------------------------
class eWindowImGuiDemo : public IWindowImGui
{
public:
	virtual void Render() override;
};

//---------------------------------------------
class eMainImGuiWindow : public IWindowImGui
{
	using eItem = std::tuple<std::string, TypeImGui, void*>;
public:
	virtual void Render()	override;
	virtual void Add(TypeImGui, const std::string& name, void*) override;
protected:
	std::vector<eItem>	lines;
	std::string open_file_menu_name;
	std::function<void(const std::string&)> open_file_callback;

	std::string open_scene_menu_name;
	std::function<void(const std::string&)> open_scene_callback;

	std::string save_scene_menu_name;
	std::function<void(const std::string&)> save_scene_callback;
};

//---------------------------------------------
class eWindowImGui : public IWindowImGui
{
	using eItem = std::tuple<std::string, TypeImGui, void*>;
public:
  eWindowImGui(const std::string& _name);
	void SetViewportOffset(float x_offset, float y_offset);
	void SetWindowOffset(float x_offset, float y_offset);
	
	virtual void Render()	override;
	virtual void Add(TypeImGui, const std::string& name, void*) override;
  
	virtual bool OnMousePress(uint32_t x, uint32_t y, bool left) override;
  virtual bool OnMouseMove(uint32_t x, uint32_t y) override;

protected:
	std::vector<eItem>	lines;
	std::string					name;
	bool								visible = true;
	std::map<std::string, std::function<void()>> callbacks;
	std::vector<char> combo_list;

	float								window_pos_x;
  float								window_pos_y;
  float								window_size_x;
  float								window_size_y;
	float								viewport_offset_x;
	float								viewport_offset_y;
	float								window_offset_x;
	float								window_offset_y;
};

struct eDockSpaceImGui
{

};