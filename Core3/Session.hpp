#pragma once
#include "Object.hpp"
#include <mutex>

namespace materia3
{

class Session : public Object
{
public:
   Session(const materia::Id& id, const std::string& name);

   void forwardMessage(const materia::Id& destination, const std::string& type, const std::string& content);
   Message pick();
   void handleMessage(const Message& msg) override;

private:
   const std::string mName;
   bool mHasMsg = false;
   Message mMsg;
   std::mutex mMutex;
};

}