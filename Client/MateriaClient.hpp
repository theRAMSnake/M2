#pragma once

namespace materia
{

class MateriaClientImpl;
class MateriaClient
{
public:
   MateriaClient(const std::string& clientName, const std::string& ip = "localhost");

   IDatabase& getDatabase();
   IAdmin& getAdmin();
   IInbox& getInbox();
   IActions& getActions();
   ICalendar& getCalendar();
   IJournal& getJournal();
   IContainer& getContainer();
   IStrategy& getStrategy();
   IEvents& getEvents();

private:
   std::unique_ptr<MateriaClientImpl>  mImpl;
};

}