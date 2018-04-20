#include "MateriaClient.hpp"

namespace materia
{

MateriaClient::MateriaClient(const std::string& clientName, const std::string& ip)
: mContext(1)
, mSocket(mContext, ZMQ_REQ)
, mChannel(mSocket, clientName)
, mDatabase(mChannel)
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

Database& MateriaClient::getDatabase()
{
   return mDatabase;
}

Actions& MateriaClient::getActions()
{
   return mActions;
}
Calendar& MateriaClient::getCalendar()
{
   return mCalendar;
}  
Journal& MateriaClient::getJournal()
{
   return mJournal;
}
Admin& MateriaClient::getAdmin()
{
   return mAdmin;
}
Inbox& MateriaClient::getInbox()
{
   return mInbox;
}
Container& MateriaClient::getContainer()
{
   return mContainer;
}
Events& MateriaClient::getEvents()
{
   return mEvents;
}

}