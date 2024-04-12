#pragma once

#include "opengl_assets.h"

#include "AssimpMesh.h"

#include <math/AnimatedModel.h>
#include <math/Bone.h>

#define NUM_BONES_PER_VEREX 4

DLL_OPENGL_ASSETS IModel* MakeModel(const std::string& _name, std::vector<AssimpMesh>&& _meshes, std::vector<Bone> _bones);

//------------------------------------------------------
class /*DLL_OPENGL_ASSETS*/ Model: public eAnimatedModel //I not e?
{
	friend class AssimpLoader;
public:
	Model(char* path, const std::string& _name);
	Model(const std::string& _name, std::vector<AssimpMesh>&&	_meshes, std::vector<Bone> _bones);
  virtual ~Model();

	virtual void Draw();
	virtual void DrawInstanced(int32_t _instances) override;

	virtual void SetUpMeshes() override;
	virtual void ReloadTextures() override;

	virtual const std::string& GetName() const override { return m_name; }
	virtual const std::string& GetPath() const override { return m_directory; }

  virtual size_t													GetVertexCount() const override;
  virtual size_t													GetMeshCount() const  override { return m_meshes.size();}
	virtual std::vector<const I3DMesh*>			Get3DMeshes() const override;
	virtual std::vector<const IMesh*>				GetMeshes() const override;

	virtual bool														HasBones() const { return !m_bones.empty(); }
	virtual std::vector<const IBone*>				GetBones() const;

  virtual size_t													GetAnimationCount() const override { return m_animations.size(); }
  virtual std::vector<const IAnimation*>	GetAnimations() const override;

	// Animated Model
	std::string														RootBoneName();
	std::vector<Bone>											Bones()				const { return m_bones; }
	const std::vector<SceletalAnimation>&	Animations()		const { return m_animations; }
	glm::mat4															GlobalTransform()	const { return m_GlobalInverseTransform; }

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
	/*  Model Data  */
	std::vector<AssimpMesh>		m_meshes;
	std::string								m_directory;
	std::string								m_name;

	/*  Bone Data  */
	std::vector<Bone>				      m_bones;
	Bone*							            m_root_bone	= nullptr;
	int								            m_NumBones	= 0;
	std::map<std::string, int>		m_BoneMapping;

	/*  Animation Data  */
	std::vector<SceletalAnimation>	m_animations;
	glm::mat4						            m_GlobalInverseTransform;
  bool														m_invert_y_uv = false;
	bool														m_no_real_bones = false;

private:
	void												updateAnimation(Bone & bone,const Frame& frame, const glm::mat4 & ParentTransform);
	void												mapMehsesToNodes();
	Texture											LoatTexture(std::string _fileName, std::string _typeName);
};
