#include "stdafx.h"
#include "GUI.h"
#include <math/Rigger.h>

#include <memory>
#include <iostream>

GUI::GUI()
{
}

GUI::GUI(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight)
	: topleftX(topleftX), topleftY(topleftY), Width(Width), Height(Height), screenWidth(scWidth), screenHeight(scHeight) {}

GUI::GUI(const GUI& _other)
	: screenWidth(_other.screenWidth)
    , screenHeight(_other.screenHeight)
    , texture(_other.texture)
    , topleftX(_other.topleftX)
    , topleftY(_other.topleftY)
    , Width(_other.Width)
    , Height(_other.Height)
    , cmd(_other.cmd)
    , isVisible(_other.isVisible)
{
}

bool GUI::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	if(left && isVisible && isPressed(x, y))
	{
		Perssed();
		return true;
	}
	return false;
}

void GUI::setCommand(std::shared_ptr<ICommand> com)
{
	cmd = com;
}

void GUI::SetTexture(Texture * t)
{
	texture = t;
}

Texture * GUI::GetTexture()
{
	 return texture;
}

void GUI::Perssed()
{
  if(cmd)
	cmd->Execute();
}

bool GUI::isPressed(int x, int y)
{
	return x > topleftX && y > topleftY && x < (topleftX + Width) && y < (topleftY + Height); //test!
}

glm::vec4 GUI::getViewPort() const //vec int?
{
	return glm::vec4(topleftX, screenHeight - topleftY - Height, Width, Height);
}

AnimStart::AnimStart(shObject obj)
	:m_obj(obj)
{
}

AnimStart::AnimStart(const AnimStart& _other)
	:m_obj(_other.m_obj)
{
}

void AnimStart::Execute()
{
	m_obj->GetRigger()->Apply("Running");
}

AnimStop::AnimStop(shObject obj)
	:m_obj(obj)
{
}

AnimStop::AnimStop(const AnimStop& _other)
	: m_obj(_other.m_obj)
{
}

void AnimStop::Execute()
{
	m_obj->GetRigger()->Stop();
}

CommandTest::CommandTest()
{
}

void CommandTest::Execute()
{
}
