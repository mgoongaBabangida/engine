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
  eyePositionWorldUniformLocation = glGetUniformLocation(pbrShader.ID, "eyePositionWorld");
  FarPlaneUniformLocation = glGetUniformLocation(pbrShader.ID, "far_plane");
}

//-----------------------------------------------------------------------------------------------------------
void ePBRRender::Render(const Camera& camera, std::vector<Light> lights, std::vector<shObject>& objects)
{
  glUseProgram(pbrShader.ID);
  glUniform3fv(camPosLoc, 1, &camera.getPosition()[0]);

  glm::mat4 worldToViewMatrix = glm::lookAt(glm::vec3(lights[0].light_position), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 shadowMatrix = camera.getProjectionBiasedMatrix() * worldToViewMatrix;
  glUniformMatrix4fv(shadowMatrixUniformLocation, 1, GL_FALSE, &shadowMatrix[0][0]);
  glUniform1f(FarPlaneUniformLocation, camera.getFarPlane());
  glUniform3fv(eyePositionWorldUniformLocation, 1, &camera.getPosition()[0]);
  glm::mat4 worldToProjectionMatrix = camera.getProjectionMatrix() * camera.getWorldToViewMatrix();
  
  for (int i = 0; i < lights.size(); ++i)
  {
    std::string loc_p = "lightPositions[" + std::to_string(i) + "]";
    GLuint loc_pos = glGetUniformLocation(pbrShader.ID, loc_p.c_str());
    glUniform3fv(loc_pos, 1, &lights[i].light_position[0]);
    
    std::string loc_c = "lightColors[" + std::to_string(i) + "]";
    GLuint loc_col = glGetUniformLocation(pbrShader.ID, loc_c.c_str());
    glUniform3fv(loc_col, 1, &lights[i].diffuse[0]);
  }

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
