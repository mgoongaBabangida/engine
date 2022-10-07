#ifndef SHIP_SCRIPT_H
#define SHIP_SCRIPT_H
#include "game_assets.h"

#include <base/interfaces.h>

#include <vector>
#include <memory>

#include <math/Camera.h>
#include <math/CameraRay.h>

class eObject;
struct Flag;
struct Texture;
class RemSnd;
class eRenderManager;

//----------------------------------------------------------------------------------
class DLL_GAME_ASSETS eShipScript : public IScript
{
public:
	eShipScript(Texture*		flag_texture,
			        eRenderManager&	render_manager,
		          Camera& camera,
			        Texture*			shoting_texture,
			        RemSnd*			shooting_sound,
			        dbb::CameraRay*,
			        float			waterHeight);
	
	eShipScript(const eShipScript&)				= delete;
	eShipScript& operator=(const eShipScript&)	= delete;

	virtual bool	OnKeyPress(uint32_t asci)							override;
	virtual bool	OnMousePress(uint32_t x, uint32_t y, bool left)		override;

	virtual void	Update(std::vector<std::shared_ptr<eObject> > objs) override;
	virtual std::vector<eObject*>			GetChildrenObjects() override;

	void			SetDestination(glm::vec3 dst) { destination = dst; }
	
	void			SetShootAfterMove(bool shoot) { shoot_after_move = shoot;}
	void			Shoot();

protected:
	float			  waterHeight;
	float			  turn_speed;
	float			  move_speed;
	glm::vec3		  destination		= NONE;
	bool			  shoot_after_move  = false;

  Texture* flag_tex;
  glm::vec3		  flag_scale = { 0.01f, 0.01f ,0.01f };
  std::unique_ptr<eObject> flag;

	std::reference_wrapper<eRenderManager> render_manager;
	std::reference_wrapper<Camera> camera;
	Texture*							   shoot_tex;
	RemSnd*								   shoot_snd;

	dbb::CameraRay*	  camRay;
};

#endif // SHIP_SCRIPT_H