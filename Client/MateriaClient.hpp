#pragma once

#include <string>
#include <memory>

namespace materia
{

class IDatabase;
class MateriaClientImpl;
class IAdmin;
class IInbox;
class IActions;
class ICalendar;
class IContainer;
class IStrategy;
class IEvents;
class MateriaClient
{
public:
   MateriaClient(const std::string& clientName, const std::string& ip = "localhost");
   MateriaClient(const MateriaClient& other) = delete;
   void operator = (const MateriaClient& other) = delete;
   ~MateriaClient();

   IDatabase& getDatabase();
   IAdmin& getAdmin();
   IInbox& getInbox();
   IActions& getActions();
   ICalendar& getCalendar();
   IContainer& getContainer();
   IStrategy& getStrategy();
   IEvents& getEvents();

private:
   MateriaClientImpl*  mImpl;
};

}