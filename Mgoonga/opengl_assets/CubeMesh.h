#pragma once
#include <base/interfaces.h>

class CubeMesh : public IMesh
{
public:
  CubeMesh();
  ~CubeMesh();
  virtual void							 Draw() override;
  virtual const std::string& Name() const override { return name; }

protected:
  unsigned int cubeVAO = 0;
  unsigned int cubeVBO = 0;
  std::string name = "Cube Mesh";
};