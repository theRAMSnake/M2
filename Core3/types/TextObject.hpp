#pragma once

#include "../Object.hpp"

namespace materia3
{

class TextObject : public Object
{
public:
    TextObject(const materia::Id id, const std::string& text, std::unique_ptr<DatabaseSlot>&& slot);
    TextObject(const materia::Id id, std::unique_ptr<DatabaseSlot>&& slot);

private:
    void registerHandlers();

    std::string mText;
};

}