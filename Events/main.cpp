#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <messages/events.pb.h>

namespace std
{
   bool operator < (const events::EventInfo a, const events::EventInfo b)
   {
      return a.timestamp() < b.timestamp();
   }
}

bool operator < (const events::EventInfo a, const events::EventInfo b)
{
   return a.timestamp() < b.timestamp();
}

namespace materia
{

bool eventCmp(const events::EventInfo a, const events::EventInfo b)
{
   return a.timestamp() < b.timestamp();
}

class EventsServiceImpl : public events::EventsService
{
public:
   EventsServiceImpl()
   {
      events::EventInfo nfo;
      nfo.set_timestamp(boost::posix_time::to_time_t(boost::posix_time::second_clock::local_time()));
      nfo.set_type(events::EventType::HistoryStarted);

      mEvents.insert(nfo);
   }

   void GetEvents(::google::protobuf::RpcController* controller,
      const ::events::GetEventsRequest* request,
      ::events::Events* response,
      ::google::protobuf::Closure* done)
      {
         events::EventInfo nfo;
         nfo.set_timestamp(request->timestampfrom());

         for(auto iter = std::lower_bound(mEvents.begin(), mEvents.end(), nfo, eventCmp); iter != mEvents.end(); ++iter)
         {
            response->add_items()->CopyFrom(*iter);
         }
      }

   void PutEvent(::google::protobuf::RpcController* controller,
      const ::events::EventInfo* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         mEvents.insert(*request);
         if(mEvents.size() > mMaxHistorySize)
         {
            mEvents.erase(mEvents.begin());
         }
      }

private:
   std::multiset<events::EventInfo> mEvents;
   const std::size_t mMaxHistorySize = 200;
};

}

int main(int argc, char *argv[])
{
   materia::EventsServiceImpl serviceImpl;
   materia::InterprocessService<materia::EventsServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gEventsPort, "EventsService");
   
   return 0;
}