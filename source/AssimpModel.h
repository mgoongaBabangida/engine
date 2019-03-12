#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>
#include <GL/glew.h>
#include "AssimpMesh.h"
#include "MyModel.h"
#include "Bone.h"
#include "SceletalAnimation.h"

using namespace std;

#define NUM_BONES_PER_VEREX 4 

class Model: public IModel
{
public:
	/*  Functions   */
	Model(GLchar* path)
	{
		this->loadModel(path);
	}

	virtual void Draw() override;

private:
	/*  Model Data  */
	vector<AssimpMesh>	meshes;
	string				directory;

	/*  Functions   */
	void							loadModel(string path);
	void							processNode(aiNode* node, const aiScene* scene);
	AssimpMesh						processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture>					loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	virtual std::vector<glm::vec3>	getPositions()	const override;
	virtual std::vector<GLuint>		getIndeces()	const override;
	
	// VertexBoneData
	struct VertexBoneData
	{
		int IDs[NUM_BONES_PER_VEREX]{0,0,0,0};
		float Weights[NUM_BONES_PER_VEREX]{ 0.0f,0.0f,0.0f,0.0f };
		int numTries = 0;

		VertexBoneData()
		{
			Reset();
		};

		void Reset()
		{
			//ZERO_MEM(IDs);
			//ZERO_MEM(Weights);
		}

		void AddBoneData(int BoneID, float Weight);
		void DebugWeights() 
		{
			float sum = 0.0f;
			for (int i = 0; i < NUM_BONES_PER_VEREX; ++i)
			{
				sum += Weights[i];
			}
			std::cout << "bonData " << numTries << " " << sum << " " << 1.0f / sum << " " << std::endl;
		}

		void SetOneTotalWeight()
		{
			float sum = 0.0f;
			for (int i = 0; i < NUM_BONES_PER_VEREX; ++i)
			{
				sum += Weights[i];
			}
			float coef = 1.0f / sum;
			for (int i = 0; i < NUM_BONES_PER_VEREX; ++i)
			{
				Weights[i] *= coef;
			}
		}
	};

public:
	std::string						RootBoneName();
	std::vector<Bone>				Bones()				const { return m_bones; }
	std::vector<SceletalAnimation>	Animations()		const { return m_animations; }
	glm::mat4						GlobalTransform()	const { return m_GlobalInverseTransform; }

private:
	Assimp::Importer*				m_import; //?
	aiScene*						m_scene; //?
	
	std::vector<Bone>				m_bones;
	Bone*							root_bone	= nullptr;
	int								m_NumBones	= 0;
	std::map<std::string, int>		m_BoneMapping;

	std::vector<SceletalAnimation>	m_animations;
	glm::mat4						m_GlobalInverseTransform;

	void							loadNodesToBone(aiNode * node);
	void							loadBoneChildren(aiNode* node);
	SceletalAnimation				ProccessAnimations(const aiAnimation* anim);
	void							updateAnimation(Bone & bone,const Frame& frame, const glm::mat4 & ParentTransform);

	//Dumpers////
	std::vector<std::string>		DumpAiMeshes();
	std::vector<std::string>		DumpAiMesh(aiMesh* mesh);
	std::vector<std::string>		DumpAiNodes();
	void							DumpAiNode(aiNode* node, std::vector<std::string>& BoneNames);
	void							DumpBone();
	void							DumpAiAnimation(const aiAnimation * anim);
	void							DumpAssimpMeshes();
};
