#pragma once

#include "Database.hpp"
#include <map>

namespace materia3
{

struct Message
{
    materia::Id sender;
    materia::Id receiver;
    std::string type;
    std::string content;
};

using MessageHandler = std::function<void(Message)>;

class Object
{
public:
    Object(const materia::Id id);
    Object(const materia::Id id, std::unique_ptr<DatabaseSlot>&& slot);

    virtual void handleMessage(const Message& msg);
    virtual ~Object(){}

protected:

    void registerHandler(const std::string& type, MessageHandler& handler);
    void sendMessage(const materia::Id& destination, const std::string& type, const std::string& content);

    const materia::Id mId;
    std::unique_ptr<DatabaseSlot> mSlot;

private:
    std::map<std::string, MessageHandler> mHandlers;
};

}