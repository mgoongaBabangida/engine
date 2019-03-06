#ifndef GUI_H
#define GUI_H
#include "Structures.h"
#include "Texture.h"
#include "InterfacesDB.h"
#include <memory>
#include <iostream>

class GUI
{
	int							screenWidth;
	int							screenHeight;
	Texture*					texture;
	int							topleftX;
	int							topleftY;
	int							Width;
	int							Height;
	std::shared_ptr<ICommand>	cmd;

public:
	GUI(int topleftX, int topleftY, int Width, int Height,int scWidth,int scHeight) 
		:topleftX(topleftX), topleftY(topleftY), Width(Width), Height(Height), screenWidth(scWidth), screenHeight(scHeight){}
	virtual void Draw(GLuint shader); //do we need?
	void setCommand(std::shared_ptr<ICommand> com) { cmd = com; }
	void SetTexture (Texture* t) { texture = t; }
	Texture* GetTexture() { return texture; }
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
