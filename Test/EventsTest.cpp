#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Client/MateriaClient.hpp>

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
   materia::Events& mService;
};

BOOST_FIXTURE_TEST_CASE( AddGetEvents, EventsTest ) 
{  
   namespace pt = boost::posix_time;
   auto ts = pt::second_clock::local_time();

   mClient.getEvents().putEvent<materia::Event>(materia::Event{materia::EventType::ActionsUpdated, ts});  
   mClient.getEvents().putEvent<materia::Event>(materia::Event{materia::EventType::ActionsUpdated, ts + pt::seconds(10)});

   materia::ContainerUpdatedEvent ev;
   ev.type = materia::EventType::ContainerUpdated;
   ev.timestamp = ts;
   ev.containerName = "con1";

   mClient.getEvents().putEvent<materia::ContainerUpdatedEvent>(ev);

   materia::IdEvent idEv;
   idEv.type = materia::EventType::GoalUpdated;
   idEv.timestamp = ts;
   idEv.id = materia::Id("id");

   mClient.getEvents().putEvent<materia::IdEvent>(idEv);

   struct : public materia::IEventHandler
   {
      virtual void onGenericEvent(const materia::Event& event)
      {
         BOOST_CHECK_EQUAL(materia::EventType::ActionsUpdated, event.type);
         BOOST_CHECK(mTs == event.timestamp || mTs + pt::seconds(10) == event.timestamp);
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

      decltype(ts) mTs;
      int mNumItems = 0;
   } evHdr;

   evHdr.mTs = ts;

   mClient.getEvents().getEvents(ts - pt::seconds(1), evHdr);
   BOOST_CHECK_EQUAL(4, evHdr.mNumItems);
}