#pragma once
#include "game_assets.h"

#include <base/interfaces.h>

class eMesh;
class eModel;

//-----------------------------------------------
class Serializer
{
public:
  static void WriteMeshToFile(const eMesh& mesh, const std::string& filename);
  static void ReadMeshFromFile(eModel& model, const std::string& filename);
};