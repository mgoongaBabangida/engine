#include "stdafx.h"
#include "AssimpModel.h"
#include "Transform.h"
#include <algorithm>

using namespace std;

//#define DEBUG_VERTICES

glm::mat4 toMat4(const aiMatrix4x4 & from) {
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

Transform createTransform(const aiVectorKey& PositionKey,const aiQuatKey& RotationKey,const aiVectorKey& ScalingKey)
{ 	
	Transform trans;

	glm::vec3 s(ScalingKey.mValue.x, ScalingKey.mValue.y, ScalingKey.mValue.z);
	glm::quat q(RotationKey.mValue.w, RotationKey.mValue.x, RotationKey.mValue.y, RotationKey.mValue.z);
	glm::vec3 t(PositionKey.mValue.x, PositionKey.mValue.y, PositionKey.mValue.z);
	
	trans.setTranslation(t);
	trans.setRotation(q);
	trans.setScale(s);

	return trans;
}

std::string Model::RootBoneName()
{
	if (!root_bone)
	{
		auto scene = m_scene;
		std::vector<Bone>::iterator root = std::find_if(m_bones.begin(), m_bones.end(), [scene](const Bone& bone) { return scene->mRootNode->mName.C_Str() == bone.Name(); });
		root_bone = &(*root);
	}
	return root_bone->Name();
}

void Model::Draw()
{
	for (GLuint i = 0; i < this->meshes.size(); i++)
		this->meshes[i].Draw();
}

void Model::loadModel(string path)
{
	//Assimp::Importer import;
	m_import = new Assimp::Importer(); // delete
	m_import->SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
	const aiScene* scene = m_import->ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights); //get sm normals
	m_scene = (aiScene*)scene;
	if (!m_scene || m_scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << m_import->GetErrorString() << endl;
		return;
	}
	this->directory = path.substr(0, path.find_last_of('/'));

	this->processNode(m_scene->mRootNode, m_scene);
	
	this->m_GlobalInverseTransform = glm::inverse(toMat4(m_scene->mRootNode->mTransformation));
	/////Bones///////////////////////////////////////
	this->loadNodesToBone(m_scene->mRootNode);
	this->loadBoneChildren(m_scene->mRootNode);
	///////////////Dump///////////////////////////////////
	/*auto BoneNames = DumpAiMeshes();
	auto NodeNames = DumpAiNodes();

	BoneNames.resize(NodeNames.size());
	std::cout << "aiBones " << BoneNames.size() << "---"<< "aiNodes " << NodeNames.size() <<"---" << "m_Bones " << m_bones.size() << std::endl;
	for (int i = 0; i < BoneNames.size(); ++i)
	{
		std::cout << "aiBones " << BoneNames[i] << "---" << "aiNodes " << NodeNames[i] << "---" << "m_Bones " << m_bones[i].Name()<<" "<< m_bones[i].ID() << std::endl;
	}*/
	//DumpBone();
	DumpAssimpMeshes();
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	if (m_scene->mRootNode->mNumMeshes)
		root_bone = &m_bones[this->m_BoneMapping.find(m_scene->mRootNode->mName.C_Str())->second];
	if (root_bone != nullptr)
		root_bone->calculateInverseBindTransform(UNIT_MATRIX);
	for (uint32_t i = 0; i < m_scene->mNumAnimations; ++i)
		ProccessAnimations(m_scene->mAnimations[i]);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
	// Process all the node's meshes (if any)
	for (GLuint i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	//std::cout << "ENTERED-------------" << std::endl;
	// Then do the same for each of its children
	for (GLuint i = 0; i < node->mNumChildren; i++)
	{
		this->processNode(node->mChildren[i], scene);
	}

}

AssimpMesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	vector< AssimpVertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<VertexBoneData> boneData;

	for (GLuint i = 0; i < mesh->mNumVertices; i++)
	{
		AssimpVertex vertex;
		// Process vertex positions, normals and texture coordinates
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.tangent = vector;

		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.bitangent = vector;

		if (mesh->mTextureCoords[0]) // Does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}
	////////Bones Set Up ///////////////////////////
	boneData.resize(vertices.size());
	for (uint32_t i = 0; i < mesh->mNumBones; i++) 
	{
		int BoneIndex = 0;
		string BoneName(mesh->mBones[i]->mName.C_Str());
		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) {
			BoneIndex = m_NumBones;
			m_NumBones++;
			m_bones.push_back(Bone(BoneIndex, BoneName, toMat4(mesh->mBones[i]->mOffsetMatrix)));
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else {
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (int j = 0; j < mesh->mBones[i]->mNumWeights; j++) {
			int VertexID = mesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = mesh->mBones[i]->mWeights[j].mWeight;
			boneData[VertexID].AddBoneData(BoneIndex, Weight);
		}

	}

	if (mesh->mNumBones > 0)
	  for (int i = 0; i < boneData.size(); ++i) {
		vertices[i].boneIDs = glm::vec4(boneData[i].IDs[0], boneData[i].IDs[1], boneData[i].IDs[2], boneData[i].IDs[3]);
		vertices[i].weights = glm::vec4(boneData[i].Weights[0], boneData[i].Weights[1], boneData[i].Weights[2], boneData[i].Weights[3]);
	  }
	else
		for (int i = 0; i < vertices.size(); ++i) {
			vertices[i].boneIDs = glm::vec4(0, 0, 0, 0);
			vertices[i].weights = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		}

#ifdef DEBUG_VERTICES
	if (mesh->mNumBones > 0)
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
	///////////////////////////////////////////////////

	// Process indices
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (GLuint j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
		// Process material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			vector<Texture> diffuseMaps = this->loadMaterialTextures(material,
				aiTextureType_DIFFUSE, "texture_diffuse");
			textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
			vector<Texture> specularMaps = this->loadMaterialTextures(material,
				aiTextureType_SPECULAR, "texture_specular");
			textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
			vector<Texture> normalMaps = loadMaterialTextures(material, 
				aiTextureType_HEIGHT, "texture_normal");
			textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		}

	return AssimpMesh(vertices, indices, textures);
}

vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName)
{
	vector<Texture> textures;
	for (GLuint i = 0; i < mat->GetTextureCount(type); i++)
	{
		//std::cout << "--------------" << std::endl;
		aiString str;
		mat->GetTexture(type, i, &str);
		Texture texture;
		string filename = string(str.C_Str());
		filename = directory + '/' + filename;
		//std::cout << filename << std::endl;
		texture.loadTextureFromFile(filename.c_str());		
		texture.type = typeName;
		texture.path = str.C_Str();
		//std::cout << typeName << std::endl;
		//std::cout << str.C_Str() << std::endl;
		textures.push_back(texture);
	}
	return textures;
}

