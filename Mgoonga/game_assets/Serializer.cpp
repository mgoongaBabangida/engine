#include "stdafx.h"
#include "Serializer.h"

#include <opengl_assets/Model.h>
#include <opengl_assets/Mesh.h>

#include <fstream>

//----------------------------------------------------------------------------
void Serializer::WriteMeshToFile(const eMesh& _mesh, const std::string& _filename)
{
  std::ofstream outFile(_filename, std::ios::binary);
  if (!outFile) {
    throw std::ios_base::failure("Failed to open file for writing");
  }

  // Write vertices
  size_t verticesSize = _mesh.GetVertexs().size();
  outFile.write(reinterpret_cast<const char*>(&verticesSize), sizeof(verticesSize));
  outFile.write(reinterpret_cast<const char*>(_mesh.GetVertexs().data()), verticesSize * sizeof(Vertex));

  // Write indices
  size_t indicesSize = _mesh.GetIndices().size();
  outFile.write(reinterpret_cast<const char*>(&indicesSize), sizeof(indicesSize));
  outFile.write(reinterpret_cast<const char*>(_mesh.GetIndices().data()), indicesSize * sizeof(GLuint));

  // Write textures
  size_t texturesSize = _mesh.GetTextures().size();
  outFile.write(reinterpret_cast<const char*>(&texturesSize), sizeof(texturesSize));
  for (const auto& texture : _mesh.GetTextures())
  {
    size_t typeSize = texture.m_type.size();
    outFile.write(reinterpret_cast<const char*>(&typeSize), sizeof(typeSize));
    outFile.write(texture.m_type.data(), typeSize);

    size_t pathSize = texture.m_path.size();
    outFile.write(reinterpret_cast<const char*>(&pathSize), sizeof(pathSize));
    outFile.write(texture.m_path.data(), pathSize);

    /*outFile.write(reinterpret_cast<const char*>(&texture.m_width), sizeof(texture.m_width));
    outFile.write(reinterpret_cast<const char*>(&texture.m_height), sizeof(texture.m_height));
    outFile.write(reinterpret_cast<const char*>(&texture.m_channels), sizeof(texture.m_channels));
    outFile.write(reinterpret_cast<const char*>(&texture.m_layers), sizeof(texture.m_layers));
    outFile.write(reinterpret_cast<const char*>(&texture.m_num_rows), sizeof(texture.m_num_rows));*/
  }

  // Write name
  size_t nameSize = _mesh.Name().size();
  outFile.write(reinterpret_cast<const char*>(&nameSize), sizeof(nameSize));
  outFile.write(_mesh.Name().data(), nameSize);

  // Write material
  const Material& material = _mesh.GetMaterial().value();
  outFile.write(reinterpret_cast<const char*>(&material.albedo), sizeof(material.albedo));
  outFile.write(reinterpret_cast<const char*>(&material.metallic), sizeof(material.metallic));
  outFile.write(reinterpret_cast<const char*>(&material.roughness), sizeof(material.roughness));
  outFile.write(reinterpret_cast<const char*>(&material.ao), sizeof(material.ao));

  outFile.write(reinterpret_cast<const char*>(&material.albedo_texture_id), sizeof(material.albedo_texture_id));
  outFile.write(reinterpret_cast<const char*>(&material.metalic_texture_id), sizeof(material.metalic_texture_id));
  outFile.write(reinterpret_cast<const char*>(&material.normal_texture_id), sizeof(material.normal_texture_id));
  outFile.write(reinterpret_cast<const char*>(&material.roughness_texture_id), sizeof(material.roughness_texture_id));
  outFile.write(reinterpret_cast<const char*>(&material.emissive_texture_id), sizeof(material.emissive_texture_id));

  outFile.write(reinterpret_cast<const char*>(&material.use_albedo), sizeof(material.use_albedo));
  outFile.write(reinterpret_cast<const char*>(&material.use_metalic), sizeof(material.use_metalic));
  outFile.write(reinterpret_cast<const char*>(&material.use_normal), sizeof(material.use_normal));
  outFile.write(reinterpret_cast<const char*>(&material.use_roughness), sizeof(material.use_roughness));
  outFile.write(reinterpret_cast<const char*>(&material.use_phong_shading), sizeof(material.use_phong_shading));

  outFile.close();
}

