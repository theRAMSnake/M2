#include "TextObject.hpp"
#include "../Json.hpp"

namespace materia3
{

TextObject::TextObject(const materia::Id id, const std::string& text, std::unique_ptr<DatabaseSlot>&& slot)
: Object(id, std::move(slot))
, mText(text)
{
    mSlot->put(mText);

    registerHandlers();
}

TextObject::TextObject(const materia::Id id, std::unique_ptr<DatabaseSlot>&& slot)
: Object(id, std::move(slot))
, mText(mSlot->get())
{
    registerHandlers();
}

void TextObject::registerHandlers()
{
    auto f = MessageHandler([=](auto msg){

        Json j;
        j.set("text", mText);
        j.set("id", mId);

        sendMessage(msg.sender, "description", j.str());
    });

    registerHandler("describe", f);
}

}