#include "PBRRender.h"

#include <math/Camera.h>
#include <glm\glm\gtc\matrix_transform.hpp>
#include <glm\glm\gtx\transform.hpp>
#include <glm\glm\gtc\quaternion.hpp>
#include <glm\glm\gtx\quaternion.hpp>

#include <map>

//-------------------------------------------------------------------
ePBRRender::ePBRRender(const std::string& vS, const std::string& fS)
: matrices(MAX_BONES)
{
  pbrShader.installShaders(vS.c_str(), fS.c_str());
  glUseProgram(pbrShader.ID());

  albedoLoc         = glGetUniformLocation(pbrShader.ID(), "albedo");
  metallicLoc       = glGetUniformLocation(pbrShader.ID(), "metallic");
  roughnessLoc      = glGetUniformLocation(pbrShader.ID(), "roughness");
  aoLoc             = glGetUniformLocation(pbrShader.ID(), "ao");
  camPosLoc         = glGetUniformLocation(pbrShader.ID(), "camPos");

  glUniform1f(aoLoc, 1.0f); //@todo take from material if textured too

  //vertex shader
  BonesMatLocation = glGetUniformLocation(pbrShader.ID(), "gBones");
  fullTransformationUniformLocation = glGetUniformLocation(pbrShader.ID(), "modelToProjectionMatrix");
  modelToWorldMatrixUniformLocation = glGetUniformLocation(pbrShader.ID(), "modelToWorldMatrix");
  shadowMatrixUniformLocation       = glGetUniformLocation(pbrShader.ID(), "shadowMatrix"); //shadow
}

//-----------------------------------------------------------------------------------------------------------
void ePBRRender::Render(const Camera& camera, const Light& _light, std::vector<shObject>& objects)
{
  glUseProgram(pbrShader.ID());
  pbrShader.SetUniformData("camPos", glm::vec4(camera.getPosition(), 1.0f));

  glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(_light.light_position), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
  glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
  
  glm::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix(); 
  {
    std::vector<glm::vec4> lpositions;
    std::vector<glm::vec4> lcolors;

    lpositions.push_back(_light.light_position);
    lcolors.push_back({ _light.intensity });

    GLuint loc_pos = glGetUniformLocation(pbrShader.ID(), "lightPositions");
    glUniform3fv(loc_pos, 1, &lpositions[0][0]);

    GLuint loc_col = glGetUniformLocation(pbrShader.ID(), "lightColors");
    glUniform3fv(loc_col, 1, &lcolors[0][0]);
  }

  std::map<std::string, std::vector<shObject>> instanced;
  for (auto& object : objects)
  {
    if (object->GetInstancingTag().empty())
    {
      if(!object->Is2DScreenSpace())
        modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
      else
        modelToProjectionMatrix = object->GetTransform()->getModelMatrix();
      glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
      glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);

      _SetMaterial(object);
      if (object->GetRigger() != nullptr)
      {
        matrices = object->GetRigger()->GetMatrices();
      }
      else
      {
        for (auto& m : matrices)
          m = UNIT_MATRIX;
      }
      glUniformMatrix4fv(BonesMatLocation, MAX_BONES, GL_FALSE, &matrices[0][0][0]);
      object->GetModel()->Draw();
    }
    else
    {
      if (auto it = instanced.find(object->GetInstancingTag()); it != instanced.end())
        it->second.push_back(object);
      else
        instanced.insert({ object->GetInstancingTag(), {object} });
    }
  }
  
  // draw instanced
  for (auto& node : instanced)
  {
    for (int i = 0; i < node.second.size(); ++i)
    {
      glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * node.second[i]->GetTransform()->getModelMatrix();
      GLuint fullTransformation = glGetUniformLocation(pbrShader.ID(), std::string("modelToProjectionMatrix[" + std::to_string(i) + "]").c_str());
      glUniformMatrix4fv(fullTransformation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
      GLuint modelToWorldMatrix = glGetUniformLocation(pbrShader.ID(), std::string("modelToWorldMatrix[" + std::to_string(i) + "]").c_str());
      glUniformMatrix4fv(modelToWorldMatrix, 1, GL_FALSE, &node.second[i]->GetTransform()->getModelMatrix()[0][0]);
    }
    _SetMaterial(node.second[0]);

    //Instancing does not support bone matrices !!!
      for (auto& m : matrices)
        m = UNIT_MATRIX;
    glUniformMatrix4fv(BonesMatLocation, MAX_BONES, GL_FALSE, &matrices[0][0][0]);
    node.second[0]->GetModel()->DrawInstanced(node.second.size());
  }
}

//---------------------------------------------------------------------------------
void ePBRRender::_SetMaterial(shObject _obj)
{
  if (_obj->GetModel()->HasMaterial())
  {
    Material material = _obj->GetModel()->GetMaterial().value();
    if (material.use_albedo)
    {
      glUniform1i(glGetUniformLocation(pbrShader.ID(), "textured"), 1);
      glUniform1f(aoLoc, material.ao);
    }
    else
    {
      glUniform1i(glGetUniformLocation(pbrShader.ID(), "textured"), 0);

      glUniform4f(albedoLoc, material.albedo[0], material.albedo[1], material.albedo[2], 1.0f);
      glUniform1f(aoLoc, material.ao);
      glUniform1f(metallicLoc, material.metallic);
      glUniform1f(roughnessLoc, material.roughness);
    }
    // draw object and return
  }
  else
  {
    for (const IMesh* mesh : _obj->GetModel()->GetMeshes())
    {
      if (mesh->HasMaterial())
      {
        Material material = mesh->GetMaterial().value();
        if (material.use_albedo)
        {
          glUniform1i(glGetUniformLocation(pbrShader.ID(), "textured"), 1);
          glUniform1f(aoLoc, material.ao);
        }
        else
        {
          glUniform1i(glGetUniformLocation(pbrShader.ID(), "textured"), 0);

          glUniform4f(albedoLoc, material.albedo[0], material.albedo[1], material.albedo[2], 1.0f);
          glUniform1f(aoLoc, material.ao);
          glUniform1f(metallicLoc, material.metallic);
          glUniform1f(roughnessLoc, material.roughness);
        }
      }
      // draw mesh ? //set matterial and draw for each mesh
    }
  }
}
