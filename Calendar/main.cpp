#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <messages/calendar.pb.h>
#include <messages/database.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/RemoteCollection.hpp>
#include <Client/ICalendar.hpp>
#include <Client/private/ProtoConverter.hpp>

namespace materia
{
   
boost::uuids::random_generator generator;

template<>
CalendarItem fromJson(const std::string& json)
{
   CalendarItem result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.text = pt.get<std::string> ("text");
   result.timestamp = pt.get<std::time_t> ("timestamp");

   return result;
}

template<>
std::string toJson(const CalendarItem& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id.getGuid());
   pt.put ("text", from.text);
   pt.put ("timestamp", from.timestamp);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

class CalendarServiceImpl : public calendar::CalendarService
{
public:
   CalendarServiceImpl()
   : mClient("CalendarService")
   , mItems("calendar", mClient.getContainer())
   {
      
   }

   virtual void Query(::google::protobuf::RpcController* controller,
        const ::calendar::TimeRange* request,
        ::calendar::CalendarItems* response,
        ::google::protobuf::Closure* done)
   {     
      for(auto x : mItems)
      {
         if(x.timestamp > request->timestampfrom() && x.timestamp < request->timestampto())
         {
            auto newItem = response->add_items();
            newItem->CopyFrom(toProto(x));
         }
      }
   }

   virtual void Next(::google::protobuf::RpcController* controller,
         const ::calendar::NextQueryParameters* request,
         ::calendar::CalendarItems* response,
         ::google::protobuf::Closure* done)
   {
      if(request->limit() <= 0)
      {
         return;
      }

      std::vector<calendar::CalendarItem> fetchedItems;
      for(auto x : mItems)
      {
         if(x.timestamp > request->timestampfrom())
         {
            fetchedItems.push_back(toProto(x));
         }
      }

      std::sort(fetchedItems.begin(), fetchedItems.end(), [](auto a, auto b)->bool {return a.timestamp() < b.timestamp();});

      int count = 0;
      for(auto x : fetchedItems)
      {
         auto newItem = response->add_items();
         newItem->CopyFrom(x);
         count++;

         if(count == request->limit())
         {
            break;
         }
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
         const ::common::UniqueId* request,
         ::common::OperationResultMessage* response,
         ::google::protobuf::Closure* done)
   {
      auto pos = mItems.find(fromProto(*request));
      bool found = pos != mItems.end();
      if(found)
      {
         mItems.erase(pos);
      }

      response->set_success(found);
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
         const ::calendar::CalendarItem* request,
         ::common::OperationResultMessage* response,
         ::google::protobuf::Closure* done)
   {
      auto pos = mItems.find(fromProto(request->id()));
      bool found = pos != mItems.end();
      if(found)
      {
         mItems.update(fromProto(*request));
      }

      response->set_success(found);
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
         const ::calendar::CalendarItem* request,
         ::common::UniqueId* response,
         ::google::protobuf::Closure* done)
   {
      std::string id = to_string(generator());
      
      calendar::CalendarItem newItem(*request);
      newItem.mutable_id()->set_guid(id);

      mItems.insert(fromProto(newItem));

      response->set_guid(id);
   }

   void Clear(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         mItems.clear();
      }

private:
   materia::MateriaClient mClient;
   RemoteCollection<CalendarItem> mItems;
};

}

int main(int argc, char *argv[])
{
   materia::CalendarServiceImpl serviceImpl;
   materia::InterprocessService<materia::CalendarServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gCalendarPort, "CalendarService");
   
   return 0;
}