std::vector<glm::vec3> Model::getPositions()const {
	std::vector<glm::vec3> ret;
	for (auto& mesh: meshes) {
		for (auto& vert : mesh.vertices)
			ret.push_back(vert.Position);
	}
	return ret;
}

std::vector<GLuint> Model::getIndeces() const
{
	std::vector<GLuint> inds;
	for (auto& mesh : meshes) 
	{
		for(int i = 0; i < mesh.indices.size(); ++i)  // algortithm! concat 
		 inds.push_back(mesh.indices[i]);
	}
	return inds;
}

void Model::loadNodesToBone(aiNode * node) //all nodes have names
{
	for (int i = 0; i< node->mNumChildren; ++i)
		loadNodesToBone(node->mChildren[i]);

	std::vector<Bone>::iterator CurBoneIter = std::find_if(m_bones.begin(), m_bones.end(), [node](const Bone& bone) 
	{ return bone.Name() == node->mName.C_Str(); });
	if (CurBoneIter == m_bones.end()) //not a bone
	{
		m_bones.push_back(Bone(m_NumBones, node->mName.C_Str(), UNIT_MATRIX,false));
		CurBoneIter = --m_bones.end();
		CurBoneIter->setMTransform(toMat4(node->mTransformation));
		m_BoneMapping[node->mName.C_Str()] = m_NumBones;
		m_NumBones++;
		//std::cout << "inside add node" << std::endl;
	}
}

void Model::loadBoneChildren(aiNode * node)
{
	for (uint32_t i = 0; i< node->mNumChildren; ++i)
		loadBoneChildren(node->mChildren[i]);

	std::vector<Bone>::iterator CurBoneIter = std::find_if(m_bones.begin(), m_bones.end(), [node](const Bone& bone)
	{ return bone.Name() == node->mName.C_Str(); }); // end() ? all bones should be already there
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		int id = m_BoneMapping.find(node->mChildren[i]->mName.C_Str())->second;
		CurBoneIter->addChild(&m_bones[id]);
	}
}

