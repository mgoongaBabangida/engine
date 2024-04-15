#pragma once

#include "stdafx.h"

#include <assimp-3.1.1/include/assimp/Importer.hpp>
#include <assimp-3.1.1/include/assimp/scene.h>
#include <assimp-3.1.1/include/assimp/postprocess.h>

//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

#include "AssimpModel.h"

#define NUM_BONES_PER_VEREX 4

//--------------------------------------------------------------------
class AssimpLoader
{
public:
	AssimpLoader();
	~AssimpLoader();

	Model* LoadModel(char* path,
									const std::string& _name,
									bool _m_invert_y_uv = false);
protected:
	void										_ProcessNode(aiNode* node, const aiScene* scene);
	AssimpMesh							_ProcessMesh(aiMesh* mesh, const aiScene* scene);
	void										_LoadNodesToBone(aiNode* node);
	void										_LoadBoneChildren(aiNode* node);
	void										_ProccessAnimations(const aiAnimation* anim);
	std::vector<Texture>		_LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
	Material								_ProcessMaterial(aiMesh* mesh, std::vector<Texture>& textures);

protected:
	std::unique_ptr<Assimp::Importer>				m_import;
	std::unique_ptr<aiScene>								m_scene; //?
	Model*																	m_model = nullptr;
	bool																		m_invert_y_uv;
};