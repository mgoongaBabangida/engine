#pragma once

#include "Camera.h"
#include "InterfacesDB.h"

class eObject;
class eParticleRender;
struct Flag;
struct Texture;
class remSnd;

class MoveScript : public IScript
{
protected:
	//static const glm::vec3 (-100.0f, -100.0f, -100.0f); //!?
	float turn_speed;
	float move_speed;
	glm::vec3 destination = glm::vec3(-100.0f, -100.0f, -100.0f);
	Texture* flag_tex;
	eParticleRender* prt_renderer;
	Texture* shoot_tex;
	remSnd* shoot_snd;
public:
	MoveScript(Texture*t, eParticleRender* prt, Texture*shoting_texture, remSnd* shooting_sound);
	virtual void Update(std::vector<std::shared_ptr<eObject> > objs);
	virtual void setDestination(glm::vec3 dst);
	Flag* getFlag(const Camera& camera);
	virtual void shoot();
};
