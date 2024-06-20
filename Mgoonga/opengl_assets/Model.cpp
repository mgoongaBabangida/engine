#include "stdafx.h"

#include "Model.h"
#include <math/Transform.h>

#include <algorithm>
#include <assert.h>

using namespace std;

//---------------------------------------------------------------------------
eModel::eModel(char* _path, const std::string& _name)
	: m_name(_name)
{
	std::string path(_path);
	this->m_directory = path.substr(0, path.find_last_of('/'));
}

// -------------------------------------------------------------------------- -
eModel::eModel(const std::string& _name, std::vector<eMesh>&& _meshes, const std::vector<Bone>& _bones)
	: m_name(_name), m_meshes(std::move(_meshes)), m_bones(_bones)
{
	//@ ? extra init,? extra paramenetrs
}

//---------------------------------------------------------------------------
eModel::~eModel()
{
	for (auto& mesh : m_meshes)
		mesh.FreeTextures();
}

//---------------------------------------------------------------------------
std::string eModel::RootBoneName()
{
	return m_root_bone->GetName();
}

//---------------------------------------------------------------------------
void eModel::AddMesh(std::vector<Vertex> _vertices, std::vector<GLuint> _indices, std::vector<TextureInfo> _textures, const Material& _material, const std::string& _name, bool _calculate_tangent)
{
	std::vector<Texture> textures;
	for (auto t : _textures)
		textures.emplace_back(t);

	m_meshes.emplace_back(_vertices, _indices, textures, _material, _name, _calculate_tangent);
}

//---------------------------------------------------------------------------
const eMesh* eModel::GetMeshByIndex(size_t _index) const
{
	if (_index < m_meshes.size())
		return &m_meshes[_index];
	else
		return nullptr;
}

//---------------------------------------------------------------------------
void eModel::Draw()
{
	for (GLuint i = 0; i < this->m_meshes.size(); ++i)
		this->m_meshes[i].Draw();
}

//---------------------------------------------------------------------------
void eModel::DrawInstanced(int32_t _instances)
{
	for (GLuint i = 0; i < this->m_meshes.size(); ++i)
		this->m_meshes[i].DrawInstanced(_instances);
}

//---------------------------------------------------------------------------
void eModel::SetMaterial(const Material& _m)
{
	for (auto& mesh : m_meshes)
		mesh.SetMaterial(_m);
}

//---------------------------------------------------------------------------
void	eModel::SetUpMeshes()
{
	for (auto& mesh : m_meshes)
		mesh.SetupMesh();
}

//---------------------------------------------------------------------------
void eModel::ReloadTextures()
{
	for (auto& mesh : m_meshes)
		mesh.ReloadTextures();
}

//---------------------------------------------------------------------------
size_t eModel::GetVertexCount() const
{
  size_t count = 0;
  for (auto& mesh : m_meshes)
    count += mesh.m_vertices.size();
  return count;
}

//----------------------------------------------------------------------------
std::vector<const I3DMesh*> eModel::Get3DMeshes() const
{
	std::vector<const I3DMesh*> ret;
	for (const eMesh& mesh : m_meshes)
		ret.push_back(&mesh);
	return ret;
}

//----------------------------------------------------------------------------
std::vector<const IMesh*> eModel::GetMeshes() const
{
  std::vector<const IMesh*> ret;
  for (const eMesh& mesh : m_meshes)
    ret.push_back(&mesh);
  return ret;
}

//----------------------------------------------------------------------------
std::vector<const IBone*> eModel::GetBones() const
{
	std::vector<const IBone*> bones;
	for (auto& bone : m_bones)
		bones.push_back(&bone);
	return bones;
}

//----------------------------------------------------------------------------
std::vector<const IAnimation*> eModel::GetAnimations() const
{
  std::vector<const IAnimation*> ret;
  for (auto& anim : m_animations)
  {
    ret.push_back(&anim);
  }
  return ret;
}

//-------------------------------------------------------------------------------------------
void eModel::mapMehsesToNodes()
{
	for (int32_t i = 0; i < m_meshes.size(); ++i)
	{
		std::vector<Bone>::iterator meshBoneIter = std::find_if(m_bones.begin(), m_bones.end(), [this, i](const Bone& bone)
			{ return bone.GetName() == m_meshes[i].Name(); });
		if (meshBoneIter != m_bones.end())
		{
			for (int32_t j = 0; j < m_meshes[i].m_vertices.size(); ++j)
			{
				m_meshes[i].m_vertices[j].boneIDs[0] = (glm::i32)meshBoneIter->GetID();
				m_meshes[i].m_vertices[j].weights[0] = 1.0f;
			}
			m_meshes[i].ReloadVertexBuffer();
		}
	}
}

//----------------------------------------------------------------------------------------
void eModel::updateAnimation(Bone &bone, const Frame& frame, const glm::mat4 &ParentTransform)
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
void eModel::VertexBoneData::AddBoneData(int BoneID, float Weight)
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

//-----------------------------------------------------------------------
//IModel* MakeModel(const std::string& _name, std::vector<AssimpMesh>&& _meshes, const std::vector<Bone>& _bones)
//{
//	return new eModel(_name, std::move(_meshes), _bones);
//}
