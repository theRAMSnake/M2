#include "ListObject.hpp"
#include "../Json.hpp"

namespace materia3
{

ListObject::ListObject(const materia::Id id, const bool restore, std::unique_ptr<DatabaseSlot>&& slot)
: Object(id, std::move(slot))
{
   if(!restore)
   {
      saveState();
   }
   else
   {
      Json j(mSlot->get());

      j.get("items", mItems);
   }

   registerHandlers();
}

void ListObject::saveState()
{
   Json j;
   j.set("items", mItems);

   mSlot->put(j.str());
}

void ListObject::registerHandlers()
{
    auto insert = MessageHandler([=](auto msg){

        Json j(msg.content);

        if(j.contains("id"))
        {
           mItems.push_back(j.get<materia::Id>("id"));
           saveState();
        }
        else
        {
           sendError(msg.sender, "failed to insert object");
        }
    });

    registerHandler("insert", insert);

    auto erase = MessageHandler([=](auto msg){

        Json j(msg.content);

        if(j.contains("id"))
        {
           auto pos = std::find(mItems.begin(), mItems.end(), j.get<materia::Id>("id"));
           if(pos != mItems.end())
           {
              mItems.erase(pos);
              sendMessage(j.get<materia::Id>("id"), "destroy", "");
              saveState();
           }
        }
        else
        {
           sendError(msg.sender, "failed to erase object");
        }
    });

    registerHandler("erase", erase);

    auto destroy = MessageHandler([=](auto msg){

        for(auto i : mItems)
        {
           sendMessage(i, "destroy", "");
        }

        mItems.clear();
        mSlot->clear();
    });

    registerHandler("destroy", destroy);

    auto describe = MessageHandler([=](auto msg){

        Json j;
        j.set("items", mItems);
        j.set("id", mId);

        sendMessage(msg.sender, "description", j.str());
    });

    registerHandler("describe", describe);
}

}