SceletalAnimation Model::ProccessAnimations(const aiAnimation * anim)
{
	int durationMsc = (int)(anim->mDuration / anim->mTicksPerSecond * 1000);//mTicksPerSecond can be 0 ?
	int qNodes = anim->mNumChannels; //quantity of nodes(bones) in anim 45
	int qframes = anim->mChannels[0]->mNumPositionKeys; //check if it is the same for all 41
	//const aiNodeAnim* pNodeAnim = anim->mChannels[0];
	std::vector<Frame> frames(qframes);
	//std::cout << "Frames" << qframes << std::endl;
	//DumpAiAnimation(anim);

	for (int i = 0; i<qframes; ++i)
		frames[i].addTimeStemp((durationMsc / qframes) * i);
	
	for (int i = 0; i<qNodes; ++i)
	{
		/*auto iterBone = std::find_if(m_bones.begin(), m_bones.end(), [anim, i](const Bone& bone)
		{ return bone.Name() == anim->mChannels[i]->mNodeName.C_Str(); });*/
		for (int j = 0; j <qframes; ++j) // numTransforms ? frames?
		{
			frames[j].addTrnasform(anim->mChannels[i]->mNodeName.C_Str(),
									createTransform(anim->mChannels[i]->mPositionKeys[j], 
													anim->mChannels[i]->mRotationKeys[j], 
													anim->mChannels[i]->mScalingKeys[j]));
		}
	}
	m_animations.push_back(SceletalAnimation(durationMsc, frames, anim->mName.C_Str())); // duration ?
	return m_animations.back();
}

void Model::updateAnimation(Bone &bone, const Frame& frame, const glm::mat4 &ParentTransform)
{
	glm::mat4 currentLocalTransform;
	if(frame.exists(bone.Name()))
		currentLocalTransform = frame.pose.find(bone.Name())->second.getModelMatrix() ;
	else
		currentLocalTransform =	bone.getMTransform();
	glm::mat4 globalTransform =   ParentTransform * currentLocalTransform;

	//glm::mat4 totalTransform = currentTransform * bone.getInverseBindTransform(); //ThinMatrix
	glm::mat4 totalTransform = m_GlobalInverseTransform * globalTransform * bone.getBindTransform(); // OGLDev
	bone.setAnimatedTransform(totalTransform);

	for (int i = 0; i<bone.NumChildren(); ++i)
		updateAnimation(*(bone.getChildren()[i]), frame, globalTransform);
}

void Model::VertexBoneData::AddBoneData(int BoneID, float Weight)
 {
		++numTries;
	     for (int i = 0; i < NUM_BONES_PER_VEREX; i++) { //ARRAY_SIZE_IN_ELEMENTS(IDs)
		        if (Weights[i] == 0.0) {
			             IDs[i] = BoneID;
			            Weights[i] = Weight;
			            return;			
		  }
		
	}
	
		    // should never get here - more bones than we have space for 
		  //  assert(0);
}

//-----------------------------------------------------------------------
// DUMP
//-----------------------------------------------------------------------
std::vector<std::string> Model::DumpAiMeshes()
{
	std::vector<std::string> BoneNames;
	for (int i = 0; i < m_scene->mNumMeshes; ++i)
	{
		auto names = DumpAiMesh(m_scene->mMeshes[i]);
		for (auto item : names)
			BoneNames.push_back(item);
	}
	std::sort(BoneNames.begin(), BoneNames.end());
	auto iter = std::unique(BoneNames.begin(), BoneNames.end());
	BoneNames.erase(iter, BoneNames.end());
	return BoneNames;
}

std::vector<std::string> Model::DumpAiMesh(aiMesh* mesh)
{
	std::vector<std::string> BoneNames;
	std::cout << "Mesh " << mesh->mName.C_Str() << std::endl;
	std::cout << "Vertecies " << mesh->mNumVertices << std::endl;
	std::cout << "Bouns count " << mesh->mNumBones << std::endl;
	for (int j = 0; j < mesh->mNumBones; ++j)
	{
		std::cout << "Bouns name " << mesh->mBones[j]->mName.C_Str() << std::endl;
		BoneNames.push_back(mesh->mBones[j]->mName.C_Str());
		std::cout << "Bouns num weights " << mesh->mBones[j]->mNumWeights << std::endl;
		unsigned int Max = 0;
		std::vector<unsigned int> weightsPerVert;
		weightsPerVert.resize(mesh->mNumVertices);
		for (int k = 0; k < mesh->mBones[j]->mNumWeights; ++k)
		{
			if (mesh->mBones[j]->mWeights[k].mVertexId > Max)
				Max = mesh->mBones[j]->mWeights[k].mVertexId;
			weightsPerVert[mesh->mBones[j]->mWeights[k].mVertexId]++;
		}
		std::cout << "Max Vertex " << Max << std::endl;
		std::sort(weightsPerVert.begin(), weightsPerVert.end());
		std::cout << "Max Weights " <<  weightsPerVert[mesh->mNumVertices-1] << std::endl;
	}
	return BoneNames;
}

