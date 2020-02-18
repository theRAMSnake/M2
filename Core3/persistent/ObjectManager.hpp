#pragma once
#include <Core3/Object.hpp>

namespace materia3
{

class Database;
class ObjectManager : public Object
{
public:
   ObjectManager(Database& db);

   void dispatchMessage(const Message& msg);

   template<class T, class TParam>
   T& addTemporaryObject( const TParam p1)
   {
      auto id = materia::Id::generate();
      mTemporaryObjects.insert({id, std::unique_ptr<Object>(new T(id, p1))});
      return *static_cast<T*>(mTemporaryObjects[id].get());
   }

private:
   std::unique_ptr<Object> restoreObject(const materia::Id& id, std::unique_ptr<DatabaseSlot>& slot);

   Database& mDb;
   std::map<materia::Id, std::unique_ptr<Object>> mTemporaryObjects;
};

}