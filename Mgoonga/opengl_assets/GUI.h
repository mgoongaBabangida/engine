#ifndef GUI_H
#define GUI_H

#include <base/interfaces.h>
#include <base/Object.h>

#include <math/Timer.h>
#include <math/AnimationLeaner.h>
#include <math/Rect.h>
#include "Texture.h"

#include "opengl_assets.h"

//---------------------------------------------------------------
class DLL_OPENGL_ASSETS GUI : public IInputObserver
{
public:
	enum RenderFunc
	{
		Default = 0,
		CursorFollow = 1,
		GreyKernel = 2
	};

	GUI();
	GUI(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight);
	GUI(const dbb::Rect&, int scWidth, int scHeight);
	GUI(const GUI&);
	virtual ~GUI() {}

	virtual bool	OnMousePress(int32_t x, int32_t y, bool left) override;
	virtual bool	OnMouseMove(int32_t _x, int32_t _y)override;

	virtual void UpdateSync();

	void			  setCommand(std::shared_ptr<ICommand> com);
	void			  SetTexture(const Texture& t,
												glm::ivec2 topLeft,
												glm::ivec2 bottomRight);
	void			  SetTextureMask(const Texture& t); //should it have its topLeft & bottomRight?

	Texture*		GetTexture();
	Texture*		GetTextureMask();

	void SetChild(std::shared_ptr<GUI>_child) { children.push_back(_child); }
	std::vector<std::shared_ptr<GUI>> GetChildren() const { return children;}

	void					virtual Perssed();

	bool									isHover(int x, int y);
	bool					virtual isPressed(int x, int y);

	bool					IsVisible() const { return m_is_visible; }
	void					SetVisible(bool _isVisible) { m_is_visible = _isVisible; }

	bool					IsTransparent() const { return m_is_transparent; }
	void					SetTransparent(bool _isTransparent) { m_is_transparent = _isTransparent; }

	bool					IsTakingMouseEvents() const { return m_take_mouse_moves; }
	void					SetTakeMouseEvents(bool _take_mouse_moves) { m_take_mouse_moves = _take_mouse_moves; }

	RenderFunc GetRenderingFunc() const { return m_render_func; }
	void SetRenderingFunc(RenderFunc _func) { m_render_func = _func; }

	void Move(glm::ivec2 _newTopLeft)
	{
		topleftX = _newTopLeft.x;
		topleftY = _newTopLeft.y;
	}

	glm::ivec4		getViewPort() const;
	glm::ivec2		getTopLeft() const;
	glm::ivec2		getBottomRight() const;

	glm::ivec2		getTopLeftTexture() const;
	glm::ivec2		getBottomRightTexture() const;

	std::pair<uint32_t, uint32_t> pointOnGUI(uint32_t x_window, uint32_t y_window);

protected:
	int32_t						screenWidth;
	int32_t						screenHeight;

	Texture						texture;
	Texture						textureMask;

	int32_t						topleftX;
	int32_t						topleftY;
	int32_t						Width;
	int32_t						Height;

	int32_t						tex_topleftX;
	int32_t						tex_topleftY;
	int32_t						tex_Width;
	int32_t						tex_Height;

	std::vector<std::shared_ptr<GUI>> children;
	std::shared_ptr<ICommand>	cmd;

	bool						m_is_visible = true;
	bool						m_is_transparent = false;
	bool						m_take_mouse_moves = false;
	RenderFunc			m_render_func = RenderFunc::Default;
};

//------------------------------------------------------
struct GUICommand : public ICommand
{
	GUICommand(std::function<void()> _func)
		:m_func(_func)
	{}
	virtual void Execute()
	{
		m_func();
	}
	std::function<void()> m_func;
};

//----------------------------------------------
class DLL_OPENGL_ASSETS GUIWithAlpha : public GUI
{
public:
	GUIWithAlpha(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight);
	virtual bool isPressed(int x, int y) override;
	virtual void Perssed() override;
	virtual void UpdateSync() override;
protected:
	bool m_check_if_pressed = false;
	std::pair<size_t, size_t> m_press_coords; //move to base?
};

//----------------------------------------------
class DLL_OPENGL_ASSETS Cursor : public GUIWithAlpha
{
public:
	Cursor(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight)
		: GUIWithAlpha(topleftX, topleftY, Width, Height, scWidth, scHeight) {}

	virtual bool	OnMouseMove(int32_t x, int32_t y) override;
};

//----------------------------------------------
class DLL_OPENGL_ASSETS Movable2D : public GUI
{
public:
	Movable2D(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight)
		: GUI(topleftX, topleftY, Width, Height, scWidth, scHeight)
	{
		m_take_mouse_moves = true;
	}

	virtual bool	OnMouseMove(int32_t x, int32_t y) override;
	virtual bool	OnMousePress(int32_t x, int32_t y, bool left) override;
	virtual bool	OnMouseRelease() override;
protected:
	bool is_pressed = false;
	std::pair<size_t, size_t> m_press_coords; //move to base?
};

//----------------------------------------------
class DLL_OPENGL_ASSETS MenuBehavior : public ICommand
{
public:
	MenuBehavior(GUI* _g) :gui(_g) { _g->SetVisible(false); }
	MenuBehavior(const MenuBehavior&) = delete;
	virtual void Execute() override;
protected:
	GUI* gui = nullptr;
};

//----------------------------------------------
class DLL_OPENGL_ASSETS MenuBehaviorLeanerMove : public ICommand
{
public:
	MenuBehaviorLeanerMove(GUI* _g, math::AnimationLeaner&& _anim) :gui(_g), anim(std::move(_anim)) {}
	MenuBehaviorLeanerMove(const MenuBehaviorLeanerMove&) = delete;
	virtual void Execute() override;
	~MenuBehaviorLeanerMove();

protected:
	GUI* gui = nullptr;
	std::unique_ptr<math::Timer> timer;
	math::AnimationLeaner anim;
};

//----------------------------------------------
class DLL_OPENGL_ASSETS CommandTest : public ICommand
{
public:
	CommandTest(/*Game* g*/) /*:invoker(g)*/;
	CommandTest(const CommandTest&) = delete;
	virtual void Execute() override;
};

class DLL_OPENGL_ASSETS AnimStart : public ICommand
{
	shObject m_obj;
public:
	AnimStart(shObject obj);
	AnimStart(const AnimStart&);
	virtual void Execute() override;
};

class DLL_OPENGL_ASSETS AnimStop : public ICommand
{
	shObject m_obj;
public:
	AnimStop(shObject obj);
	AnimStop(const AnimStop&);
	virtual void Execute() override;
};

#endif

