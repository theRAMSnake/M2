#include "Session.hpp"
#include <thread>
#include <chrono>

namespace materia3
{

Session::Session(const materia::Id& id, const std::string& name)
: Object(id)
, mName(name)
{

}

void Session::forwardMessage(const materia::Id& destination, const std::string& type, const std::string& content)
{
   sendMessage(destination, type, content);
}

Message Session::pick()
{
   while(!mHasMsg)
   {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
   }

   std::unique_lock<std::mutex> lock(mMutex);
   mHasMsg = false;
   return mMsg;
}

void Session::handleMessage(const Message& msg)
{
   mMutex.lock();
   mMsg = msg;
   mMsg.receiver = materia::Id(mName);
   mHasMsg = true;
   mMutex.unlock();
}

}