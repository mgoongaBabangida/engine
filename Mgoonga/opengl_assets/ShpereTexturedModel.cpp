#include "ShpereTexturedModel.h"

#include <glew-2.1.0\include\GL\glew.h>
#include <opengl_assets\Texture.h>
#include "MyMesh.h"

//--------------------------------------------
SphereTexturedMesh::SphereTexturedMesh()
{
  glGenVertexArrays(1, &sphereVAO);

  glGenBuffers(1, &vbo);
  glGenBuffers(1, &ebo);

  const unsigned int X_SEGMENTS = 64;
  const unsigned int Y_SEGMENTS = 64;
  const float PI = 3.14159265359f;
  for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
  {
    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
    {
      float xSegment = (float)x / (float)X_SEGMENTS;
      float ySegment = (float)y / (float)Y_SEGMENTS;
      float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
      float yPos = std::cos(ySegment * PI);
      float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

      MyVertex vert;
      vert.position = (glm::vec3(xPos, yPos, zPos));
      vert.TexCoords = (glm::vec2(xSegment, ySegment));
      vert.Normal = (glm::vec3(xPos, yPos, zPos));
      vertices.push_back(vert);
    }
  }

  bool oddRow = false;
  for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
  {
    if (!oddRow) // even rows: y == 0, y == 2; and so on
    {
      for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
      {
        indices.push_back(y * (X_SEGMENTS + 1) + x);
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
      }
    }
    else
    {
      for (int x = X_SEGMENTS; x >= 0; --x)
      {
        indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        indices.push_back(y * (X_SEGMENTS + 1) + x);
      }
    }
    oddRow = !oddRow;
  }
  indexCount = static_cast<unsigned int>(indices.size());

  std::vector<float> data;
  for (unsigned int i = 0; i < vertices.size(); ++i)
  {
    data.push_back(vertices[i].position.x);
    data.push_back(vertices[i].position.y);
    data.push_back(vertices[i].position.z);
    if (vertices.size() > 0)
    {
      data.push_back(vertices[i].Normal.x);
      data.push_back(vertices[i].Normal.y);
      data.push_back(vertices[i].Normal.z);
    }
    if (vertices.size() > 0)
    {
      data.push_back(vertices[i].TexCoords.x);
      data.push_back(vertices[i].TexCoords.y);
    }
  }
  glBindVertexArray(sphereVAO);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
  unsigned int stride = (3 + 2 + 3) * sizeof(float);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
}

SphereTexturedMesh::~SphereTexturedMesh()
{
  glDeleteVertexArrays(1, &sphereVAO);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);
}

void SphereTexturedMesh::Draw()
{
  glBindVertexArray(sphereVAO);
  glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void SphereTexturedMesh::SetMaterial(const Material& _m)
{
  material = _m;
}

std::optional<Material> SphereTexturedMesh::GetMaterial() const
{
  return  material;
}

void SphereTexturedModel::Draw()
{
  if (m_textures.size() >= 4)
  {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_textures[0]->id);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_textures[1]->id);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_textures[2]->id);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_textures[3]->id);
  }
  m_mesh->Draw();
}

std::vector<glm::vec3> SphereTexturedModel::GetPositions() const
  {
   std::vector<glm::vec3> ret;
   for (auto& vert : m_mesh->vertices)
    ret.push_back(vert.position);
   return ret;
  }
