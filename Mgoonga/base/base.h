#pragma once

#include <functional>
#include <array>
#include <variant>

#pragma warning( disable : 4251) // vector & unique_ptr have to be exported or not used @todo
#pragma warning( disable : 4275) // non-dll class I- used as base for dll-interface
#pragma warning( disable : 4273) // inconsistent dll-linkage

#ifdef DLLDIR_EX
#define DLL_BASE __declspec(dllexport)
#else
#define DLL_BASE __declspec(dllimport)
#endif

class eObject;

#include <glm\glm\glm.hpp>

struct Texture;

struct TextureInfo
{
	TextureInfo(const std::string& _type, const std::string& _path)
		: m_type(_type)
		, m_path(_path)
	{}
	std::string m_type;
	std::string m_path; //variant path or name in manager ? or enum ?
};

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
	glm::vec3  ambient  = { 0.1f, 0.1f, 0.1f };
	glm::vec3  diffuse  = { 0.45f, 0.45f, 0.45f };
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

constexpr float PI = 3.1415926536f;

static constexpr glm::vec3 NONE{ glm::vec3(-100.0f, -100.0f, -100.0f) }; //@todo

constexpr glm::vec3 XAXIS		= glm::vec3(1.0f, 0.0f, 0.0f);
constexpr glm::vec3 YAXIS		= glm::vec3(0.0f, 1.0f, 0.0f);
constexpr glm::vec3 ZAXIS		= glm::vec3(0.0f, 0.0f, 1.0f);

constexpr glm::mat4 UNIT_MATRIX = glm::mat4(1.0f, 0.0f, 0.0f, 0.0f,
																						0.0f, 1.0f, 0.0f, 0.0f,
																						0.0f, 0.0f, 1.0f, 0.0f,
																						0.0f, 0.0f, 0.0f, 1.0f);

constexpr int32_t MAX_BONES = 100;

//-------------------------------------------------------
struct Vertex
{
	glm::vec3		Position;	//0
	glm::vec3		Normal;		//2
	glm::vec2		TexCoords;	//3
	glm::vec3		tangent;	//4
	glm::vec3		bitangent;	//5
	glm::ivec4	boneIDs{ 0, 0 ,0 ,0 };	//6
	glm::vec4		weights{ 0.0f, 0.0f ,0.0f ,1.0f };	//7
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
	float MaxX = -1000;//@todo max min float ?
	float MinX = 1000;
	float MaxY = -1000;
	float MinY = 1000;
	float MaxZ = -1000;
	float MinZ = 1000;
};

//--------------------------------------------------------
enum ePriority
{
	WEAK,
	STRONG,
	MONOPOLY,
	ALWAYS
};

//--------------------------------------------------------
struct Text
{
	std::string content;
	std::string font;
	glm::vec3 color;
	float pos_x;
	float pos_y;
	float scale;
	glm::mat4 mvp;
};

//--------------------------------------------------------
struct AnimationSocket
{
	eObject* m_socket_object = nullptr;
	std::string m_bone_name;
	unsigned int m_bone_id;
	glm::mat4 m_pre_transform;
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
enum class KeyModifiers : uint32_t
{
	NONE = 0,
	SHIFT = 1,
	CTRL = 2,
	CTRL_SHIFT = 3
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
