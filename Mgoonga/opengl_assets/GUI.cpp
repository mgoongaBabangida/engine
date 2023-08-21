#include "stdafx.h"
#include "GUI.h"
#include <math/Rigger.h>

#include <memory>

//-------------------------------------------------------
GUI::GUI()
{
}

//-------------------------------------------------------
GUI::GUI(int topleftX, int topleftY, int Width, int Height, int scWidth, int scHeight)
	: topleftX(topleftX),
		topleftY(topleftY),
		Width(Width),
		Height(Height),
		screenWidth(scWidth),
		screenHeight(scHeight)
	{}

//-------------------------------------------------------
GUI::GUI(const dbb::Rect& _rect, int scWidth, int scHeight)
	: topleftX(_rect.m_top_left.x),
		topleftY(_rect.m_top_left.y),
		Width(_rect.m_size.x),
		Height(_rect.m_size.y),
		screenWidth(scWidth),
		screenHeight(scHeight)
{}

//-------------------------------------------------------
GUI::GUI(const GUI& _other)
	: screenWidth(_other.screenWidth)
    , screenHeight(_other.screenHeight)
    , texture(_other.texture)
	  , textureMask(_other.textureMask)
    , topleftX(_other.topleftX)
    , topleftY(_other.topleftY)
    , Width(_other.Width)
    , Height(_other.Height)
    , cmd(_other.cmd)
    , m_is_visible(_other.m_is_visible)
{
}

//-------------------------------------------------------
bool GUI::OnMousePress(int32_t x, int32_t y, bool left)
{
	if(left && m_is_visible && isPressed(x, y))
	{
		m_is_pressed = true;
		m_press_coords = { x - topleftX , y - topleftY };
		Perssed();
		return true;
	}
	return false;
}

//-------------------------------------------------------------
bool GUI::OnMouseRelease()
{
	if (m_is_pressed)
	{
		if (cmd && m_is_execute_on_release)
			cmd->Execute();
		m_is_pressed = false;
		return true;
	}
	return false;
}

//-------------------------------------------------------
bool GUI::OnMouseMove(int32_t _x, int32_t _y)
{
	if (m_is_pressed && m_take_mouse_moves && m_is_moveble2d)
		Move({ _x - m_press_coords.first, _y - m_press_coords.second });

	if (isHover(_x, _y) && m_is_visible)
		return m_take_mouse_moves;
	else
		return false;
}

//-------------------------------------------------------
void GUI::UpdateSync()
{

}

//-------------------------------------------------------
void GUI::setCommand(std::shared_ptr<ICommand> com)
{
	cmd = com;
}

//-------------------------------------------------------
void GUI::SetTexture(const Texture& t, glm::ivec2 topLeft, glm::ivec2 bottomRight)
{
	texture = t;
	tex_topleftX = topLeft.x;
	tex_topleftY = topLeft.y;
	tex_Width = bottomRight.x - topLeft.x;
	tex_Height = bottomRight.y - topLeft.y;
}

//-------------------------------------------------------
void GUI::SetTextureMask(const Texture& t)
{
	textureMask = t;
}

//-------------------------------------------------------
Texture * GUI::GetTexture()
{
	 return &texture;
}

//-------------------------------------------------------
Texture* GUI::GetTextureMask()
{
	return &textureMask;
}

//-------------------------------------------------------
void GUI::Perssed()
{
  if(cmd && !m_is_execute_on_release)
		cmd->Execute();
}

//-------------------------------------------------------
bool GUI::isHover(int x, int y)
{
	return x > topleftX && y > topleftY && x < (topleftX + Width) && y < (topleftY + Height);
}

//-------------------------------------------------------
bool GUI::isPressed(int x, int y)
{
	return x > topleftX && y > topleftY && x < (topleftX + Width) && y < (topleftY + Height);
}

//-------------------------------------------------------
glm::ivec4 GUI::getViewPort() const
{
	return glm::ivec4(topleftX, screenHeight - topleftY - Height, Width, Height);
}

//-------------------------------------------------------
glm::ivec2 GUI::getTopLeft() const
{
	return glm::ivec2(topleftX, topleftY);
}

//-------------------------------------------------------
glm::ivec2 GUI::getBottomRight() const
{
	return glm::ivec2(topleftX + Width, topleftY + Height);
}

//-------------------------------------------------------
glm::ivec2 GUI::getTopLeftTexture() const
{
	return glm::ivec2(tex_topleftX, tex_topleftY);
}

//-------------------------------------------------------
glm::ivec2 GUI::getBottomRightTexture() const
{
	return glm::ivec2(tex_topleftX + tex_Width, tex_topleftY + tex_Height);
}

//-------------------------------------------------------
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

		float x_coef = static_cast<float>(x) / static_cast<float>(Width);
		float y_coef = static_cast<float>(y) / static_cast<float>(Height);

		int32_t tex_x = tex_topleftX + (x_coef * tex_Width);
		int32_t tex_y = tex_topleftY + (y_coef * tex_Height);

		int32_t row = tex_y * elmes_per_line;
		int32_t col = tex_x * 4;

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
bool Cursor::OnMouseMove(int32_t x, int32_t y)
{
	Move({ x,y });
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
	m_obj->GetRigger()->Apply("Running", false);
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
				gui->SetVisible(true);
				return true;
			}));
		timer->start(10);
	}
	else
	{
		gui->SetVisible(false);
		timer->stop();
		anim.Reset();
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