#pragma once

#include <base/interfaces.h>

#include "math.h"

//--------------------------------------------------------------------
class DLL_MATH Bone : public IBone
{
public:
	Bone(size_t index, std::string name, glm::mat4 localBindTransform, bool real = true)
		: index(index)
    , name(name)
    , localBindTransform(localBindTransform)
    , realBone(real)
  {}

	Bone() {}
	
	void												addChild(Bone* bone)													{ children.push_back(bone); }
	void												setAnimatedTransform(const glm::mat4& trans)	{ animatedTransform = trans; }

	const glm::mat4&						getAnimatedTransform() const						{ return animatedTransform; }
	glm::mat4										getInverseBindTransform()const					{ return inverseBindTransform; }

	virtual size_t							GetID()const														{ return index; }
	virtual const std::string&	GetName() const									{ return name; }
	virtual bool								IsRealBone() const { return realBone; }

	void												setMTransform(glm::mat4 trans) { mTransform = trans; }

	virtual const glm::mat4&		GetLocalBindTransform() const		{ return localBindTransform; }
	virtual const glm::mat4&		GetMTransform() const					{ return mTransform; }

	virtual  std::vector<const IBone*> GetChildren() const
	{
		std::vector<const IBone*> ret_children;
		for (auto& child : children)
			ret_children.push_back(child);

		return ret_children;
	}

	void calculateInverseBindTransform(const glm::mat4& ParentBindTransform)
	{
		glm::mat4 trans = ParentBindTransform * localBindTransform;
		inverseBindTransform = glm::inverse(trans);
		for (auto& child : children)
			child->calculateInverseBindTransform(trans);
	}

	size_t								NumChildren() const				{ return children.size(); }
	std::vector<Bone*>&		getChildren()							{ return children; }

protected:
	size_t								index;
	std::string						name;
	std::vector<Bone*>		children;
	glm::mat4							animatedTransform;
	glm::mat4							localBindTransform;
	glm::mat4							inverseBindTransform;
	glm::mat4							mTransform = UNIT_MATRIX; //for empty nodes
	bool									realBone = true;
};