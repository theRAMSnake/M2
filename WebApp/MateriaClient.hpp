#pragma once

#include "materia/MateriaServiceProxy.hpp"
#include "materia/CalendarModel.hpp"
#include "materia/InboxModel.hpp"
#include "materia/JournalModel.hpp"
#include "materia/StrategyModel.hpp"
#include "materia/FreeDataModel.hpp"
#include "materia/ZmqPbChannel.hpp"

class MateriaClient
{
public:
   MateriaClient();

   CalendarModel& getCalendar();
   InboxModel& getInbox();
   JournalModel& getJournal();
   StrategyModel& getStrategy();
   FreeDataModel& getFreeData();

private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   ZmqPbChannel mChannel;

   std::unique_ptr<CalendarModel> mCalendar;
   std::unique_ptr<InboxModel> mInbox;
   std::unique_ptr<JournalModel> mJournal;
   std::unique_ptr<StrategyModel> mStrategy;
   std::unique_ptr<FreeDataModel> mFreeData;
};