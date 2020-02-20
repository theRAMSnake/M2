#include "ObjectManager.hpp"
#include "../Json.hpp"
#include "Core3/Logger.hpp"

#include "Core3/types/TextObject.hpp"
#include "Core3/types/ListObject.hpp"

namespace materia3
{

ObjectManager::ObjectManager(Database& db)
: Object(materia::Id("object_manager"))
, mDb(db)
{
   auto f = MessageHandler([=](auto msg){
      Json p(msg.content);
      if(p.contains("type"))
      {
         auto newId = createObject(p.get<std::string>("type"), p);
         if(newId != materia::Id::Invalid)
         {
            Json r;
            r.set("created_id", newId);

            sendMessage(msg.sender, "result", r.str());

            return;
         }

      }

      sendError(msg.sender, "failed to create object");
      
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
   else if(slot->getMetadata() == "list")
   {
      return std::make_unique<ListObject>(id, true, std::move(slot));
   }

   LOG_WARNING("Unable to restore object of type: " + slot->getMetadata());
   return std::unique_ptr<Object>();
}

materia::Id ObjectManager::createObject(const std::string& type, const Json& params)
{
   if(type == "text")
   {
      if(params.contains("text"))
      {
         materia::Id newId = materia::Id::generate();
         auto slot = mDb.allocate(newId, "text");
         TextObject result(newId, params.get<std::string>("text"), std::move(slot));

         return newId;
      }
   }
   else if(type == "list")
   {
      materia::Id newId = materia::Id::generate();
      auto slot = mDb.allocate(newId, "list");
      ListObject result(newId, false, std::move(slot));

      return newId;
   }

   return materia::Id::Invalid;
}

}