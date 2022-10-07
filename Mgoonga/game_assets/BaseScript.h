#ifndef BASE_SCRIPT_H
#define BASE_SCRIPT_H

#include "game_assets.h"

#include <base/interfaces.h>
#include <vector>
#include <glm\glm\glm.hpp>
#include <math/Camera.h>

class eObject;
struct Texture;

//-------------------------------------------------------------------
class DLL_GAME_ASSETS eBaseScript : public IScript
{
public:
	eBaseScript(Texture* flag_texture);

	virtual void	Update(std::vector<std::shared_ptr<eObject> > objs) override {}

protected:
	Texture*      tex;
	Texture*      normals;
	Texture*      height;
	Texture*		  flag_tex;
	glm::vec3		  flag_scale = { 0.01f, 0.01f ,0.01f };
	std::unique_ptr<eObject> flag;
};

#endif // BASE_SCRIPT_H