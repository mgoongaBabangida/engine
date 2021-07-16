#pragma once

#include <glm\glm/gtx/euler_angles.hpp>
#include <glm\glm/gtc/quaternion.hpp>
#include <glm\glm/gtx/quaternion.hpp>
#include <glm\glm/gtx/norm.hpp>

#include "math.h"

#include <vector>
#include <string>

//--------------------------------------------------------------------
class DLL_MATH Bone
{
	size_t				index;
	std::string			name;
	std::vector<Bone*>	children;
	glm::mat4			animatedTransform;
	glm::mat4			localBindTransform;
	glm::mat4			inverseBindTransform;
	glm::mat4			mTransform = UNIT_MATRIX; //for empty nodes
	bool				realBone = true;

public:
	Bone(int index, std::string name, glm::mat4 localBindTransform, bool real = true)
		:index(index)
    , name(name)
    , localBindTransform(localBindTransform)
    , realBone(real)
  {}

	Bone() {}
	
	void				addChild(Bone* bone)					            { children.push_back(bone); }
	const glm::mat4&	getAnimatedTransform() const				{ return animatedTransform; }
	void				setAnimatedTransform(glm::mat4 trans)	    { animatedTransform = trans; }
	glm::mat4			getInverseBindTransform()const			    { return inverseBindTransform; }
	int					ID()const								                  { return index; }
	std::string			Name()const								            { return name; }
	
	void calculateInverseBindTransform(const glm::mat4 &ParentBindTransform) 
	{
		glm::mat4 trans = ParentBindTransform * localBindTransform;
		inverseBindTransform = glm::inverse(trans);
		for (auto &child : children)
			child->calculateInverseBindTransform(trans);
	}

	const glm::mat4&		getBindTransform()const			    { return localBindTransform; }
	const glm::mat4&		getMTransform()   const			    { return mTransform; }
	void					      setMTransform(glm::mat4 trans)	{ mTransform = trans; }

	size_t					      NumChildren() const				{ return children.size(); }
	std::vector<Bone*>		getChildren() const				{ return children; }
};