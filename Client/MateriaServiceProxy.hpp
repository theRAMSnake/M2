#pragma once

#include "ZmqPbChannel.hpp"

namespace materia
{

template<class TService>
class MateriaServiceProxy
{
public:
   MateriaServiceProxy(materia::ZmqPbChannel& channel)
   : mService(&channel)
   {
      
   }
   
   typename TService::Stub& getService()
   {
      return mService;
   }
   
private:
   typename TService::Stub mService;
};

}

