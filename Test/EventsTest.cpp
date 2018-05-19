#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/IEvents.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <thread>
#include "TestHelpers.hpp"

namespace std
{
std::ostream& operator << (std::ostream& str, const materia::EventType& evType)
{
   str << (int)evType;
   return str;
}
}

class EventsTest
{
public:
   EventsTest()
   : mClient("test")
   , mService(mClient.getEvents())
   {
      
   }

protected:

   materia::MateriaClient mClient;
   materia::IEvents& mService;
};

BOOST_FIXTURE_TEST_CASE( AddGetEvents, EventsTest ) 
{  
   using namespace std::chrono_literals;
   std::this_thread::sleep_for(3s);
   std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
   auto t {std::chrono::system_clock::to_time_t(now)};

   mClient.getEvents().putEvent(materia::Event{materia::EventType::ActionsUpdated, t});  
   mClient.getEvents().putEvent(materia::Event{materia::EventType::ActionsUpdated, std::chrono::system_clock::to_time_t(now + 10s)});

   materia::ContainerUpdatedEvent ev;
   ev.type = materia::EventType::ContainerUpdated;
   ev.timestamp = t;
   ev.containerName = "con1";

   mClient.getEvents().putEvent(ev);

   materia::IdEvent idEv;
   idEv.type = materia::EventType::GoalUpdated;
   idEv.timestamp = t;
   idEv.id = materia::Id("id");

   mClient.getEvents().putEvent(idEv);

   struct : public materia::IEventHandler
   {
      virtual void onGenericEvent(const materia::Event& event)
      {
         BOOST_CHECK_EQUAL(materia::EventType::ActionsUpdated, event.type);
         auto nextTs {std::chrono::system_clock::to_time_t(std::chrono::system_clock::from_time_t(mTs) + 10s)};
         BOOST_CHECK(mTs == event.timestamp || nextTs == event.timestamp);
         mNumItems++;
      }
      virtual void onContainerUpdated(const materia::ContainerUpdatedEvent& event)
      {
         BOOST_CHECK_EQUAL(materia::EventType::ContainerUpdated, event.type);
         BOOST_CHECK_EQUAL(mTs, event.timestamp);
         mNumItems++;
      }
      virtual void onIdEvent(const materia::IdEvent& event)
      {
         BOOST_CHECK_EQUAL(materia::EventType::GoalUpdated, event.type);
         BOOST_CHECK_EQUAL(mTs, event.timestamp);
         mNumItems++;
      }

      decltype(t) mTs;
      int mNumItems = 0;
   } evHdr;

   evHdr.mTs = t;

   mClient.getEvents().getEvents(std::chrono::system_clock::to_time_t(now - 1s), evHdr);
   BOOST_CHECK_EQUAL(4, evHdr.mNumItems);
}