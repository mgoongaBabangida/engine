#pragma once
#include <assimp-3.1.1/include/assimp/Importer.hpp>
#include <assimp-3.1.1/include/assimp/scene.h>
#include <assimp-3.1.1/include/assimp/postprocess.h>

#include <vector>
#include <string>
#include <iostream>

#include "AssimpMesh.h"
#include "MyModel.h"
#include <math/AnimatedModel.h>
#include <math/Bone.h>

using namespace std;

#define NUM_BONES_PER_VEREX 4 

//------------------------------------------------------
class Model: public eAnimatedModel
{
public:
	
  Model(GLchar* path, bool _m_invert_y_uv = false)
    : m_invert_y_uv(_m_invert_y_uv)
	{
		this->loadModel(path);
	}
  virtual ~Model();

	virtual void Draw() override;

  virtual size_t                    GetVertexCount() const override;
  virtual size_t                    GetMeshCount() const  override { return meshes.size();}
  virtual std::vector<const IMesh*> GetMeshes() const override;
  virtual size_t                    GetAnimationCount() const override { return m_animations.size(); }
  virtual std::vector<const IAnimation*> GetAnimations() const override;

  public: //getters
	std::string						          RootBoneName();
	std::vector<Bone>				        Bones()				const { return m_bones; }
	std::vector<SceletalAnimation>	Animations()		const { return m_animations; }
	glm::mat4						            GlobalTransform()	const { return m_GlobalInverseTransform; }

  virtual std::vector<glm::vec3>	GetPositions()	const override;
  virtual std::vector<GLuint>		  GetIndeces()	const override;

private:
	/*  Model Data  */
	vector<AssimpMesh>	meshes;
	string				      directory;

	/*  Functions   */
  void							              loadModel(string path);
  void							              processNode(aiNode* node, const aiScene* scene);
	AssimpMesh						          processMesh(aiMesh* mesh, const aiScene* scene);
	vector<Texture>					        loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
	
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

    void DebugWeights()
    {
      float sum = 0.0f;
      for (int i = 0; i < NUM_BONES_PER_VEREX; ++i)
      {
        sum += Weights[i];
      }
      std::cout << "bonData " << numTries << " " << sum << " " << 1.0f / sum << " " << std::endl;
    }
	};

private:
	Assimp::Importer*				m_import; //?
	aiScene*						    m_scene; //?
	
	std::vector<Bone>				      m_bones;
	Bone*							            root_bone	= nullptr;
	int								            m_NumBones	= 0;
	std::map<std::string, int>		m_BoneMapping;

	std::vector<SceletalAnimation>	m_animations;
	glm::mat4						            m_GlobalInverseTransform;
  bool m_invert_y_uv = false;

	void							loadNodesToBone(aiNode * node);
	void							loadBoneChildren(aiNode* node);
	SceletalAnimation	ProccessAnimations(const aiAnimation* anim);
	void							updateAnimation(Bone & bone,const Frame& frame, const glm::mat4 & ParentTransform);

private:
	//Dumpers////
	std::vector<std::string>		DumpAiMeshes();
	std::vector<std::string>		DumpAiMesh(aiMesh* mesh);
	std::vector<std::string>		DumpAiNodes();
	void							DumpAiNode(aiNode* node, std::vector<std::string>& BoneNames);
	void							DumpBone();
	void							DumpAiAnimation(const aiAnimation * anim);
	void							DumpAssimpMeshes();
};
