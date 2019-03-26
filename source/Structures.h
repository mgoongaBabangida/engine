#pragma once

#include "Object.h"

struct Texture;
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

const float PI = 3.14159265359f;

static const glm::vec3 NONE{ glm::vec3(-100.0f, -100.0f, -100.0f) };

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

struct eCollision
{
	eObject*  collider;
	eObject*  collidee;
	glm::vec3 intersaction;
	glm::mat3 triangle;
};

enum ASCII
{
	ASCII_W = 119,//87,
	ASCII_S = 115,//83
	ASCII_D = 100,//68,
	ASCII_A = 97, //65,
	ASCII_R = 114,//82,
	ASCII_F = 102,//70,
	ASCII_Q = 113,//81
	ASCII_J = 106,//74,
	ASCII_L = 108,//76,
	ASCII_K = 107,//75,
	ASCII_I = 105,//73,
	ASCII_Z = 122,//90,
	ASCII_X = 120,//88,
	ASCII_C = 99,//67,
	ASCII_V = 118,//86,
	ASCII_B = 98,//66,
	ASCII_N = 110,//78,
	ASCII_U = 117,//85,
	ASCII_H = 104,//72,
	ASCII_G = 103,//71,
};