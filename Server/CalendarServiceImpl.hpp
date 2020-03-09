#pragma once

#include <messages/calendar.pb.h>
#include <Core/ICalendar.hpp>

namespace materia
{

calendar::CalendarItem toProto(const CalendarItem& x)
{
   calendar::CalendarItem result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   result.set_timestamp(x.timestamp);
   result.set_text(x.text);
   result.set_reccurencytype(static_cast<calendar::ReccurencyType>(x.reccurencyType));

   return result;
}

CalendarItem fromProto(const calendar::CalendarItem& x)
{
   return {fromProto(x.id()), x.text(), x.timestamp(), static_cast<ReccurencyType>(x.reccurencytype())};
}

class CalendarServiceImpl : public calendar::CalendarService
{
public:
   CalendarServiceImpl(ICore& core)
   : mCalendar(core.getCalendar())
   {
   }

   virtual void Query(::google::protobuf::RpcController* controller,
                       const ::common::TimeRange* request,
                       ::calendar::CalendarItems* response,
                       ::google::protobuf::Closure* done)
   {
      auto result = mCalendar.query(request->timestampfrom(), request->timestampto());
      for(auto x : result)
      {
         auto newItem = response->add_items();
         newItem->CopyFrom(toProto(x));
      }
   }

   virtual void Next(::google::protobuf::RpcController* controller,
                       const ::calendar::NextQueryParameters* request,
                       ::calendar::CalendarItems* response,
                       ::google::protobuf::Closure* done)
   {
      auto result = mCalendar.next(request->timestampfrom(), request->limit());
      for(auto x : result)
      {
         auto newItem = response->add_items();
         newItem->CopyFrom(toProto(x));
      }
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCalendar.deleteItem(fromProto(*request));
      response->set_success(true);
   }

   virtual void EditItem(::google::protobuf::RpcController* controller,
                       const ::calendar::CalendarItem* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCalendar.replaceItem(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::calendar::CalendarItem* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mCalendar.insertItem(fromProto(*request))));
   }

private:
   materia::ICalendar& mCalendar;
};

}