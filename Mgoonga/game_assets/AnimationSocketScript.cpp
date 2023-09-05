#include "stdafx.h"
#include "AnimationSocketScript.h"

#include <base/Object.h>

#include "MainContextBase.h"

//---------------------------------------------------
AnimationSocketScript::AnimationSocketScript(eMainContextBase* _game)
  :m_game(_game)
{
}

//---------------------------------------------------
void AnimationSocketScript::Initialize()
{
}

//---------------------------------------------------
void AnimationSocketScript::Update(float _tick)
{
  if (object != nullptr && object->GetRigger() != nullptr)
  {
    const auto& sockets = object->GetRigger()->GetSockets();
    if (object->GetRigger()->GetSockets().size() != sockets_count) //socket added
    {
      const AnimationSocket& socket = sockets.back();
      for (auto& obj : m_game->GetObjects())
      {
        if (socket.m_socket_object == obj.get())
        {
          object->AddChildObject(obj);
          m_game->DeleteObject(obj);
        }
      }
    }

    if (!sockets.empty())
    {
      for (auto& obj : object->GetChildrenObjects())
      {
        auto socket = std::find_if(sockets.begin(), sockets.end(), [obj](const AnimationSocket& _socket) { return _socket.m_socket_object == obj.get(); });
        if (socket != sockets.end())
        {
          const auto& matrices =  object->GetRigger()->GetMatrices();
          glm::mat4 cur_bone_matrix = matrices[socket->m_bone_id];
          glm::mat4 socket_matrix = object->GetTransform()->getModelMatrix() * cur_bone_matrix * glm::inverse(object->GetTransform()->getModelMatrix()) * socket->m_pre_transform;
          obj->GetTransform()->setModelMatrix(socket_matrix);
        }
      }
    }
  }
}
