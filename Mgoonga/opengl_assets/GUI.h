#ifndef GUI_H
#define GUI_H

#include <base/interfaces.h>
#include <base/Object.h>
#include <math/Timer.h>
#include <math/AnimationLeaner.h>
#include "Texture.h"

#include "opengl_assets.h"

//---------------------------------------------------------------
class DLL_OPENGL_ASSETS GUI : public IInputObserver
{
public:
	GUI();
	GUI(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight);
	GUI(const GUI&);
	virtual ~GUI() {}

	virtual bool	OnMousePress(uint32_t x, uint32_t y, bool left) override;

	virtual void UpdateSync();

	void			  setCommand(std::shared_ptr<ICommand> com);
	void			  SetTexture(const Texture& t);
	Texture*		GetTexture();
	
	void SetChild(std::shared_ptr<GUI>_child) { children.push_back(_child); }
	std::vector<std::shared_ptr<GUI>> GetChildren() const { return children;}

	void					virtual Perssed();
	bool					virtual isPressed(int x, int y);
	bool					IsVisible() const { return isVisible; }
	void					SetVisible(bool _isVisible) { isVisible = _isVisible; }

	void Move(glm::ivec2 _newTopLeft) {
		topleftX = _newTopLeft.x;
		topleftY = _newTopLeft.y;
	}

	glm::ivec4		getViewPort() const;
	glm::ivec2		getTopLeft() const;
	glm::ivec2		getBottomRight() const;

	std::pair<uint32_t, uint32_t> pointOnGUI(uint32_t x_window, uint32_t y_window);

protected:
	int32_t						screenWidth;
	int32_t						screenHeight;

	Texture					texture;

	int32_t						topleftX;
	int32_t						topleftY;
	int32_t						Width;
	int32_t						Height;

	std::vector<std::shared_ptr<GUI>> children;
	std::shared_ptr<ICommand>	cmd;
	bool						isVisible = true;
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
	std::pair<size_t, size_t> m_press_coords;
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

