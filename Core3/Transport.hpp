#pragma once
#include "Object.hpp"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace materia3
{

class ObjectManager;
class Transport
{
public:
   Transport(ObjectManager& om);
   ~Transport();

   void push(const Message& msg);

private:
   void runLoop();
   void procMsg(const Message& msg);

   ObjectManager& mOm;
   std::thread mThread;
   std::queue<Message> mQueue;
   std::mutex mMutex;
   std::condition_variable mCv;
   bool mStop = false;
};

Transport& getTransport();
void setTransport(Transport& t);

}