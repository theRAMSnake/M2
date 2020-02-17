#pragma once

#include "Database.hpp"
#include <map>

namespace materia3
{

struct Message
{
    std::string sender;
    std::string receiver;
    std::string type;
    std::string content;
};

using MessageHandler = std::function<void(Message)>;

class Object
{
public:
    Object(const materia::Id id, std::unique_ptr<DatabaseSlot>&& slot);

    void handleMessage(const Message& msg);

protected:

    void registerHandler(const std::string& type, MessageHandler& handler);
    void sendMessage(const std::string& destination, const std::string& type, const std::string& content);

    const materia::Id mId;
    std::unique_ptr<DatabaseSlot>&& mSlot;

private:
    std::map<std::string, MessageHandler> mHandlers;
};

}