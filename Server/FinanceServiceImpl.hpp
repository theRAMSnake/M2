#pragma once

#include <messages/finance.pb.h>
#include <Core/IFinance.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

finance::CategoryInfo toProto(const FinanceCategory& x)
{
   finance::CategoryInfo result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   result.set_name(x.name);

   return result;
}

FinanceCategory fromProto(const finance::CategoryInfo& x)
{
   return {fromProto(x.id()), x.name()};
}

finance::EventInfo toProto(const FinanceEvent& x)
{
   finance::EventInfo result;

   result.mutable_event_id()->CopyFrom(toProto(x.eventId));
   result.mutable_category_id()->CopyFrom(toProto(x.categoryId));
   result.set_details(x.details);
   result.set_amount_euro_cents(x.amountEuroCents);
   result.set_timestamp(x.timestamp);
   result.set_type(static_cast<finance::EventType>(x.type));

   return result;
}

FinanceEvent fromProto(const finance::EventInfo& x)
{
   return {fromProto(x.event_id()), fromProto(x.category_id()), static_cast<EventType>(x.type()), x.details(), x.amount_euro_cents(), x.timestamp()};
}

class FinanceServiceImpl : public finance::FinanceService
{
public:
   FinanceServiceImpl(ICore& core)
   : mFinance(core.getFinance())
   {
   }

   virtual void GetCategories(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::finance::CategoryItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mFinance.getCategories())
      {
         response->add_items()->CopyFrom(toProto(x));
      }
   }

   virtual void DeleteCategory(::google::protobuf::RpcController* controller,
                        const ::common::UniqueId* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFinance.removeCategory(fromProto(*request));
      response->set_success(true);
   }

   virtual void ReplaceCategory(::google::protobuf::RpcController* controller,
                        const ::finance::CategoryInfo* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFinance.replaceCategory(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddCategory(::google::protobuf::RpcController* controller,
                        const ::finance::CategoryInfo* request,
                        ::common::UniqueId* response,
                        ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mFinance.addCategory(fromProto(*request))));
   }

   virtual void DeleteEvent(::google::protobuf::RpcController* controller,
                        const ::common::UniqueId* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFinance.removeEvent(fromProto(*request));
      response->set_success(true);
   }

   virtual void ReplaceEvent(::google::protobuf::RpcController* controller,
                        const ::finance::EventInfo* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mFinance.replaceEvent(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddEvent(::google::protobuf::RpcController* controller,
                        const ::finance::EventInfo* request,
                        ::common::UniqueId* response,
                        ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mFinance.addEvent(fromProto(*request))));
   }

   virtual void QueryEvents(::google::protobuf::RpcController* controller,
                        const ::common::TimeRange* request,
                        ::finance::EventItems* response,
                        ::google::protobuf::Closure* done)
   {
      auto result = mFinance.queryEvents(request->timestampfrom(), request->timestampto());
      for(auto x : result)
      {
         auto newItem = response->add_items();
         newItem->CopyFrom(toProto(x));
      }
   }

   virtual void GetReport(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::finance::FinanceReport* response,
      ::google::protobuf::Closure* done)
   {
      auto result = mFinance.getReport();
      response->set_status(static_cast<finance::FinanceStatus>(result.status));
      response->set_balance(result.balance);  
   }

private:
   IFinance& mFinance;
};

}