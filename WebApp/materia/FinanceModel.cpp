#include "FinanceModel.hpp"
#include <messages/common.pb.h>

FinanceModel::FinanceModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<FinanceModel::Category> FinanceModel::getCategories()
{
   common::EmptyMessage e;

   finance::CategoryItems items;

   mService.getService().GetCategories(nullptr, &e, &items, nullptr);

   std::vector<Category> result;

   for(auto x : items.items())
   {
      result.push_back({x.id().guid(), x.name()});
   }

   return result;
}

void FinanceModel::renameCategory(const materia::Id& id, const std::string& newName)
{
   finance::CategoryInfo itemToEdit;
   itemToEdit.mutable_id()->set_guid(id);
   itemToEdit.set_name(newName);

   common::OperationResultMessage dummy;
   mService.getService().ReplaceCategory(nullptr, &itemToEdit, &dummy, nullptr);
}

void FinanceModel::eraseCategory(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().DeleteCategory(nullptr, &idMsg, &dummy, nullptr);
}

materia::Id FinanceModel::addCategory(const std::string& name)
{
   finance::CategoryInfo itemToAdd;
   itemToAdd.set_name(name);

   common::UniqueId id;
   mService.getService().AddCategory(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}

std::vector<FinanceModel::Event> FinanceModel::loadEvents(const std::time_t timestampFrom, const std::time_t timestampTo, finance::EventType type)
{
   common::TimeRange r;
   r.set_timestampfrom(timestampFrom);
   r.set_timestampto(timestampTo);

   finance::EventItems events;
   mService.getService().QueryEvents(nullptr, &r, &events, nullptr);

   std::vector<Event> result;

   for(auto x : events.items())
   {
      if(x.type() == type)
      {
         result.push_back({x.event_id().guid(), x.category_id().guid(), x.details(), x.timestamp(), x.amount_euro_cents(), x.type()});
      }
   }

   return result;
}

materia::Id FinanceModel::addEvent(const Event& ev)
{
   finance::EventInfo itemToAdd;
   itemToAdd.set_amount_euro_cents(ev.amountOfEuroCents);
   itemToAdd.set_details(ev.details);
   itemToAdd.set_timestamp(ev.timestamp);
   itemToAdd.mutable_category_id()->set_guid(ev.categoryId.getGuid());
   itemToAdd.set_type(ev.eventType);

   common::UniqueId id;
   mService.getService().AddEvent(nullptr, &itemToAdd, &id, nullptr);

   return id.guid();
}

void FinanceModel::deleteEvent(const materia::Id& id)
{
   common::UniqueId idMsg;
   idMsg.set_guid(id.getGuid());

   common::OperationResultMessage dummy;
   mService.getService().DeleteEvent(nullptr, &idMsg, &dummy, nullptr);
}

void FinanceModel::modifyEvent(const Event& ev)
{
   finance::EventInfo item;
   item.mutable_event_id()->set_guid(ev.eventId.getGuid());
   item.set_amount_euro_cents(ev.amountOfEuroCents);
   item.set_details(ev.details);
   item.set_timestamp(ev.timestamp);
   item.mutable_category_id()->set_guid(ev.categoryId.getGuid());
   item.set_type(ev.eventType);

   common::OperationResultMessage res;
   mService.getService().ReplaceEvent(nullptr, &item, &res, nullptr);
}