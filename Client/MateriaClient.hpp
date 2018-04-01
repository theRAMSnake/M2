#pragma once

#include "ZmqPbChannel.hpp"
#include "Database.hpp"
#include "Admin.hpp"
#include "Inbox.hpp"
#include "Actions.hpp"
#include "Calendar.hpp"
#include "Journal.hpp"
#include <Common/PortLayout.hpp>

namespace materia
{

class MateriaClient
{
public:
   MateriaClient(const std::string& clientName, const std::string& ip = "localhost");

   Database& getDatabase();
   Admin& getAdmin();
   Inbox& getInbox();
   Actions& getActions();
   Calendar& getCalendar();
   Journal& getJournal();

private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   materia::ZmqPbChannel mChannel;

   Database mDatabase;
   Admin mAdmin;
   Inbox mInbox;
   Actions mActions;
   Calendar mCalendar;
   Journal mJournal;
};

}