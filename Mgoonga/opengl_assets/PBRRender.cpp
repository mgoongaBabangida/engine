#include "PBRRender.h"

#include <math/Camera.h>

//-------------------------------------------------------------------
ePBRRender::ePBRRender(const std::string& vS, const std::string& fS)
: matrices(100)
{
  pbrShader.installShaders(vS.c_str(), fS.c_str());
  glUseProgram(pbrShader.ID);

  albedoLoc         = glGetUniformLocation(pbrShader.ID, "albedo");
  metallicLoc       = glGetUniformLocation(pbrShader.ID, "metallic");
  roughnessLoc      = glGetUniformLocation(pbrShader.ID, "roughness");
  aoLoc             = glGetUniformLocation(pbrShader.ID, "ao");
  lightPositionsLoc = glGetUniformLocation(pbrShader.ID, "lightPositions");
  lightColorsLoc    = glGetUniformLocation(pbrShader.ID, "lightColors");
  camPosLoc         = glGetUniformLocation(pbrShader.ID, "camPos");

  //vertex shader
  fullTransformationUniformLocation = glGetUniformLocation(pbrShader.ID, "modelToProjectionMatrix");
  modelToWorldMatrixUniformLocation = glGetUniformLocation(pbrShader.ID, "modelToWorldMatrix");
  shadowMatrixUniformLocation = glGetUniformLocation(pbrShader.ID, "shadowMatrix"); //shadow
}

//-----------------------------------------------------------------------------------------------------------
void ePBRRender::Render(const Camera& camera, const std::vector<Light>& lights, std::vector<shObject>& objects)
{
  glUseProgram(pbrShader.ID);
  glUniform3fv(camPosLoc, 1, &camera.getPosition()[0]);

  glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(lights[0].light_position), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
  glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
  glm::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
  
  std::vector<glm::vec3> lpositions;
  std::vector<glm::vec3> lcolors;
  for (int i = 0; i < lights.size(); ++i)
  {
    lpositions.push_back(lights[i].light_position);
    lcolors.push_back(lights[i].diffuse);
  }

  std::string loc_p = "lightPositions";
  GLuint loc_pos = glGetUniformLocation(pbrShader.ID, loc_p.c_str());
  glUniform3fv(loc_pos, 4, &lpositions[0][0]);

  std::string loc_c = "lightColors";
  GLuint loc_col = glGetUniformLocation(pbrShader.ID, loc_c.c_str());
  glUniform3fv(loc_col, 4, &lcolors[0][0]);

  for (auto& object : objects)
  {
    glm::mat4 modelToProjectionMatrix = worldToProjectionMatrix * object->GetTransform()->getModelMatrix();
    glUniformMatrix4fv(fullTransformationUniformLocation, 1, GL_FALSE, &modelToProjectionMatrix[0][0]);
    glUniformMatrix4fv(modelToWorldMatrixUniformLocation, 1, GL_FALSE, &object->GetTransform()->getModelMatrix()[0][0]);
  
    if (object->GetRigger() != nullptr)
      matrices = object->GetRigger()->GetMatrices();
    else
      for (auto& m : matrices)
        m = UNIT_MATRIX;
  
    int loc = glGetUniformLocation(pbrShader.ID, "gBones");
    glUniformMatrix4fv(loc, 100, GL_FALSE, &matrices[0][0][0]);
  
    for (const IMesh* mesh : object->GetModel()->GetMeshes())
    {
      if (mesh->HasMaterial())
      {
        Material m = mesh->GetMaterial().value();
        glUniform3f(albedoLoc, m.diffuse[0], m.diffuse[1], m.diffuse[2]);
        glUniform1f(aoLoc, m.ao);
        glUniform1f(metallicLoc, m.metallic);
        glUniform1f(roughnessLoc, m.roughness);
        const_cast<IMesh*>(mesh)->Draw(); // draw should be const?
      }
    }
  }
}
