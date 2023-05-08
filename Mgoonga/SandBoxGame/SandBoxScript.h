#ifndef SAND_BOX_SCRIPT_H
#define SAND_BOX_SCRIPT_H
#include "stdafx.h"

#include <base/interfaces.h>
#include <math/Clock.h>

//------------------------------------------------------------------
class eSandBoxScript : public IScript
{
public:
	virtual void	Update(std::vector<std::shared_ptr<eObject>> objs);
	virtual void	CollisionCallback(const eCollision&);
protected:
	math::eClock	clock;
	float			g = 0.000098f;//per millisecond
};

#endif // SAND_BOX_SCRIPT_H