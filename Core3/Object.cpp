#include "Object.hpp"
#include "Transport.hpp"

namespace materia3
{

Object::Object(const materia::Id id, std::unique_ptr<DatabaseSlot>&& slot)
: mId(id)
, mSlot(std::move(slot))
{

}

Object::Object(const materia::Id id)
: mId(id)
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

void Object::sendMessage(const materia::Id& destination, const std::string& type, const std::string& content)
{
    getTransport().push({mId, destination, type, content});
}

}