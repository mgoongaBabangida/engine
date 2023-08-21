#pragma once
#include <map>
#include <functional>

template<class Callback>
class Event
{
public:
  int32_t Subscribe(Callback _callback);
  void    Unsubscribe(int32_t _subscriptionHandle);
  template<class...Args>
  void Occur(Args... args);
private:
  std::map<int32_t, Callback> m_callbacks;
  int32_t counter = 0;
};

template<class Callback>
inline int32_t Event<Callback>::Subscribe(Callback _callback)
{
  m_callbacks.insert({ ++counter, _callback });
  return counter;
}

template<class Callback>
inline void Event<Callback>::Unsubscribe(int32_t _subscriptionHandle)
{
  m_callbacks.erase(_subscriptionHandle);
}

template<class Callback>
template<class ...Args>
inline void Event<Callback>::Occur(Args... args)
{
  for (auto& callback : m_callbacks)
    callback.second(args...);
}
