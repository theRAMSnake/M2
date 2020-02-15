#include "Object.hpp"

namespace materia3
{

Object::Object(const materia::Id id, std::unique_ptr<DatabaseSlot>&& slot)
: mId(id)
, mSlot(std::move(slot))
{

}

void Object::handleMessage(const Message& msg)
{
    auto pos = mHandlers.find(msg.type);

    if(pos != mHandlers.end())
    {
        pos->second(msg);
    }
}

void Object::registerHandler(const std::string& type, MessageHandler& handler)
{
    mHandlers[type] = handler;
}

}