std::vector<std::string> Model::DumpAiNodes()
{
	std::vector<std::string> BoneNames;
	DumpAiNode(m_scene->mRootNode, BoneNames);
	std::sort(BoneNames.begin(), BoneNames.end());
	std::unique(BoneNames.begin(), BoneNames.end());
	return BoneNames;
}

void Model::DumpAiNode(aiNode* node , std::vector<std::string>& BoneNames)
{
	std::cout << "-----Node------" << std::endl;
	std::cout << "Name "<< node->mName.C_Str() << std::endl;
	BoneNames.push_back(node->mName.C_Str());
	std::cout << "NumMeshes " << node->mNumMeshes << std::endl;
	for (int i = 0; i < node->mNumMeshes; ++i)
	{
		std::cout << node->mMeshes[i] << " ";
	}
	std::cout << std::endl;
	std::cout << "Children "  << node->mNumChildren << std::endl;
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		std::cout << node->mChildren[i]->mName.C_Str() << " ";
	}
	std::cout << std::endl;
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		DumpAiNode(node->mChildren[i], BoneNames);
	}
}

void Model::DumpBone()
{
	std::cout << "-----Bone------" << std::endl;
	int i = 0;
	for (auto& bone : m_bones)
	{
		std::cout << "Bone name " << bone.Name() << bone.ID() <<"index "<< i << std::endl;
		++i;
		std::cout << "Children " << bone.NumChildren() <<" "<< bone.getChildren().size() << std::endl;
		for (auto& ch : bone.getChildren() )
		{
			std::cout << ch->Name() << ch->ID()  << std::endl;
		}
	}
}

void Model::DumpAiAnimation(const aiAnimation * anim)
{
	int durationMsc = (int)(anim->mDuration / anim->mTicksPerSecond * 1000);//mTicksPerSecond can be 0 ?
	int qNodes = anim->mNumChannels; //quantity of nodes(bones) in anim
	int qframes = anim->mChannels[0]->mNumPositionKeys; //check if it is the same for all 
	std::cout << "Animation " << anim->mName.C_Str() << " length msc " << durationMsc << " Bones " << qNodes << " Frames" << qframes << std::endl;

	for (uint32_t i = 0; i < anim->mNumChannels; ++i)
	{
		std::cout << anim->mChannels[i]->mNodeName.C_Str() << anim->mChannels[i]->mNumPositionKeys << " "<<
			anim->mChannels[i]->mNumRotationKeys <<" "<< anim->mChannels[i]->mNumScalingKeys << std::endl;
	}

	for (uint32_t i = 0; i < anim->mChannels[0]->mNumPositionKeys ; ++i)
	{
		std::cout << anim->mChannels[0]->mPositionKeys[i].mTime   << std::endl;
	}
}

void Model::DumpAssimpMeshes()
{
	//Name
	for(auto &mesh : meshes)
	{
		int maxID = 0; int minID = 0; int ones = 0; int zeros = 0; int other = 0;
		for(auto& vertex : mesh.vertices)
		{
			for (int i = 0; i < 4; i++)
			{
				if (vertex.boneIDs[i] > maxID)
					maxID = vertex.boneIDs[i];
				if (vertex.boneIDs[i] < minID)
					minID = vertex.boneIDs[i];
				float sum = vertex.weights[0] + vertex.weights[1] + vertex.weights[2] + vertex.weights[3];
				if (sum == 1.0f) ones++;
				else if (sum == 0.0f) zeros++;
				else  ones++;
			}			
		}
		std::cout << "Mesh " << "max= "<< maxID << "min= " << minID<<"one "<<ones<<"zero "<<zeros<<"other "<<other<< std::endl;
	}
}
