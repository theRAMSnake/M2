#include "MateriaClient.hpp"

MateriaClient::MateriaClient()
: mContext(1)
, mSocket(mContext, ZMQ_REQ)
, mChannel(mSocket, "webapp")
{
    mSocket.connect("tcp://localhost:5757");
    mCalendar.reset(new CalendarModel(mChannel));
    mInbox.reset(new InboxModel(mChannel));
    mJournal.reset(new JournalModel(mChannel));
    mStrategy.reset(new StrategyModel(mChannel));
    mFreeData.reset(new FreeDataModel(mChannel));
}

CalendarModel& MateriaClient::getCalendar()
{
   return *mCalendar;
}

InboxModel& MateriaClient::getInbox()
{
   return *mInbox;
}

JournalModel& MateriaClient::getJournal()
{
   return *mJournal;
}

StrategyModel& MateriaClient::getStrategy()
{
   return *mStrategy;
}

FreeDataModel& MateriaClient::getFreeData()
{
   return *mFreeData;
}