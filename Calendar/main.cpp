#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/MateriaServiceProxy.hpp>
#include <Common/PortLayout.hpp>
#include <messages/calendar.pb.h>
#include <messages/database.pb.h>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

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
   {
      mDbProxy.reset(new MateriaServiceProxy<database::DatabaseService>("CalendarService"));
      mDatabase = &mDbProxy->getService();
   }

   virtual void Query(::google::protobuf::RpcController* controller,
        const ::calendar::TimeRange* request,
        ::calendar::CalendarItems* response,
        ::google::protobuf::Closure* done)
   {
      database::DocumentQuery query;
      query.set_category(mCategory);
      {
         /*auto kvl = query.add_query();
         kvl->set_key("timestamp");
         kvl->set_type(database::QueryElementType::Between);
         kvl->set_value(boost::lexical_cast<std::string>(request->timestampfrom()));
         kvl->set_value2(boost::lexical_cast<std::string>(request->timestampto()));*/
         //Known Issue: Database doesn't work with with conditional query of CalendarItems
         //Fix if performance issue
      }

      database::Documents result;
      mDatabase->SearchDocuments(nullptr, &query, &result, nullptr);

      for(auto x : result.result())
      {
         calendar::CalendarItem item;
         from_json(x.body(), item);
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

      database::DocumentQuery query;
      query.set_category(mCategory);
      {
         /*auto kvl = query.add_query();
         kvl->set_key("timestamp");
         kvl->set_type(database::QueryElementType::Greater);
         kvl->set_value(boost::lexical_cast<std::string>(request->timestampfrom()));*/
         //Known Issue: Database doesn't work with conditional query of CalendarItems
         //Fix if performance issue
      }

      database::Documents result;
      mDatabase->SearchDocuments(nullptr, &query, &result, nullptr);

      std::vector<calendar::CalendarItem> fetchedItems;
      for(auto x : result.result())
      {
         calendar::CalendarItem item;
         from_json(x.body(), item);
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
      database::DocumentHeader head;
      head.set_key(request->guid());
      head.set_category(mCategory);
      common::OperationResultMessage result;

      mDatabase->DeleteDocument(nullptr, &head, &result, nullptr);
      response->set_success(result.success());
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
         const ::calendar::CalendarItem* request,
         ::common::OperationResultMessage* response,
         ::google::protobuf::Closure* done)
   {
      database::Document doc;
      doc.set_body(to_json(*request));
      doc.mutable_header()->set_category(mCategory);
      doc.mutable_header()->set_key(request->id().guid());

      common::OperationResultMessage result;
      mDatabase->ModifyDocument(nullptr, &doc, &result, nullptr);

      response->set_success(result.success());
      return;
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
         const ::calendar::CalendarItem* request,
         ::common::UniqueId* response,
         ::google::protobuf::Closure* done)
   {
      std::string id = to_string(generator());
      
      calendar::CalendarItem newItem(*request);
      newItem.mutable_id()->set_guid(id);

      common::UniqueId result;
      database::Document doc;
      doc.set_body(to_json(newItem));
      doc.mutable_header()->set_key(id);
      doc.mutable_header()->set_category(mCategory);
   
      mDatabase->AddDocument(nullptr, &doc, &result, nullptr);
      response->set_guid(result.guid());
   }

private:
   std::unique_ptr<MateriaServiceProxy<database::DatabaseService>> mDbProxy;
   database::DatabaseService_Stub* mDatabase;
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
