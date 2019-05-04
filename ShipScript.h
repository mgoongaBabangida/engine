#ifndef SHIP_SCRIPT_H
#define SHIP_SCRIPT_H

#include "InterfacesDB.h"
#include <vector>
#include <memory>
#include <glm\glm\glm.hpp>
#include "Camera.h"
#include "CameraRay.h"

class eObject;
class eParticleRender;
struct Flag;
struct Texture;
class remSnd;

class eShipScript : public IScript
{
public:
	eShipScript(Texture*, 
			   eParticleRender*	prt,
			   Texture*			shoting_texture, 
			   remSnd*			shooting_sound, 
			   dbb::CameraRay*,  
			   float			waterHeight);
	
	virtual bool	OnKeyPress(uint32_t asci)							override;
	virtual bool	OnMousePress(uint32_t x, uint32_t y, bool left)		override;

	virtual void	Update(std::vector<std::shared_ptr<eObject> > objs) override;
	Flag			GetFlag(const Camera& camera);

protected:
	float			 turn_speed;
	float			 move_speed;
	glm::vec3		 destination = NONE;
	Texture*		 flag_tex;
	eParticleRender* prt_renderer;
	Texture*		 shoot_tex;
	remSnd*			 shoot_snd;
	float			 waterHeight;
	dbb::CameraRay*	 camRay;

	void			 SetDestination(glm::vec3 dst) { destination = dst; }
	void			 Shoot();
};

#endif // SHIP_SCRIPT_H