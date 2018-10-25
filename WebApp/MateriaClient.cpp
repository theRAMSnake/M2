#include "MateriaClient.hpp"

MateriaClient::MateriaClient()
: mContext(1)
, mSocket(mContext, ZMQ_REQ)
, mChannel(mSocket, "webapp")
, mCalendar(mChannel)
, mInbox(mChannel)
, mJournal(mChannel)
, mStrategy(mChannel)
{
    mSocket.connect("tcp://localhost:5757");
}

CalendarModel& MateriaClient::getCalendar()
{
   return mCalendar;
}

InboxModel& MateriaClient::getInbox()
{
   return mInbox;
}

JournalModel& MateriaClient::getJournal()
{
   return mJournal;
}

StrategyModel& MateriaClient::getStrategy()
{
   return mStrategy;
}