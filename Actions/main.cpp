#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Common/Utils.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/IContainer.hpp>
#include <Client/IStrategy.hpp>
#include <Client/ICalendar.hpp>
#include <Client/IActions.hpp>
#include <Client/RemoteCollection.hpp>
#include <Client/private/ProtoConverter.hpp>

namespace materia
{

boost::uuids::random_generator generator;

template<>
ActionItem fromJson(const std::string& json)
{
   ActionItem result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.title = pt.get<std::string> ("title");
   result.description = pt.get<std::string> ("description");
   result.type = static_cast<materia::ActionType> (pt.get<int> ("type"));
   result.dataSourceId = pt.get<std::string> ("dataSourceId");

   return result;
}

template<>
std::string toJson(const ActionItem& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id.getGuid());
   pt.put ("title", from.title);
   pt.put ("description", from.description);
   pt.put ("type", static_cast<int>(from.type));
   pt.put ("dataSourceId", from.dataSourceId.getGuid());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

class IDataSource
{
public:
   using TItemsType = std::vector<ActionItem>;

   virtual TItemsType getItems() = 0;
   virtual void erase(const Id& id) = 0;
   virtual bool allowUpdate() const = 0;
   virtual void update(const ActionItem& item) = 0; 

   virtual ~IDataSource()
   {

   }
};

class ItemsDataSource : public IDataSource
{
public:
   ItemsDataSource(RemoteCollection<ActionItem>& items)
   : mItems(items)
   {

   }

   TItemsType getItems() override
   {
      TItemsType result(mItems.size());
      std::copy(mItems.begin(), mItems.end(), result.begin());

      return result;
   }

   void erase(const Id& id) override
   {
      auto pos = mItems.find(id);
      if(pos != mItems.end())
      {
         mItems.erase(pos);
      }
   }

   bool allowUpdate() const override
   {
      return true;
   }

   void update(const ActionItem& item) override
   {
      mItems.update(item);
   }

private:
   RemoteCollection<ActionItem>& mItems;
};

std::time_t getOneWeekAgo()
{
   auto day = boost::gregorian::day_clock::local_day();
   day -= boost::gregorian::date_duration(7);

   return day_to_time_t(day);
}

std::time_t getTodayNight()
{
   auto day = boost::gregorian::day_clock::local_day();
   day += boost::gregorian::date_duration(1);

   return day_to_time_t(day);
}

class CalendarDataSource : public IDataSource
{
public:
   CalendarDataSource(ICalendar& calendar)
   : mCalendar(calendar)
   {

   }

   TItemsType getItems() override
   {
      TItemsType result;

      auto calendarItems = mCalendar.query(getOneWeekAgo(), getTodayNight());

      for(auto c : calendarItems)
      {
         result.push_back({
            c.id,
            c.text,
            "",
            ActionType::Task,
            mDataSourceId
         });
      }

      return result;
   }

   void erase(const Id& id) override
   {
      mCalendar.deleteItem(id);
   }

   bool allowUpdate() const override
   {
      return false;
   }

   void update(const ActionItem& item) override
   {
      throw -1;
   }

private:
   Id mDataSourceId {"calendar"};
   ICalendar& mCalendar;
};

class StrategyDataSource : public IDataSource
{
public:
   StrategyDataSource(IStrategy& strategy)
   : mStrategy(strategy)
   {

   }

   TItemsType getItems() override
   {
      TItemsType result;

      for(auto t : getUnfinishedTasks())
      {
         result.push_back({
            t.id,
            t.name,
            t.notes,
            ActionType::Task,
            mDataSourceId
         });
      }

      return result;
   }

   void erase(const Id& id) override
   {
      auto items = getUnfinishedTasks();
      auto pos = find_by_id(items, id);
      if(pos != items.end())
      {
         pos->done = true;
         mStrategy.modifyTask(*pos);
      }
   }

   bool allowUpdate() const override
   {
      return false;
   }

   void update(const ActionItem& item) override
   {
      throw -1;
   }

private:
   std::vector<Task> getUnfinishedTasks()
   {
      std::vector<Task> result;

      auto goals = mStrategy.getGoals();
      for(auto g : goals)
      {
         if(g.focused)
         {
            auto items = mStrategy.getGoalItems(g.id);
            for(auto t : std::get<0>(items))
            {
               if(!t.done)
               {
                  result.push_back(t);
               }
            }
         }
      }

      return result;
   }

   Id mDataSourceId {"strategy"};
   IStrategy& mStrategy;
};

class ActionsServiceImpl : public actions::ActionsService
{
public:
   ActionsServiceImpl()
   : mClient("ActionsService")
   , mItems("actions", mClient.getContainer())
   {
      mDataSources.insert(std::make_pair(Id("items"), new ItemsDataSource(mItems)));
      mDataSources.insert(std::make_pair(Id("calendar"), new CalendarDataSource(mClient.getCalendar())));
      mDataSources.insert(std::make_pair(Id("strategy"), new StrategyDataSource(mClient.getStrategy())));
   }

   void GetItems(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto ds : mDataSources)
      {
         for(auto x : ds.second->getItems())
         {
            response->add_list()->CopyFrom(toProto(x));
         }
      }
   }

   void AddElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      //Adds only permited to ItemsDataSource
      materia::ActionItem newItem(fromProto(*request));
      newItem.id = to_string(generator());
      newItem.dataSourceId = materia::Id("items");

      mItems.insert(newItem);

      response->set_guid(newItem.id.getGuid());
   }

   void DeleteElement(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto id = fromProto(*request);

      for(auto ds : mDataSources)
      {
         auto& d = (*ds.second);
         d.erase(id);
      }

      response->set_success(true);
   }

   void EditElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto newItem = fromProto(*request);
      auto dsPos = mDataSources.find(newItem.dataSourceId);

      if(dsPos != mDataSources.end())
      {
         auto ds = dsPos->second;
         if(ds->allowUpdate())
         {
            ds->update(newItem);
            response->set_success(true);
            return;
         }
      }

      response->set_success(false);
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
   std::map<Id, std::shared_ptr<IDataSource>> mDataSources;
   RemoteCollection<ActionItem> mItems;
};

}

int main(int argc, char *argv[])
{
   materia::ActionsServiceImpl serviceImpl;
   materia::InterprocessService<materia::ActionsServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gActionsPort, "ActionsService");
   
   return 0;
}