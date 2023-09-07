#ifndef SHIP_SCRIPT_H
#define SHIP_SCRIPT_H

#include "game_assets.h"

#include <base/interfaces.h>
#include <math/Camera.h>
#include <math/CameraRay.h>

class eObject;
struct Flag;
struct Texture;
class RemSnd;
class eOpenGlRenderPipeline;

//----------------------------------------------------------------------------------
class DLL_GAME_ASSETS eShipScript : public IScript
{
public:
	eShipScript(IGame*									game,
							const Texture*					flag_texture,
							eOpenGlRenderPipeline&	render_manager,
		          Camera&									camera,
			        const Texture*					shoting_texture,
			        RemSnd*									shooting_sound,
			        float										waterHeight);
	
	~eShipScript();

	eShipScript(const eShipScript&)				= delete;
	eShipScript& operator=(const eShipScript&)	= delete;

	virtual bool	OnKeyPress(uint32_t asci, KeyModifiers _modifier)												override;
	virtual bool	OnMousePress(int32_t x, int32_t y, bool left, KeyModifiers _modifier)		override;

	virtual void	Update(float _tick) override;

	//specific
	void			SetDestination(glm::vec3 dst) { destination = dst; }
	glm::vec3 GetDestination() const { return  destination; }

	void			SetShootAfterMove(bool shoot) { shoot_after_move = shoot;}
	void			Shoot();

protected:
	void _UpdateFlagPos();

	IGame*			m_game = nullptr;
	float			  waterHeight;
	float			  turn_speed;
	float			  move_speed;
	glm::vec3		destination		= NONE;
	bool			  shoot_after_move  = false;

  const Texture*						flag_tex;
  glm::vec3									flag_scale = { 0.01f, 0.01f ,0.01f };
	std::shared_ptr<eObject>	m_flag;

	std::reference_wrapper<eOpenGlRenderPipeline> pipeline;
	std::reference_wrapper<Camera> camera;
	const Texture*						shoot_tex;
	RemSnd*								   shoot_snd;
};

#endif // SHIP_SCRIPT_H
