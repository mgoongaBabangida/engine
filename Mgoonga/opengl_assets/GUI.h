#ifndef GUI_H
#define GUI_H

#include <base/interfaces.h>
#include <base/Object.h>

#include "opengl_assets.h"

struct Texture;

class DLL_OPENGL_ASSETS GUI : public IInputObserver
{
public:
	GUI();
	GUI(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight);
	GUI(const GUI&);

	virtual bool	OnMousePress(uint32_t x, uint32_t y, bool left) override;

	void			setCommand(std::shared_ptr<ICommand> com);
	void			SetTexture(Texture* t);
	Texture*		GetTexture();
	
	void			Perssed();	
	bool			isPressed(int x, int y);	
	glm::vec4		getViewPort() const; //vec int?

protected:
	int32_t						screenWidth;
	int32_t						screenHeight;
	Texture*					texture;
	int32_t						topleftX;
	int32_t						topleftY;
	int32_t						Width;
	int32_t						Height;
	std::shared_ptr<ICommand>	cmd;
	bool						isVisible = true;
};

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
