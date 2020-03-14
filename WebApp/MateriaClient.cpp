#include "MateriaClient.hpp"

MateriaClient::MateriaClient(const std::string& password)
: mContext(1)
, mSocket(mContext, ZMQ_REQ)
, mChannel(mSocket, "webapp", password)
{
    mSocket.connect("tcp://localhost:5757");
    mCalendar.reset(new CalendarModel(mChannel));
    mInbox.reset(new InboxModel(mChannel));
    mJournal.reset(new JournalModel(mChannel));
    mStrategy.reset(new StrategyModel(mChannel));
    mFreeData.reset(new FreeDataModel(mChannel));
    mFinance.reset(new FinanceModel(mChannel));
    mReward.reset(new RewardModel(mChannel));
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

FinanceModel& MateriaClient::getFinance()
{
   return *mFinance;
}

RewardModel& MateriaClient::getReward()
{
   return *mReward;
}

StrategyModel& MateriaClient::getStrategy()
{
   return *mStrategy;
}

FreeDataModel& MateriaClient::getFreeData()
{
   return *mFreeData;
}