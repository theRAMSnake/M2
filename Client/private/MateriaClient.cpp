#include "../MateriaClient.hpp"
#include <Common/PortLayout.hpp>
#include "ZmqPbChannel.hpp"
#include "Admin.hpp"
#include "Inbox.hpp"
#include "Actions.hpp"
#include "Calendar.hpp"
#include "Container.hpp"
#include "Journal.hpp"
#include "Events.hpp"
#include "Strategy.hpp"

namespace materia
{

struct MateriaClientImpl
{
   MateriaClientImpl(const std::string& clientName, const std::string& ip)
   : mContext(1)
   , mSocket(mContext, ZMQ_REQ)
   , mChannel(mSocket, clientName)
   , mAdmin(mChannel)
   , mInbox(mChannel)
   , mActions(mChannel)
   , mCalendar(mChannel)
   , mJournal(mChannel)
   , mContainer(mChannel)
   , mEvents(mChannel)
   , mStrategy(mChannel)
   {
      mSocket.connect("tcp://" + ip + ":" + gCentralPort);
   }

   zmq::context_t mContext;
   zmq::socket_t mSocket;
   materia::ZmqPbChannel mChannel;

   Admin mAdmin;
   Inbox mInbox;
   Actions mActions;
   Calendar mCalendar;
   Journal mJournal;
   Container mContainer;
   Events mEvents;
   Strategy mStrategy;
};

MateriaClient::MateriaClient(const std::string& clientName, const std::string& ip)
: mImpl(new MateriaClientImpl(clientName, ip))
{

}

MateriaClient::~MateriaClient()
{
   delete mImpl;
}

IActions& MateriaClient::getActions()
{
   return mImpl->mActions;
}

ICalendar& MateriaClient::getCalendar()
{
   return mImpl->mCalendar;
}

IAdmin& MateriaClient::getAdmin()
{
   return mImpl->mAdmin;
}

IInbox& MateriaClient::getInbox()
{
   return mImpl->mInbox;
}

IContainer& MateriaClient::getContainer()
{
   return mImpl->mContainer;
}

IEvents& MateriaClient::getEvents()
{
   return mImpl->mEvents;
}

IStrategy& MateriaClient::getStrategy()
{
   return mImpl->mStrategy;
}

}