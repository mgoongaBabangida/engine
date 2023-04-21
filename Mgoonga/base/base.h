﻿#pragma once

#include <functional>
#include <array>
#include <variant>

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
	glm::vec3 albedo;
	float     metallic = 0.0f;
	float     roughness = 0.0f;
	float     ao = 1.0f;

	uint32_t albedo_texture_id = -1;
	uint32_t metalic_texture_id = -1;
	uint32_t normal_texture_id = -1;
	uint32_t roughness_texture_id = -1;
	uint32_t emissive_texture_id = -1;

	bool use_albedo = false;
	bool use_metalic = false;
	bool use_normal = false;
	bool use_roughness = false;

	bool use_phong_shading = false;
};

enum class eLightType { POINT, DIRECTION, SPOT};

//-------------------------------------------------------
struct Light
{
	eLightType type = eLightType::POINT;
	glm::vec4  light_position;
	glm::vec3  light_direction;
	glm::vec3  ambient = { 0.1f, 0.1f, 0.1f };
	glm::vec3  diffuse = { 0.45f, 0.45f, 0.45f };
	glm::vec3  specular = { 0.45f, 0.45f, 0.45f };

	float constant	= 1.0f;
	float linear		= 0.09f;
	float quadratic = 0.032f;
	float cutOff		= 0.3f;
	float outerCutOff = 0.0f;

	glm::vec3  intensity = { 50, 50, 50 };
};

//-------------------------------------------------------
using UniformData = std::variant<bool, float, int32_t, size_t,
	glm::vec2, glm::vec3, glm::vec4,
	glm::mat2, glm::mat3, glm::mat4>;

//-------------------------------------------------------
struct Uniform
{
	std::string name;
	int32_t location;
	int32_t type;
	UniformData data;
};

//----------------------------------------------------------
struct ShaderInfo
{
	std::string name;
	int32_t id;
	const std::vector<Uniform>& uniforms;
};

const float PI = 3.14159265359f;

static const glm::vec3 NONE{ glm::vec3(-100.0f, -100.0f, -100.0f) }; //@todo

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
struct eThreeFloat
{
	std::array<float, 3> data = { 0.f, 0.f, 0.f };
	float min, max;
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
