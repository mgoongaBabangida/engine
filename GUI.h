#ifndef GUI_H
#define GUI_H
#include "Structures.h"
#include "InterfacesDB.h"
#include "Texture.h"
#include <memory>

#include <iostream>

class GUI : public IInputObserver
{
	int32_t						screenWidth;
	int32_t						screenHeight;
	Texture*					texture;
	int32_t						topleftX;
	int32_t						topleftY;
	int32_t						Width;
	int32_t						Height;
	std::shared_ptr<ICommand>	cmd;
	bool						isVisible = true;

public:
	GUI(int topleftX, int topleftY, int Width, int Height,int scWidth,int scHeight) 
		:topleftX(topleftX), topleftY(topleftY), Width(Width), Height(Height), screenWidth(scWidth), screenHeight(scHeight){}
	
	virtual bool OnMousePress(uint32_t x, uint32_t y, bool left) override;

	void			setCommand(std::shared_ptr<ICommand> com) { cmd = com; }
	void			SetTexture (Texture* t) { texture = t; }
	Texture*		GetTexture() { return texture; }
	
	void Perssed() 
	{ 
		cmd->execute(); 
	//std::cout << topleftX <<" "<< screenHeight - topleftY << " " << Width << " " << Height << " " << screenWidth << " " << screenHeight<< std::endl;
	}
	
	bool isPressed(int x, int y) 
	{
		return x > topleftX && y > topleftY && x < (topleftX + Width) && y < (topleftY + Height); //test!
	}
	
	glm::vec4 getViewPort()const 
	{
		return glm::vec4(topleftX, screenHeight - topleftY- Height, Width, Height); 
	}
};

class CommandTest :public ICommand
{
	/*Game* invoker;*/
public:
	CommandTest(/*Game* g*/) /*:invoker(g)*/ {}
	void execute() { std::cout << "Hello button!" << std::endl; }

};

class AnimStart : public ICommand
{
	shObject m_obj;
public:
	AnimStart(shObject obj) :m_obj(obj) {}
	void execute() {
		std::cout << "execute start" << std::endl;
		m_obj->getRigger()->Apply("Running");
	}
};

class AnimStop : public ICommand
{
	shObject m_obj;
public:
	AnimStop(shObject obj) :m_obj(obj) {}
	void execute() {
		std::cout << "execute stop" << std::endl;
		m_obj->getRigger()->Stop();
	}
};

#endif
