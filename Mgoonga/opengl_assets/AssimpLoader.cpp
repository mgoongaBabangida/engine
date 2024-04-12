#include "AssimpLoader.h"

#include <math/Transform.h>

#include <algorithm>
#include <assert.h>

using namespace std;

//#define DEBUG_VERTICES

glm::mat4 toMat4(const aiMatrix4x4& from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

//---------------------------------------------------------------------------
Transform createTransform(const aiVectorKey& PositionKey, const aiQuatKey& RotationKey, const aiVectorKey& ScalingKey)
{
	Transform trans;

	glm::vec3 s(1.0f, 1.0f, 1.0f);
	//glm::vec3 s(ScalingKey.mValue.x, ScalingKey.mValue.y, ScalingKey.mValue.z);
	glm::quat q(RotationKey.mValue.w, RotationKey.mValue.x, RotationKey.mValue.y, RotationKey.mValue.z);
	glm::vec3 t(PositionKey.mValue.x, PositionKey.mValue.y, PositionKey.mValue.z);

	trans.setTranslation(t);
	trans.setRotation(q);
	trans.setScale(s);

	return trans;
}

//---------------------------------------------------------------------------
AssimpLoader::AssimpLoader()
{
	//Assimp::Importer import;
	m_import.reset(new Assimp::Importer());
	m_import->SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
}

//---------------------------------------------------------------------------
AssimpLoader::~AssimpLoader()
{
	//need to read Assimp doc, looks like we do not own it
	m_scene.release();
}

//--------------------------------------------------------------------------------------
Model* AssimpLoader::LoadModel(char* path, const std::string& _name, bool _m_invert_y_uv)
{
	if (!m_import)
		return nullptr;

	const aiScene* scene = m_import->ReadFile(path,
																						aiProcess_Triangulate |
																						aiProcess_GenSmoothNormals |
																						aiProcess_FlipUVs |
																						aiProcess_CalcTangentSpace |
																						aiProcess_LimitBoneWeights);//get sm normals

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << m_import->GetErrorString() << endl;
		return nullptr;
	}
	m_scene.release();
	m_scene.reset((aiScene*)scene);

	m_invert_y_uv = _m_invert_y_uv;

	m_model = new Model(path, _name);

	_ProcessNode(m_scene->mRootNode, m_scene.get());

	m_model->m_GlobalInverseTransform = glm::inverse(toMat4(m_scene->mRootNode->mTransformation));

	//Bones
	_LoadNodesToBone(m_scene->mRootNode);
	_LoadBoneChildren(m_scene->mRootNode);
	if (m_model->m_no_real_bones)
		m_model->mapMehsesToNodes();

	if (m_scene->mRootNode->mNumMeshes)
		m_model->m_root_bone = &m_model->m_bones[m_model->m_BoneMapping.find(m_scene->mRootNode->mName.C_Str())->second];
	if (m_model->m_root_bone != nullptr)
		m_model->m_root_bone->calculateInverseBindTransform(UNIT_MATRIX);
	for (uint32_t i = 0; i < m_scene->mNumAnimations; ++i)
		_ProccessAnimations(m_scene->mAnimations[i]);

	if (!m_model->m_root_bone)
	{
		auto scene = m_scene.get();
		std::vector<Bone>::iterator root = std::find_if(m_model->m_bones.begin(), m_model->m_bones.end(), [scene](const Bone& bone)
			{ return scene->mRootNode->mName.C_Str() == bone.GetName(); });
		m_model->m_root_bone = &(*root);
	}
	return m_model; //@todo set to nullptr
}

//--------------------------------------------------------------------------------------
void AssimpLoader::_ProcessNode(aiNode* _node, const aiScene* _scene)
{
	// Process all the node's meshes (if any)
		for (GLuint i = 0; i < _node->mNumMeshes; i++)
		{
			aiMesh* mesh = _scene->mMeshes[_node->mMeshes[i]];
			m_model->m_meshes.emplace_back(_ProcessMesh(mesh, _scene));
		}
	// Then do the same for each of its children
	for (GLuint i = 0; i < _node->mNumChildren; i++)
	{
		_ProcessNode(_node->mChildren[i], _scene);
	}
}

