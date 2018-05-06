#pragma once
#include "messages/common.pb.h"

namespace materia
{

class Id
{
public:
   const static Id Invalid;

   Id();
   Id(const common::UniqueId& protoId);
   Id(const std::string& guid);

   common::UniqueId toProtoId() const;

   bool operator == (const Id& other) const;
   bool operator != (const Id& other) const;
   bool operator < (const Id& other) const;
   
   const std::string& getGuid() const;

private:
   std::string mGuid;
};

}