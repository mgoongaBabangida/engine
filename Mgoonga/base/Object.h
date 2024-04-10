#pragma once

#include "interfaces.h"

//---------------------------------------------------------------------------------
class DLL_BASE eObject: public std::enable_shared_from_this<eObject>
{
public:

	enum class RenderType
	{
		PHONG,
		PBR,
		FLAG,
		OUTLINED,
		GEOMETRY,
		BEZIER_CURVE,
		LINES,
		AREA_LIGHT_ONLY,
		TERRAIN_TESSELLATION
	};

	explicit eObject() {} //@todo
	virtual ~eObject();

	bool operator==(const eObject&);
	bool operator!=(const eObject&);

	//Properties
	bool IsVisible() const { return m_is_visible; }
	void SetVisible(bool _visible) { m_is_visible = _visible; }

	bool IsPickable() const { return m_is_pickable; }
	void SetPickable(bool _pickable) { m_is_pickable = _pickable; }

	bool Is2DScreenSpace() const { return m_is_2d_screen_space; }
	void Set2DScreenSpace(bool _2d) { m_is_2d_screen_space = _2d; }

	bool IsTextureBlending() const { return m_is_texture_blending; }
	void SetTextureBlending(bool _tex_blending) { m_is_texture_blending = _tex_blending; }

	void SetInstancingTag(const std::string& _tag) { m_instancing_tag = _tag; }
	const std::string& GetInstancingTag() const { return m_instancing_tag; }

	RenderType GetRenderType() const { return m_render_type; }
	void SetRenderType(RenderType _render_type) { m_render_type = _render_type; }

	std::vector<std::shared_ptr<eObject>>&	GetChildrenObjects() { return m_children; }
	void AddChildObject(std::shared_ptr<eObject> _obj) { m_children.push_back(_obj); }

	//Setters
	void				SetRigger(IRigger* r);
	void				SetScript(IScript* scr);
	void				SetTransform(ITransform*);
	void				SetCollider(ICollider*);
	void				SetModel(IModel*);
	void				SetModel(std::shared_ptr<IModel>);
	void 				SetName(const std::string& _name) { name = _name; }

	//Getters
	IScript*						GetScript()		const;
	ITransform*					GetTransform()	const;
	ICollider*					GetCollider()	const;
	IModel*							GetModel()		const;
	IRigger*						GetRigger()		const;
	const std::string&	Name()			const	{ return name;				}

protected:
	std::shared_ptr<IModel>				model;
	std::unique_ptr<IScript>			script;
	std::unique_ptr<ITransform>		transform;
	std::unique_ptr<ICollider>		collider;
	std::unique_ptr<IRigger>			rigger;
	std::string										name;
	std::vector<std::shared_ptr<eObject>> m_children;

	bool m_is_visible = true;
	bool m_is_pickable = true;
	bool m_is_2d_screen_space = false;
	bool m_is_texture_blending = false;
	RenderType m_render_type = RenderType::PHONG;
	std::string m_instancing_tag;
};

using shObject = std::shared_ptr<eObject>;

std::vector<shObject> DLL_BASE GetObjectsWithChildren(std::vector<shObject>);