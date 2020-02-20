#pragma once

#include "../Object.hpp"

namespace materia3
{

class ListObject : public Object
{
public:
   ListObject(const materia::Id id, const bool restore, std::unique_ptr<DatabaseSlot>&& slot);

private:
   void registerHandlers();
   void saveState();

   std::vector<materia::Id> mItems;
};

}