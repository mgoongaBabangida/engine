#pragma once

#include "ImGuiContext.h"

//---------------------------------------------
class eWindowImGuiExternal : public eWindowImGui
{
	using eItem = std::tuple<std::string, TypeImGui, void*>;
public:
	explicit eWindowImGuiExternal(const std::string& _name);

	virtual void Render()	override;
	virtual void Add(TypeImGui, const std::string& name, void*) override;
protected:
	std::vector<eItem>	lines;
	std::string					name;
	bool								visible = true;
	std::map<std::string, std::function<void()>> callbacks;
	std::map<std::string, std::function<void(shObject, const std::string&)>> callbacks_string;
	IGame* mp_game = nullptr;

	//should be specific for descendents
	std::vector<char>					m_combo_list;
	shObject									m_current_object;
	std::vector<std::string>	mesh_names;
	std::vector<std::string>	animation_names;
	std::vector<std::string>	frame_names;
	std::vector<std::string>	bone_names;
	std::function<void(const std::string&)> console_callback;
};

//---------------------------------------------
class eMainImGuiWindow : public eWindowImGuiExternal
{
	using eItem = std::tuple<std::string, TypeImGui, void*>;
public:
	eMainImGuiWindow();
	virtual void Render()	override;
	virtual void Add(TypeImGui, const std::string& name, void*) override;

	virtual bool OnMouseMove(int32_t x, int32_t y, KeyModifiers _modifier) override;
	virtual bool IsHovered() override;

protected:
	std::vector<eItem>	lines;
	std::vector<eItem>	tool_items;

	std::string open_file_menu_name;
	std::function<void(const std::string&)> open_file_callback;

	std::string open_scene_menu_name;
	std::function<void(const std::string&)> open_scene_callback;

	std::string save_scene_menu_name;
	std::function<void(const std::string&)> save_scene_callback;
};

struct eDockSpaceImGui
{

};