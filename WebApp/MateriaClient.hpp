#pragma once

#include "materia/MateriaServiceProxy.hpp"
#include "materia/CalendarModel.hpp"
#include "materia/InboxModel.hpp"
#include "materia/JournalModel.hpp"
#include "materia/StrategyModel.hpp"
#include "materia/FreeDataModel.hpp"
#include "materia/FinanceModel.hpp"
#include "materia/RewardModel.hpp"
#include "materia/ZmqPbChannel.hpp"

class MateriaClient
{
public:
   MateriaClient(const std::string& password);

   CalendarModel& getCalendar();
   InboxModel& getInbox();
   JournalModel& getJournal();
   StrategyModel& getStrategy();
   FreeDataModel& getFreeData();
   FinanceModel& getFinance();
   RewardModel& getReward();

private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   ZmqPbChannel mChannel;

   std::unique_ptr<CalendarModel> mCalendar;
   std::unique_ptr<InboxModel> mInbox;
   std::unique_ptr<JournalModel> mJournal;
   std::unique_ptr<StrategyModel> mStrategy;
   std::unique_ptr<FreeDataModel> mFreeData;
   std::unique_ptr<FinanceModel> mFinance;
   std::unique_ptr<RewardModel> mReward;
};