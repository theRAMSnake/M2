#include "logger.h"

void Logger::log(const QString& msg)
{
    emit onNewMessage({msg, MessageType::Info});
}
