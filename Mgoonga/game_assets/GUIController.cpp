#include "stdafx.h"

#include "GUIController.h"
#include "MainContextBase.h"

#include <math/Rect.h>

#include <opengl_assets/Sound.h>
#include <opengl_assets/GUI.h>
#include <opengl_assets/openglrenderpipeline.h>

//-------------------------------------------------------------
GUIController::GUIController(eMainContextBase* _game, eOpenGlRenderPipeline& _pipeline, RemSnd* _pageg_sound)
	: m_game(_game)
	, m_pipeline(_pipeline)
	, m_page_sound(_pageg_sound)
{
}

//-------------------------------------------------------------
void GUIController::Initialize()
{
	// init gui
	glm::vec2 menu_size = { 600 , 400 };
	float pos_x = m_game->Width() / 2 - menu_size.x / 2;
	float pos_y = m_game->Height() / 2 - menu_size.y / 2;

	const Texture* menu_tex = m_game->GetTexture("menu3");
	const Texture* mask_tex = m_game->GetTexture("menu2");
	std::shared_ptr<GUI> menu = std::make_shared<GUI>(pos_x, pos_y, menu_size.x, menu_size.y, m_game->Width(), m_game->Height());
	menu->SetTexture(*menu_tex, { 0,0 }, { menu_tex->mTextureWidth, menu_tex->mTextureHeight });
	menu->SetTextureMask(*mask_tex);
	menu->SetRenderingFunc(GUI::RenderFunc::CursorFollow);
	m_game->AddGUI(menu);
	m_game->AddInputObserver(menu.get(), MONOPOLY);

	//inverted logic for y-axis makes it confusing
	m_buttons.push_back(dbb::Rect{ glm::vec2{260 + pos_x, 510 - pos_y}, glm::vec2{90, 35} });
	m_buttons.push_back(dbb::Rect{ glm::vec2{260 + pos_x, 450 - pos_y}, glm::vec2{95, 40} });
	m_buttons.push_back(dbb::Rect{ glm::vec2{260 + pos_x, 380 - pos_y}, glm::vec2{90, 35} });
	m_buttons.push_back(dbb::Rect{ glm::vec2{260 + pos_x, 325 - pos_y}, glm::vec2{85, 30} });

	menu->setCommand(std::make_shared<GUICommand>([this, menu]()
		{
			for (auto& rect : m_buttons)
			{
				if (rect.IsInside({ m_cursor_x , m_cursor_y }))
				{
					menu->SetVisible(false); // just close the menu if any button is pressed for now
					m_is_menu_active = false;
				}
			}
		}));

	const Texture* cursor_tex = m_game->GetTexture("cursor1");
	std::shared_ptr<GUI> cursor = std::make_shared<Cursor>(0, 0, 30, 30, m_game->Width(), m_game->Height());
	cursor->SetTexture(*cursor_tex, { 0,0 }, { cursor_tex->mTextureWidth, cursor_tex->mTextureHeight });
	cursor->SetTransparent(true);
	m_game->AddGUI(cursor);
	m_game->AddInputObserver(cursor.get(), ALWAYS);

	m_game->AddInputObserver(this, ALWAYS);
}

//-------------------------------------------------------------
GUIController::~GUIController()
{
}

//-------------------------------------------------------------
void GUIController::Update(float _tick)
{
	if(m_is_menu_active)
		m_pipeline.get().SetUniformData("class eScreenRender", "CursorPos", glm::vec2(m_cursor_x, m_cursor_y));
}

//----------------------------------------------------------------
bool GUIController::OnMouseMove(int32_t _x, int32_t _y, KeyModifiers _modifier)
{
	// it is probably safer to pass in relative coordinates
	/*float curPosX = (float(_x) / m_game->Width());
	float curPosY = (float(_y) / m_game->Height());*/
	m_cursor_x = _x;
	m_cursor_y = m_game->Height() - _y;

	if (m_is_menu_active)
	{
		for (auto& rect : m_buttons)
		{
			if (rect.IsInside({ m_cursor_x , m_cursor_y }))
			{
				if (m_cursor_is_outside_buttons)
				{
					m_cursor_is_outside_buttons = false;
					m_page_sound->Play();
				}
				return false;
			}
		}

		if (!m_cursor_is_outside_buttons)
			m_cursor_is_outside_buttons = true;
	}

	return false;
}
