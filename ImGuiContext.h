#pragma once
#include"GLWindowSDL.h"
#include <tuple>

class eImGuiContext
{
public:
	void Init();
	void NewFrame();
	void Render();
	void PreRender();
	void CleanUp();

	static eImGuiContext& GetInstance(SDL_GLContext* context, SDL_Window* window)
	{
		static eImGuiContext  instance(context, window);
		return instance;
	}
protected:
	eImGuiContext(SDL_GLContext*, SDL_Window*);
	SDL_GLContext*	context;
	SDL_Window*		window;
};

enum TypeImGui
{
	SLIDER_FLOAT,
};

class IWindowImGui
{
public:
	virtual void Render()	{}
	virtual void Add(TypeImGui, const std::string& name, std::reference_wrapper<float>)		{}
};

class eWindowImGuiDemo : public IWindowImGui
{
public:
	virtual void Render() override;
};

class eWindowImGui : public IWindowImGui
{
	using eItem = std::tuple<std::string, TypeImGui, std::reference_wrapper<float>>;
public:
	eWindowImGui(const std::string& _name) :name(_name) {}
	virtual void Render()																override;
	virtual void Add(TypeImGui, const std::string& name, std::reference_wrapper<float>) override;
protected:
	std::vector<eItem>	lines;
	std::string			name;
	bool				visible = true;
};