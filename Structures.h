#pragma once

#include <glm\glm.hpp>
#include <string>
#include "Object.h"
#include <memory>

typedef std::shared_ptr<eObject> shObject;

struct Material 
{
	glm::vec3	ambient;
	glm::vec3	diffuse;
	glm::vec3	specular;
	float		shininess;
};

struct Light 
{
	glm::vec4 light_vector;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct Flag
{
	glm::vec3	position;
	Texture*	tex;
	
	Flag(glm::vec3 pos, Texture* t) 
		:position(pos), tex(t) {}
};


struct ParticleSystemInfo
{
	glm::vec3	systemCenter;
	float		scale;
	Texture*	texture;
};

namespace str 
{
	const float PI = 3.14159265359;
}

const glm::vec3 XAXIS = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 YAXIS = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 ZAXIS = glm::vec3(0.0f, 0.0f, 1.0f);

struct MyVertex
{
	glm::vec3	position;	//0
	glm::vec3	color;		//1
	glm::vec3	Normal;		//2
	glm::vec2	TexCoords;	//3
	glm::vec3	tangent;	//4
	glm::vec3	bitangent;	//5
	glm::ivec4	boneIDs;	//6
	glm::vec4	weights{ 0.0f, 0.0f ,0.0f ,1.0f };	//7
};

struct AssimpVertex
{
	glm::vec3	Position;	//0
	glm::vec3	Normal;		//1
	glm::vec2	TexCoords;	//2
	glm::vec3	tangent;	//3
	glm::vec3	bitangent;	//4
	glm::ivec4	boneIDs;	//5
	glm::vec4	weights;	//6
};
