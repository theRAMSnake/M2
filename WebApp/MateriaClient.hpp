#pragma once

#include "materia/MateriaServiceProxy.hpp"
#include "materia/CalendarModel.hpp"
#include "materia/InboxModel.hpp"
#include "materia/JournalModel.hpp"
#include "materia/StrategyModel.hpp"
#include "materia/ZmqPbChannel.hpp"

class MateriaClient
{
public:
   MateriaClient();

   CalendarModel& getCalendar();
   InboxModel& getInbox();
   JournalModel& getJournal();
   StrategyModel& getStrategy();

private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   ZmqPbChannel mChannel;

   CalendarModel mCalendar;
   InboxModel mInbox;
   JournalModel mJournal;
   StrategyModel mStrategy;
   /*MateriaServiceProxy<inbox::InboxService> mInbox;
   MateriaServiceProxy<calendar::CalendarService> mCalendar;
   MateriaServiceProxy<journal::JournalService> mJournal;
   */
};