//----------------------------------------------------------------------------
void Serializer::ReadMeshFromFile(eModel& _model, const std::string& _filename)
{
  std::ifstream inFile(_filename, std::ios::binary);
  if (!inFile) {
    throw std::ios_base::failure("Failed to open file for reading");
  }

  // Read vertices
  std::vector<Vertex>	vertices;
  size_t verticesSize;
  inFile.read(reinterpret_cast<char*>(&verticesSize), sizeof(verticesSize));
  vertices.resize(verticesSize);
  inFile.read(reinterpret_cast<char*>(vertices.data()), verticesSize * sizeof(Vertex));

  // Read indices
  std::vector<GLuint>	indices;
  size_t indicesSize;
  inFile.read(reinterpret_cast<char*>(&indicesSize), sizeof(indicesSize));
  indices.resize(indicesSize);
  inFile.read(reinterpret_cast<char*>(indices.data()), indicesSize * sizeof(GLuint));

  // Read textures
  std::vector<TextureInfo> textures;
  size_t texturesSize;
  inFile.read(reinterpret_cast<char*>(&texturesSize), sizeof(texturesSize));
  for (int i =0; i < texturesSize; ++i)
  {
    std::string type;
    size_t typeSize;
    inFile.read(reinterpret_cast<char*>(&typeSize), sizeof(typeSize));
    type.resize(typeSize);
    inFile.read(&type[0], typeSize);

    std::string path;
    size_t pathSize;
    inFile.read(reinterpret_cast<char*>(&pathSize), sizeof(pathSize));
    path.resize(pathSize);
    inFile.read(&path[0], pathSize);

    textures.emplace_back(type, path);

    /*inFile.read(reinterpret_cast<char*>(&texture.m_width), sizeof(texture.m_width));
    inFile.read(reinterpret_cast<char*>(&texture.m_height), sizeof(texture.m_height));
    inFile.read(reinterpret_cast<char*>(&texture.m_channels), sizeof(texture.m_channels));
    inFile.read(reinterpret_cast<char*>(&texture.m_layers), sizeof(texture.m_layers));
    inFile.read(reinterpret_cast<char*>(&texture.m_num_rows), sizeof(texture.m_num_rows));*/
  }

  // Read name
  std::string name;
  size_t nameSize;
  inFile.read(reinterpret_cast<char*>(&nameSize), sizeof(nameSize));
  name.resize(nameSize);
  inFile.read(&name[0], nameSize);

  // Read material
  Material material;
  inFile.read(reinterpret_cast<char*>(&material.albedo), sizeof(material.albedo));
  inFile.read(reinterpret_cast<char*>(&material.metallic), sizeof(material.metallic));
  inFile.read(reinterpret_cast<char*>(&material.roughness), sizeof(material.roughness));
  inFile.read(reinterpret_cast<char*>(&material.ao), sizeof(material.ao));

  inFile.read(reinterpret_cast<char*>(&material.albedo_texture_id), sizeof(material.albedo_texture_id));
  inFile.read(reinterpret_cast<char*>(&material.metalic_texture_id), sizeof(material.metalic_texture_id));
  inFile.read(reinterpret_cast<char*>(&material.normal_texture_id), sizeof(material.normal_texture_id));
  inFile.read(reinterpret_cast<char*>(&material.roughness_texture_id), sizeof(material.roughness_texture_id));
  inFile.read(reinterpret_cast<char*>(&material.emissive_texture_id), sizeof(material.emissive_texture_id));

  inFile.read(reinterpret_cast<char*>(&material.use_albedo), sizeof(material.use_albedo));
  inFile.read(reinterpret_cast<char*>(&material.use_metalic), sizeof(material.use_metalic));
  inFile.read(reinterpret_cast<char*>(&material.use_normal), sizeof(material.use_normal));
  inFile.read(reinterpret_cast<char*>(&material.use_roughness), sizeof(material.use_roughness));
  inFile.read(reinterpret_cast<char*>(&material.use_phong_shading), sizeof(material.use_phong_shading));

  inFile.close();

  _model.AddMesh(vertices, indices, textures, material, name, true);
}
