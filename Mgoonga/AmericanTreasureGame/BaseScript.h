#ifndef BASE_SCRIPT_H
#define BASE_SCRIPT_H

#include <base/interfaces.h>
#include <base/Event.h>

#include <glm\glm\glm.hpp>

//-------------------------------------------------------------------
class eBaseScript : public IScript
{
public:
	eBaseScript(IGame* game,
		const Texture* flag_texture);
	virtual ~eBaseScript();

	virtual void	Update(float _tick) override;

	Event<std::function<void(std::shared_ptr<eObject> _object, const std::string& _base_name)>>	ObjectCameToBase;

protected:
	IGame*																m_game;
	const Texture*												flag_tex;
	glm::vec3															flag_scale = { 0.01f, 0.01f ,0.01f };
	eObject*															flag;
	std::vector<std::shared_ptr<eObject>> m_objects_on_base;
};

#endif // BASE_SCRIPT_H
