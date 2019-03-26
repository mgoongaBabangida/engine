
#ifndef BOX_COLLIDER_H
#define BOX_COLLIDER_H

#include "Transform.h"
#include "PlaneLine.h"

struct eCollision;

struct extremDots 
{
	float MaxX;
	float MinX;
	float MaxY;
	float MinY;
	float MaxZ;
	float MinZ;
};

enum Side
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

class BoxCollider
{
private:
	extremDots dots;
	//bool dynamic = false; ?
	std::vector<dbb::line> getRays(const Transform& trans, Side moveDirection, std::vector<float>& lengths)	const;
	void getForwardRayLengths(const Transform & trans, Side moveDirection, std::vector<float>& lengths)		const;

public:
	void calculateExtremDots(const std::vector<glm::vec3>& positions);
	bool collidesWith(const Transform& trans1, const Transform& trans2, const BoxCollider& other, Side moveDirection, eCollision& collision);
	float getMaxX()const { return dots.MaxX; }
	float getMaxY()const { return dots.MaxY; }
	float getMaxZ()const { return dots.MaxZ; }
	float getMinX()const { return dots.MinX; }
	float getMinY()const { return dots.MinY; }
	float getMinZ()const { return dots.MinZ; }
	glm::vec3 getCenter() const;
	std::vector<glm::mat3> getBoundingTriangles(const Transform& trans)const;
	std::vector<glm::vec3> getExtrems(const Transform& trans) const;
};

#endif