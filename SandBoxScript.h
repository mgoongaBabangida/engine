#ifndef SAND_BOX_SCRIPT_H
#define SAND_BOX_SCRIPT_H

#include "InterfacesDB.h"
#include <vector>
#include "Clock.h"

class eSandBoxScript : public IScript
{
public:
	virtual void	Update(std::vector<std::shared_ptr<eObject>> objs);
	virtual void	CollisionCallback(const eCollision&);
protected:
	eClock			clock;
	float			g = 0.00098f;//per millisecond
};

#endif // SAND_BOX_SCRIPT_H