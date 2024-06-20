#include "stdafx.h"

#include "EngineBaseVisualsScript.h"
#include "MainContextBase.h"
#include "ObjectFactory.h"
#include "ModelManagerYAML.h"

//------------------------------------------------------------------------------
EngineBaseVisualsScript::EngineBaseVisualsScript(eMainContextBase* _game)
  : m_game(_game)
{
}

//------------------------------------------------------------------------------
EngineBaseVisualsScript::~EngineBaseVisualsScript()
{
}

//------------------------------------------------------------------------------
void EngineBaseVisualsScript::Update(float _tick)
{
  if (m_is_visible)
  {
    Transform trans;
    trans.setScale({ 0.008f, 0.008f, 0.008f });
    trans.setTranslation({ 1,0,0 });
    m_labels[0]->mvp = m_game->GetMainCamera().getProjectionMatrix()
      * m_game->GetMainCamera().getWorldToViewMatrix()
      * trans.getModelMatrix();
    trans.setTranslation({ 0,1,0 });
    m_labels[1]->mvp = m_game->GetMainCamera().getProjectionMatrix()
      * m_game->GetMainCamera().getWorldToViewMatrix()
      * trans.getModelMatrix();
    trans.setTranslation({ 0,0,1 });
    m_labels[2]->mvp = m_game->GetMainCamera().getProjectionMatrix()
      * m_game->GetMainCamera().getWorldToViewMatrix()
      * trans.getModelMatrix();

    arrowX->SetVisible(true);
    arrowY->SetVisible(true);
    arrowZ->SetVisible(true);
    m_grid->SetVisible(true);

    for (auto& lable : m_labels)
      lable->visible = true;
  }
  else
  {
    arrowX->SetVisible(false);
    arrowY->SetVisible(false);
    arrowZ->SetVisible(false);
    m_grid->SetVisible(false);

    for (auto& lable : m_labels)
      lable->visible = false;
  }
}

//------------------------------------------------------------------------------
void EngineBaseVisualsScript::Initialize()
{
  // Add grid x, z
  ObjectFactoryBase factory;
  LineMesh* line_mesh = new LineMesh({}, {}, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f });
  m_grid = factory.CreateObject(std::make_shared<SimpleModel>(line_mesh), eObject::RenderType::LINES, "Grid");
  m_game->AddObject(m_grid);

  std::vector<dbb::point> linePoints;
  std::vector <unsigned int> lineIndexes;
  for (int i = -10 ; i <= 10 ; ++i)
  {
    linePoints.push_back({i, 0, 10});
    linePoints.push_back({ i, 0, -10 });
    lineIndexes.push_back(linePoints.size()-1);
    lineIndexes.push_back(linePoints.size()-2);

    linePoints.push_back({ 10, 0, i });
    linePoints.push_back({ -10, 0, i });
    lineIndexes.push_back(linePoints.size() - 1);
    lineIndexes.push_back(linePoints.size() - 2);
  }
  line_mesh->UpdateData(linePoints, lineIndexes, { 1.0f, 1.0f , 1.0f, 1.0f });

  // Add arrows
  arrowX = factory.CreateObject(m_game->GetModelManager()->Find("ArrowBlend"), eObject::RenderType::PBR, "ArrowX");
  arrowX->GetTransform()->setRotation(0,0,glm::radians(-90.f));
  arrowX->GetTransform()->setScale({ 0.13f, 0.13f, 0.13f });
  Material m{ glm::vec3{1,0,0}, 0.0f, 0.0f, 1.0f,
    Texture::GetEmptyTextureId(), Texture::GetTexture1x1(TColor::WHITE).m_id,
    Texture::GetTexture1x1(TColor::BLUE).m_id,   Texture::GetTexture1x1(TColor::WHITE).m_id, Texture::GetTexture1x1(TColor::BLACK).m_id,
  false, false, false, false };
  arrowX->GetModel()->SetMaterial(m);
  m_game->AddObject(arrowX);

  arrowY = factory.CreateObject(m_game->GetModelManager()->Find("ArrowBlend"), eObject::RenderType::PBR, "ArrowY");
  arrowY->GetTransform()->setRotation(0, 0, 0);
  arrowY->GetTransform()->setScale({ 0.13f, 0.13f, 0.13f });
  Material m2{ glm::vec3{0,1,0}, 0.0f, 0.0f, 1.0f,
    Texture::GetEmptyTextureId(), Texture::GetTexture1x1(TColor::WHITE).m_id,
    Texture::GetTexture1x1(TColor::BLUE).m_id,   Texture::GetTexture1x1(TColor::WHITE).m_id, Texture::GetTexture1x1(TColor::BLACK).m_id,
  false, false, false, false };
  arrowY->GetModel()->SetMaterial(m2);
  m_game->AddObject(arrowY);

  arrowZ = factory.CreateObject(m_game->GetModelManager()->Find("ArrowBlend"), eObject::RenderType::PBR, "ArrowZ");
  arrowZ->GetTransform()->setRotation(glm::radians(90.f), 0, 0);
  arrowZ->GetTransform()->setScale({ 0.13f, 0.13f, 0.13f });
  Material m3{ glm::vec3{0,0,1}, 0.0f, 0.0f, 1.0f,
    Texture::GetEmptyTextureId(), Texture::GetTexture1x1(TColor::WHITE).m_id,
    Texture::GetTexture1x1(TColor::BLUE).m_id,   Texture::GetTexture1x1(TColor::WHITE).m_id, Texture::GetTexture1x1(TColor::BLACK).m_id,
  false, false, false, false };
  arrowZ->GetModel()->SetMaterial(m3);
  m_game->AddObject(arrowZ);

  // Add axis names m_labels
  m_labels.push_back(std::make_shared<Text>());
  m_labels[0]->content = "X";
  m_labels[0]->font = "ARIALN";
  m_labels[0]->pos_x = 0.0f;
  m_labels[0]->pos_y = 0.0f;
  m_labels[0]->scale = 1.0f;
  m_labels[0]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_labels[0]->mvp = m_game->GetMainCamera().getProjectionMatrix()
                   * m_game->GetMainCamera().getWorldToViewMatrix()
                   * UNIT_MATRIX;
  m_game->AddText(m_labels[0]);

  m_labels.push_back(std::make_shared<Text>());
  m_labels[1]->content = "Y";
  m_labels[1]->font = "ARIALN";
  m_labels[1]->pos_x = 0.0f;
  m_labels[1]->pos_y = 0.0f;
  m_labels[1]->scale = 1.0f;
  m_labels[1]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_labels[1]->mvp = m_game->GetMainCamera().getProjectionMatrix()
                   * m_game->GetMainCamera().getWorldToViewMatrix()
                   * UNIT_MATRIX;
  m_game->AddText(m_labels[1]);

  m_labels.push_back(std::make_shared<Text>());
  m_labels[2]->content = "Z";
  m_labels[2]->font = "ARIALN";
  m_labels[2]->pos_x = 0.0f;
  m_labels[2]->pos_y = 0.0f;
  m_labels[2]->scale = 1.0f;
  m_labels[2]->color = glm::vec3(1.0f, 1.0f, 1.0f);
  m_labels[2]->mvp = m_game->GetMainCamera().getProjectionMatrix()
                   * m_game->GetMainCamera().getWorldToViewMatrix()
                   * UNIT_MATRIX;
  m_game->AddText(m_labels[2]);
}
