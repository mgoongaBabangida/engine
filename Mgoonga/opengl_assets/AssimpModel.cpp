#include "stdafx.h"

#include "AssimpModel.h"
#include <math/Transform.h>

#include <algorithm>
#include <assert.h>

using namespace std;

//---------------------------------------------------------------------------
std::string Model::RootBoneName()
{
	return m_root_bone->GetName();
}

//---------------------------------------------------------------------------
Model::Model(GLchar* _path, const std::string& _name)
	: m_name(_name)
{
	std::string path(_path);
	this->m_directory = path.substr(0, path.find_last_of('/'));
}

// -------------------------------------------------------------------------- -
Model::Model(const std::string& _name, std::vector<AssimpMesh>&& _meshes, std::vector<Bone> _bones)
	: m_name(_name), m_meshes(std::move(_meshes)), m_bones(_bones)
{
	//@ ? extra init,? extra paramenetrs
}

//---------------------------------------------------------------------------
Model::~Model()
{
	for (auto& mesh : m_meshes)
		mesh.FreeTextures();
}

//---------------------------------------------------------------------------
void Model::Draw()
{
	for (GLuint i = 0; i < this->m_meshes.size(); ++i)
		this->m_meshes[i].Draw();
}

//---------------------------------------------------------------------------
void Model::DrawInstanced(int32_t _instances)
{
	for (GLuint i = 0; i < this->m_meshes.size(); ++i)
		this->m_meshes[i].DrawInstanced(_instances);
}

//---------------------------------------------------------------------------
void	Model::SetUpMeshes()
{
	for (auto& mesh : m_meshes)
		mesh.SetupMesh();
}

//---------------------------------------------------------------------------
void Model::ReloadTextures()
{
	for (auto& mesh : m_meshes)
		mesh.ReloadTextures();
}

//---------------------------------------------------------------------------
size_t Model::GetVertexCount() const
{
  size_t count = 0;
  for (auto& mesh : m_meshes)
    count += mesh.vertices.size();
  return count;
}

//----------------------------------------------------------------------------
std::vector<const I3DMesh*> Model::Get3DMeshes() const
{
	std::vector<const I3DMesh*> ret;
	for (const AssimpMesh& mesh : m_meshes)
		ret.push_back(&mesh);
	return ret;
}

//----------------------------------------------------------------------------
std::vector<const IMesh*> Model::GetMeshes() const
{
  std::vector<const IMesh*> ret;
  for (const AssimpMesh& mesh : m_meshes)
    ret.push_back(&mesh);
  return ret;
}

//----------------------------------------------------------------------------
std::vector<const IBone*> Model::GetBones() const
{
	std::vector<const IBone*> bones;
	for (auto& bone : m_bones)
		bones.push_back(&bone);
	return bones;
}

//----------------------------------------------------------------------------
std::vector<const IAnimation*> Model::GetAnimations() const
{
  std::vector<const IAnimation*> ret;
  for (auto& anim : m_animations)
  {
    ret.push_back(&anim);
  }
  return ret;
}

//-------------------------------------------------------------------------------------------
void Model::mapMehsesToNodes()
{
	for (int32_t i = 0; i < m_meshes.size(); ++i)
	{
		std::vector<Bone>::iterator meshBoneIter = std::find_if(m_bones.begin(), m_bones.end(), [this, i](const Bone& bone)
			{ return bone.GetName() == m_meshes[i].Name(); });
		if (meshBoneIter != m_bones.end())
		{
			for (int32_t j = 0; j < m_meshes[i].vertices.size(); ++j)
			{
				m_meshes[i].vertices[j].boneIDs[0] = (glm::i32)meshBoneIter->GetID();
				m_meshes[i].vertices[j].weights[0] = 1.0f;
			}
			m_meshes[i].ReloadVertexBuffer();
		}
	}
}

//------------------------------------------------------------------------------------------------
Texture Model::LoatTexture(std::string _fileName, std::string _typeName)
{
	//@todo this is temp for pirate ship need to make better solution
	std::string substring_to_delete = "C:\\\\Users\\\\58sal\\\\Desktop\\\\stylized_ship";
	std::string::size_type it = _fileName.find(substring_to_delete);
	if (it != std::string::npos)
		_fileName.erase(it, substring_to_delete.length());

	Texture texture;
	_fileName = m_directory + '/' + _fileName;
	texture.type = _typeName;
	texture.path = _fileName;
	texture.loadTextureFromFile(_fileName.c_str());
	return texture;
}

//----------------------------------------------------------------------------------------
void Model::updateAnimation(Bone &bone, const Frame& frame, const glm::mat4 &ParentTransform)
{
	glm::mat4 currentLocalTransform;
	if(frame.exists(bone.GetName()))
		currentLocalTransform = frame.pose.find(bone.GetName())->second.getModelMatrix() ;
	else
		currentLocalTransform =	bone.GetMTransform();
	glm::mat4 globalTransform =   ParentTransform * currentLocalTransform;

	//glm::mat4 totalTransform = currentTransform * bone.getInverseBindTransform(); //ThinMatrix
	glm::mat4 totalTransform = m_GlobalInverseTransform * globalTransform * bone.GetLocalBindTransform(); // OGLDev
	bone.setAnimatedTransform(totalTransform);

	for (int i = 0; i<bone.NumChildren(); ++i)
		updateAnimation(*(bone.getChildren()[i]), frame, globalTransform);
}

//-----------------------------------------------------------------------
void Model::VertexBoneData::AddBoneData(int BoneID, float Weight)
 {
		++numTries;
		for (int i = 0; i < NUM_BONES_PER_VEREX; i++)
		{
			//ARRAY_SIZE_IN_ELEMENTS(IDs)
			if (Weights[i] == 0.0)
			{
				IDs[i] = BoneID;
				Weights[i] = Weight;
				return;
			}
		}
		// should never get here - more bones than we have space for 
		//  assert(0);
}

IModel* MakeModel(const std::string& _name, std::vector<AssimpMesh>&& _meshes, std::vector<Bone> _bones)
{
	return new Model(_name, std::move(_meshes), _bones);
}
