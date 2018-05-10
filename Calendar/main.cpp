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
#include <Client/IDatabase.hpp>
#include <Client/private/ProtoConverter.hpp>

namespace materia
{
   
boost::uuids::random_generator generator;

void from_json(const std::string& json, calendar::CalendarItem& result)
{
   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.mutable_id()->set_guid(pt.get<std::string> ("id"));
   result.set_text(pt.get<std::string> ("text"));
   result.set_timestamp(pt.get<google::protobuf::int64> ("timestamp"));
}

std::string to_json(const calendar::CalendarItem& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id().guid());
   pt.put ("text", from.text());
   pt.put ("timestamp", from.timestamp());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

class CalendarServiceImpl : public calendar::CalendarService
{
public:
   CalendarServiceImpl()
   : mClient("CalendarService")
   , mDbProxy(mClient.getDatabase())
   {
      mDbProxy.setCategory(mCategory);
   }

   virtual void Query(::google::protobuf::RpcController* controller,
        const ::calendar::TimeRange* request,
        ::calendar::CalendarItems* response,
        ::google::protobuf::Closure* done)
   {     
      for(auto x : mDbProxy.getDocuments())
      {
         calendar::CalendarItem item;
         from_json(x.body, item);
         if(item.timestamp() > request->timestampfrom() && item.timestamp() < request->timestampto())
         {
            auto newItem = response->add_items();
            newItem->CopyFrom(item);
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
      for(auto x : mDbProxy.getDocuments())
      {
         calendar::CalendarItem item;
         from_json(x.body, item);
         if(item.timestamp() > request->timestampfrom())
         {
            fetchedItems.push_back(item);
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
      response->set_success(mDbProxy.deleteDocument(fromProto(*request)));
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
         const ::calendar::CalendarItem* request,
         ::common::OperationResultMessage* response,
         ::google::protobuf::Closure* done)
   {
      materia::Document doc { fromProto(request->id()), to_json(*request) };
      response->set_success(mDbProxy.replaceDocument(doc));
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
         const ::calendar::CalendarItem* request,
         ::common::UniqueId* response,
         ::google::protobuf::Closure* done)
   {
      std::string id = to_string(generator());
      
      calendar::CalendarItem newItem(*request);
      newItem.mutable_id()->set_guid(id);

      materia::Document doc { materia::Id::Invalid, to_json(newItem) };
      *response = toProto(mDbProxy.insertDocument(doc, materia::IdMode::Generate));
   }

private:
   materia::MateriaClient mClient;
   materia::IDatabase& mDbProxy;
   const std::string mCategory = "CAL";
};

}

int main(int argc, char *argv[])
{
   materia::CalendarServiceImpl serviceImpl;
   materia::InterprocessService<materia::CalendarServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gCalendarPort, "CalendarService");
   
   return 0;
}
