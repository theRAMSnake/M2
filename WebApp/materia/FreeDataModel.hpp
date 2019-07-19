#pragma once

#include <Common/Id.hpp>
#include <messages/freedata.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

class FreeDataModel
{
public:
   FreeDataModel(ZmqPbChannel& channel);

   struct Block
   {
      std::string name;
      int value;
   };

   std::optional<Block> get(const std::string& name);
   std::vector<Block> get();
   void set(const Block& b);
   void remove(const std::string& blockname);

private:
   MateriaServiceProxy<freedata::FreeDataService> mService;
};