#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <array>

#pragma warning( disable : 4251) // vector & unique_ptr have to be exported or not used @todo

#ifdef DLLDIR_EX
#define DLL_BASE __declspec(dllexport)
#else
#define DLL_BASE __declspec(dllimport)
#endif

class eObject;

#include <glm\glm\glm.hpp>

struct Texture;

//-------------------------------------------------------
struct Material
{
	Material() = default;
	~Material() noexcept = default;

	glm::vec3 ambient;
	glm::vec3 diffuse; // or albedo
	glm::vec3 specular;
	float     shininess;

	float     metallic = 0.0f;
	float     roughness = 0.0f;
	float     ao = 1.0f;
};

enum class eLightType { POINT, DIRECTION, SPOT};

//-------------------------------------------------------
struct Light
{
	glm::vec4  light_position;
	glm::vec4  light_direction;
	glm::vec3  ambient = { 1.0f, 1.0f, 1.0f };
	glm::vec3  diffuse = { 1.0f, 1.0f, 1.0f };
	glm::vec3  specular = { 1.0f, 1.0f, 1.0f };
	eLightType type = eLightType::POINT;
};

const float PI = 3.14159265359f;

static const glm::vec3 NONE{ glm::vec3(-100.0f, -100.0f, -100.0f) };

const glm::vec3 XAXIS		= glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 YAXIS		= glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 ZAXIS		= glm::vec3(0.0f, 0.0f, 1.0f);

const glm::mat4 UNIT_MATRIX = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
                                        0.0f, 1.0f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.0f, 0.0f, 0.0f, 1.0f);

//-------------------------------------------------------
struct MyVertex
{
	glm::vec3	position;	  //0
	glm::vec3	color;		  //1
	glm::vec3	Normal;		  //2
	glm::vec2	TexCoords;	//3
	glm::vec3	tangent;	  //4
	glm::vec3	bitangent;	//5
	glm::ivec4	boneIDs{ 0, 0 ,0 ,0};		//6
	glm::vec4	weights{ 0.0f, 0.0f ,0.0f ,1.0f};	//7
};

//-------------------------------------------------------
struct AssimpVertex
{
	glm::vec3	Position;	//0
	glm::vec3	Normal;		//1
	glm::vec2	TexCoords;	//2
	glm::vec3	tangent;	//3
	glm::vec3	bitangent;	//4
	glm::ivec4	boneIDs;	//5 !?
	glm::vec4	weights{ 0.0f, 0.0f ,0.0f ,1.0f };	//6	!?
};

//-------------------------------------------------------
struct eCollision
{
	eObject*  collider;
	eObject*  collidee;
	glm::vec3 intersaction;
	glm::mat3 triangle;
};

//-------------------------------------------------------
enum Side
{
	FORWARD,
	BACK,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

//--------------------------------------------------------
struct extremDots
{
	float MaxX;
	float MinX;
	float MaxY;
	float MinY;
	float MaxZ;
	float MinZ;
};

//-------------------------------------------------------
//@todo translation from SDL to ASCII should be on controller side
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

//-------------------------------------------------------
struct eThreeFloatCallback
{
  std::array<float, 3> data = {0.f, 0.f, 0.f};
  std::function<void()> callback;
  float min, max;
};

//-------------------------------------------------------
struct eVectorStringsCallback
{
  std::vector<std::string> data;
  std::function<void(size_t)> callback;
};
