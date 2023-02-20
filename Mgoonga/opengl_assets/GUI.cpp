#include "stdafx.h"
#include "GUI.h"
#include <math/Rigger.h>

#include <memory>
#include <iostream>

GUI::GUI()
{
}

GUI::GUI(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight)
	: topleftX(topleftX),
	topleftY(topleftY),
	Width(Width),
	Height(Height),
	screenWidth(scWidth),
	screenHeight(scHeight)
	{}

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

void GUI::UpdateSync()
{

}

void GUI::setCommand(std::shared_ptr<ICommand> com)
{
	cmd = com;
}

void GUI::SetTexture(const Texture& t)
{
	texture = t;
}

Texture * GUI::GetTexture()
{
	 return &texture;
}

void GUI::Perssed()
{
  if(cmd)
		cmd->Execute();
}

bool GUI::isPressed(int x, int y)
{
	return x > topleftX && y > topleftY && x < (topleftX + Width) && y < (topleftY + Height);
}

glm::ivec4 GUI::getViewPort() const
{
	return glm::ivec4(topleftX, screenHeight - topleftY - Height, Width, Height);
}

glm::ivec2 GUI::getTopLeft() const
{
	return glm::ivec2(topleftX, topleftY);
}

glm::ivec2 GUI::getBottomRight() const
{
	return glm::ivec2(topleftX + Width, topleftY + Height);
}

std::pair<uint32_t, uint32_t> GUI::pointOnGUI(uint32_t x_window, uint32_t y_window)
{
	//works if it is inside
	return std::pair<uint32_t, uint32_t>(x_window - topleftX, y_window- topleftY);
}

//---------------------------------------------------------------------------------
GUIWithAlpha::GUIWithAlpha(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight)
: GUI(topleftX, topleftY, Width, Height, scWidth, scHeight)
{
}

//------------------------------------------------------
bool GUIWithAlpha::isPressed(int _x, int _y)
{
	m_check_if_pressed = GUI::isPressed(_x, _y);
	m_press_coords = { _x ,  _y };
	return m_check_if_pressed;
}

//------------------------------------------------------
void GUIWithAlpha::Perssed()
{
	if (!m_check_if_pressed)
		GUI::Perssed();
}

//--------------------------------------------------
void GUIWithAlpha::UpdateSync()
{
	GUI::UpdateSync();
	if (m_check_if_pressed)
	{
		uint8_t* imData = texture.getPixelBuffer();
		GLubyte r, g, b, a; // or GLubyte r, g, b, a;
		
		auto[x, y] = pointOnGUI(m_press_coords.first, m_press_coords.second);// line and column of the pixel
		y = Height - y; // invert y to bottom left coord system
		int32_t elmes_per_line = texture.mTextureWidth * 4; // elements per line = 256 * "RGBA"

		y = y * (texture.mTextureHeight / Height);
		x = x * (texture.mTextureWidth / Width);

		int32_t row = y * elmes_per_line;
		int32_t col = x * 4;

		r = imData[row + col];
		g = imData[row + col + 1];
		b = imData[row + col + 2];
		a = imData[row + col + 3];

		m_check_if_pressed = false;
		if (a > 0)
			Perssed();

		free(imData);
	}
}

//-------------------------------------------------------------
bool Cursor::OnMouseMove(uint32_t x, uint32_t y)
{
	Move({ x,y });
	return false;
}

//-------------------------------------------------------------
bool Movable2D::OnMouseMove(uint32_t x, uint32_t y)
{
	if(is_pressed)
		Move({ x - m_press_coords.first,y - m_press_coords.second });
	return is_pressed;
}

//-------------------------------------------------------------
bool Movable2D::OnMousePress(uint32_t x, uint32_t y, bool left)
{
	if (GUI::OnMousePress(x, y, left))
	{
		is_pressed = true;
		m_press_coords = {x - topleftX , y - topleftY};
		return true;
	}
	return false;
}

//-------------------------------------------------------------
bool Movable2D::OnMouseRelease()
{
	if (is_pressed)
	{
		is_pressed = false;
		return true;
	}
	return false;
}

//--------------------------------------------------
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

//--------------------------------------
CommandTest::CommandTest()
{
}

void CommandTest::Execute()
{
}

//--------------------------------------
void MenuBehavior::Execute()
{
	gui->SetVisible(!gui->IsVisible());
}

//--------------------------------------
void MenuBehaviorLeanerMove::Execute()
{
	if (!anim.IsOn())
	{
		anim.Start();
		timer.reset(new math::Timer([this]()->bool
			{
				std::vector<glm::vec3> frame = anim.getCurrentFrame();
				gui->Move({ frame[0].x, frame[0].y });
				return true;
			}));
		timer->start(10);
		gui->SetVisible(true);
	}
	else
	{
		gui->SetVisible(false);
		anim.Reset();
		timer->stop();
	}
}

MenuBehaviorLeanerMove::~MenuBehaviorLeanerMove()
{
	if (anim.IsOn())
	{
		gui->SetVisible(false);
		anim.Reset();
		timer->stop();
	}
}