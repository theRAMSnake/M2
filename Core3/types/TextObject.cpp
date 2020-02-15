#include "TextObject.hpp"

namespace materia3
{

void postSimpleResponce(const Message& msg, const std::string responce)
{
    throw 1;
}

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
        postSimpleResponce(msg, "textObject{\"text\":\"" + mText + "\"}");
    });

    registerHandler("describe", f);
}

}