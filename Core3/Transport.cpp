#include "Transport.hpp"
#include "Logger.hpp"
#include "persistent/ObjectManager.hpp"

namespace materia3
{

Transport::Transport(ObjectManager& om)
: mOm(om)
{
   mThread = std::thread(std::bind(&Transport::runLoop, this));
}

Transport::~Transport()
{
   mStop = true;
   mCv.notify_one();
   mThread.join();
}

void Transport::runLoop()
{
   while(!mStop)
   {
      Message curMsg;

      {
         std::unique_lock<std::mutex> l(mMutex);

         if(mQueue.empty())
         {
            mCv.wait(l);
         }

         if(mStop)
         {
            return;
         }

         curMsg = mQueue.front();
         mQueue.pop();
      }

      procMsg(curMsg);
   }
}

void Transport::push(const Message& msg)
{
   std::unique_lock<std::mutex> l(mMutex);

   mQueue.push(msg);

   if(mQueue.size() > 50)
   {
      LOG_WARNING("Transport queue size is greater than 50");
   }

   mCv.notify_one();
}

void Transport::procMsg(const Message& msg)
{
   LOG(msg.sender.getGuid() + "->" + msg.receiver.getGuid() + " " + msg.type + "(" + msg.content + ")");
   mOm.dispatchMessage(msg);
}

Transport* gTransport = nullptr;

Transport& getTransport()
{
   return *gTransport;
}

void setTransport(Transport& t)
{
   gTransport = &t;
}

}