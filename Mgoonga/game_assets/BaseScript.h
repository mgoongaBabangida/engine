#ifndef BASE_SCRIPT_H
#define BASE_SCRIPT_H

#include "game_assets.h"

#include <base/interfaces.h>

#include <glm\glm\glm.hpp>

//@todo delete
//-------------------------------------------------------------------
class DLL_GAME_ASSETS eBaseScript : public IScript
{
public:
	eBaseScript(const Texture* flag_texture);
	virtual ~eBaseScript();

	virtual void	Update(float _tick) override;

protected:
	Texture*      tex;
	Texture*      normals;
	Texture*      height;
	const Texture*		  flag_tex;
	glm::vec3		  flag_scale = { 0.01f, 0.01f ,0.01f };
	eObject*			flag;
};

#endif // BASE_SCRIPT_H
