#ifndef SHIP_SCRIPT_H
#define SHIP_SCRIPT_H

#include <base/interfaces.h>

#include <vector>
#include <memory>

#include <math/Camera.h>
#include <math/CameraRay.h>
#include "BaseScript.h"

class eObject;
struct Flag;
struct Texture;
class RemSnd;
class eRenderManager;

//----------------------------------------------------------------------------------
class eShipScript : public eBaseScript
{
public:
	eShipScript(Texture*		flag_texture, 
			   eRenderManager&	render_manager,
			   Texture*			shoting_texture, 
			   RemSnd*			shooting_sound, 
			   dbb::CameraRay*,  
			   float			waterHeight);
	
	eShipScript(const eShipScript&)				= delete;
	eShipScript& operator=(const eShipScript&)	= delete;

	virtual bool	OnKeyPress(uint32_t asci)							override;
	virtual bool	OnMousePress(uint32_t x, uint32_t y, bool left)		override;

	virtual void	Update(std::vector<std::shared_ptr<eObject> > objs) override;

	void			SetDestination(glm::vec3 dst) { destination = dst; }
	
	void			SetShootAfterMove(bool shoot) { shoot_after_move = shoot;}
	void			Shoot();

protected:
	float			  waterHeight;
	float			  turn_speed;
	float			  move_speed;
	glm::vec3		  destination		= NONE;
	bool			  shoot_after_move  = false;

	std::reference_wrapper<eRenderManager> render_manager;
	Texture*							   shoot_tex;
	RemSnd*								   shoot_snd;

	dbb::CameraRay*	  camRay;
};

#endif // SHIP_SCRIPT_H