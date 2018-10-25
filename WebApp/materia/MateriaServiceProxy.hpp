#pragma once

#include "ZmqPbChannel.hpp"

template<class TService>
class MateriaServiceProxy
{
public:
   MateriaServiceProxy(ZmqPbChannel& channel)
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