#ifndef BASE_SCRIPT_H
#define BASE_SCRIPT_H

#include <base/interfaces.h>
#include <vector>
#include <glm\glm\glm.hpp>
#include <math/Camera.h>

class eObject;
struct Flag;
struct Texture;

//-------------------------------------------------------------------
class eBaseScript : public IScript
{
public:
	eBaseScript(Texture* flag_texture);

	virtual void	Update(std::vector<std::shared_ptr<eObject> > objs) override {}
	Flag			GetFlag(const Camera& camera);

protected:
	Texture*		  flag_tex;
	glm::vec3		  flag_scale = { 0.01f, 0.01f ,0.01f };
};

#endif // BASE_SCRIPT_H