//------------------------------------------------------------------------------------------------
AssimpMesh AssimpLoader::_ProcessMesh(aiMesh* _mesh, const aiScene* _scene)
{
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<Model::VertexBoneData> boneData;

	for (GLuint i = 0; i < _mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// Process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = _mesh->mVertices[i].x;
		vector.y = _mesh->mVertices[i].y;
		vector.z = _mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = _mesh->mNormals[i].x;
		vector.y = _mesh->mNormals[i].y;
		vector.z = _mesh->mNormals[i].z;
		vertex.Normal = vector;

		if (_mesh->mTangents != NULL)
		{
			vector.x = _mesh->mTangents[i].x;
			vector.y = _mesh->mTangents[i].y;
			vector.z = _mesh->mTangents[i].z;
			vertex.tangent = vector;

			vector.x = _mesh->mBitangents[i].x;
			vector.y = _mesh->mBitangents[i].y;
			vector.z = _mesh->mBitangents[i].z;
			vertex.bitangent = vector;
		}

		if (_mesh->HasTextureCoords(0)) // Does the _mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = _mesh->mTextureCoords[0][i].x;
			vec.y = m_invert_y_uv ? 1.0f - _mesh->mTextureCoords[0][i].y : _mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		if (_mesh->HasTextureCoords(1))
		{
			assert("_mesh has multiple texture coordinates!");
		}

		vertices.push_back(vertex);
	}

	//-----------------------------Bones Set Up-----------------------------
	if (_mesh->HasBones())
	{
		boneData.resize(vertices.size());
		for (uint32_t i = 0; i < _mesh->mNumBones; ++i)
		{
			int BoneIndex = 0;
			string BoneName(_mesh->mBones[i]->mName.C_Str());
			if (m_model->m_BoneMapping.find(BoneName) == m_model->m_BoneMapping.end())
			{
				BoneIndex = m_model->m_NumBones;
				m_model->m_NumBones++;
				m_model->m_bones.push_back(Bone(BoneIndex, BoneName, toMat4(_mesh->mBones[i]->mOffsetMatrix)));
				m_model->m_BoneMapping[BoneName] = BoneIndex;
			}
			else
			{
				BoneIndex = m_model->m_BoneMapping[BoneName];
			}

			for (uint32_t j = 0; j < _mesh->mBones[i]->mNumWeights; j++)
			{
				int VertexID = _mesh->mBones[i]->mWeights[j].mVertexId;
				float Weight = _mesh->mBones[i]->mWeights[j].mWeight;
				boneData[VertexID].AddBoneData(BoneIndex, Weight);
			}
		}
	}
	else
		m_model->m_no_real_bones = true;

	if (_mesh->HasBones())
	{
		for (int i = 0; i < boneData.size(); ++i)
		{
			vertices[i].boneIDs = glm::vec4(boneData[i].IDs[0], boneData[i].IDs[1], boneData[i].IDs[2], boneData[i].IDs[3]);
			vertices[i].weights = glm::vec4(boneData[i].Weights[0], boneData[i].Weights[1], boneData[i].Weights[2], boneData[i].Weights[3]);
		}
	}
	else
	{
		for (int i = 0; i < vertices.size(); ++i)
		{
			vertices[i].boneIDs = glm::vec4(0, 0, 0, 0);
			vertices[i].weights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}
	}

#ifdef DEBUG_VERTICES
	if (_mesh->mNumBones > 0)
	{
		for (auto& i : boneData)
		{
			if (i.numTries > 1)
			{
				i.SetOneTotalWeight();
				i.DebugWeights();
			}
		}
	}
#endif // DEBUG_VERTICES

	//-----------------------------Process indices------------------------------------------------
	for (GLuint i = 0; i < _mesh->mNumFaces; i++)
	{
		aiFace face = _mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	//-----------------------------Process material-------------------------------------------------
	Material mat = _ProcessMaterial(_mesh, textures);

	return AssimpMesh{ vertices, indices, textures, mat, _mesh->mName.C_Str(), _mesh->mTangents == NULL };
}

//------------------------------------------------------------------------------------------------
void AssimpLoader::_LoadNodesToBone(aiNode* _node)//all nodes have names
{
	for (uint32_t i = 0; i < _node->mNumChildren; ++i)
		_LoadNodesToBone(_node->mChildren[i]);

	std::vector<Bone>::iterator CurBoneIter = std::find_if(m_model->m_bones.begin(), m_model->m_bones.end(), [_node](const Bone& bone)
		{ return bone.GetName() == _node->mName.C_Str(); });
	if (CurBoneIter == m_model->m_bones.end()) //not a bone
	{
		m_model->m_bones.push_back(Bone(m_model->m_NumBones, _node->mName.C_Str(), UNIT_MATRIX, false));
		CurBoneIter = --m_model->m_bones.end();
		CurBoneIter->setMTransform(toMat4(_node->mTransformation));
		m_model->m_BoneMapping[_node->mName.C_Str()] = m_model->m_NumBones;
		m_model->m_NumBones++;
	}
}

//------------------------------------------------------------------------------------------------
void AssimpLoader::_LoadBoneChildren(aiNode* _node)
{
	for (uint32_t i = 0; i < _node->mNumChildren; ++i)
		_LoadBoneChildren(_node->mChildren[i]);

	std::vector<Bone>::iterator CurBoneIter = std::find_if(m_model->m_bones.begin(), m_model->m_bones.end(), [_node](const Bone& bone)
		{ return bone.GetName() == _node->mName.C_Str(); }); // end() ? all bones should be already there
	for (uint32_t i = 0; i < _node->mNumChildren; ++i)
	{
		int id = m_model->m_BoneMapping.find(_node->mChildren[i]->mName.C_Str())->second;
		CurBoneIter->addChild(&m_model->m_bones[id]);
	}
}

//------------------------------------------------------------------------------------------------
void AssimpLoader::_ProccessAnimations(const aiAnimation* _anim)
{
	int durationMsc = (int)(_anim->mDuration / _anim->mTicksPerSecond * 1'000);//mTicksPerSecond can be 0 ?
	int qNodes = _anim->mNumChannels; //quantity of nodes(bones) in anim 45
	int qframes = _anim->mChannels[0]->mNumPositionKeys; //check if it is the same for all 41

	std::vector<Frame> frames(qframes);
	for (int i = 0; i < qframes; ++i)
		frames[i].addTimeStamp((durationMsc / qframes) * i);

	for (int i = 0; i < qNodes; ++i)
	{
		for (int j = 0; j < qframes; ++j) // numTransforms ? frames?
		{
			frames[j].addTrnasform(_anim->mChannels[i]->mNodeName.C_Str(),
				createTransform(_anim->mChannels[i]->mPositionKeys[j],
												_anim->mChannels[i]->mRotationKeys[j],
												_anim->mChannels[i]->mScalingKeys[j]));
		}
	}
	m_model->m_animations.push_back(SceletalAnimation(durationMsc, frames, _anim->mName.C_Str())); // duration ?
}

//------------------------------------------------------------------------------------------------
std::vector<Texture> AssimpLoader::_LoadMaterialTextures(aiMaterial* _mat, aiTextureType _type, std::string _typeName)
{
	vector<Texture> textures;
	for (GLuint i = 0; i < _mat->GetTextureCount(_type); ++i)
	{
		aiString str;
		_mat->GetTexture(_type, i, &str);
		string filename = string(str.C_Str());
		textures.push_back(m_model->LoatTexture(string(str.C_Str()), _typeName));
	}
	return textures;
}

//------------------------------------------------------------------------------------------------
Material AssimpLoader::_ProcessMaterial(aiMesh* _mesh, std::vector<Texture>& textures)
{
	Material mat;
	if (_mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = m_scene->mMaterials[_mesh->mMaterialIndex];
		vector<Texture> diffuseMaps = _LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		if (!diffuseMaps.empty())
		{
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			mat.albedo_texture_id = diffuseMaps[0].id;
			mat.use_albedo = true;
		}
		else
			mat.albedo_texture_id = Texture::GetDefaultTextureId();

		vector<Texture> specularMaps = _LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		if (!specularMaps.empty())
		{
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			mat.metalic_texture_id = specularMaps[0].id; // should be dif?
			mat.use_metalic = true;
		}
		else
			mat.metalic_texture_id = Texture::GetDefaultTextureId();

		vector<Texture> normalMaps = _LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		if (!normalMaps.empty())
		{
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			mat.normal_texture_id = normalMaps[0].id;
			mat.use_normal = true;
		}
		else
			mat.normal_texture_id = Texture::GetDefaultTextureId();

		//@!? height
		/*vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
		if (!normalMaps.empty())
		{
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
			mat.normal_texture_id = normalMaps[0].id;
		}
		else
			mat.normal_texture_id = GetDefaultTextureId();*/

		vector<Texture> emissionMaps = _LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emission");
		if (!emissionMaps.empty())
		{
			textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());
			mat.emissive_texture_id = emissionMaps[0].id;
		}
		else
			mat.emissive_texture_id = Texture::GetDefaultTextureId();

		//glossiness is oposite to roughness @todo needs to be inverted!!!
		vector<Texture> roughnessMaps = _LoadMaterialTextures(material, aiTextureType_SHININESS, "texture_roughness");
		if (!roughnessMaps.empty())
		{
			textures.insert(textures.end(), roughnessMaps.begin(), roughnessMaps.end());
			mat.roughness_texture_id = roughnessMaps[0].id;
			mat.use_roughness;
		}
		else
			mat.roughness_texture_id = Texture::GetDefaultTextureId();

		vector<Texture> displacementMaps = _LoadMaterialTextures(material, aiTextureType_DISPLACEMENT, "texture_displacement");
		if (!displacementMaps.empty())
		{
			textures.insert(textures.end(), displacementMaps.begin(), displacementMaps.end());
			//mat. = displacementMaps[0].id;
		}
	}

	return mat;
}
