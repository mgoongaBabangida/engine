#include "Log.h"
#include <iostream>

namespace base
{
  //---------------------------------------------------------------
  void Log(const std::string& _info)
  {
    if(true)
      std::cout << _info << std::endl;
  }
}