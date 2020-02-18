#include "ObjectManager.hpp"
#include "Core3/Logger.hpp"

#include "Core3/types/TextObject.hpp"

namespace materia3
{

ObjectManager::ObjectManager(Database& db)
: Object(materia::Id("object_manager"))
, mDb(db)
{
   auto f = MessageHandler([=](auto msg){
      Json p;
      //Check and use params

      auto newId = createObject(type);

      Json r;
      j.set("id", newId);

      sendMessage(msg.sender, "result", r.str());
   });

   registerHandler("create", f);
}

void ObjectManager::dispatchMessage(const Message& msg)
{
   bool handled = false;
   if(msg.receiver == mId)
   {
      handleMessage(msg);
      handled = true;
   }
   else if(mTemporaryObjects.find(msg.receiver) != mTemporaryObjects.end())
   {
      mTemporaryObjects.find(msg.receiver)->second->handleMessage(msg);
      handled = true;
   }
   else
   {
      auto slot = mDb.load(msg.receiver);

      if(slot)
      {
         auto obj = restoreObject(msg.receiver, slot);
         obj->handleMessage(msg);
         handled = true;
      }
   }

   if(!handled)
   {
      LOG_WARNING("Unable to reach destination: " + msg.receiver.getGuid());
   }
}

std::unique_ptr<Object> ObjectManager::restoreObject(const materia::Id& id, std::unique_ptr<DatabaseSlot>& slot)
{
   if(slot->getMetadata() == "text")
   {
      return std::make_unique<TextObject>(id, std::move(slot));
   }

   LOG_WARNING("Unable to restore object of type: " + slot->getMetadata());
   return std::unique_ptr<Object>();
}

}