#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <Client/MateriaClient.hpp>
#include <messages/container.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
#include "sqlite_modern_cpp/hdr/sqlite_modern_cpp.h"

namespace materia
{
   
boost::uuids::random_generator generator;

class ContainerServiceImpl : public container::ContainerService
{
public:
   ContainerServiceImpl(sqlite::database& db)
   : mDb(db)
   , mClient("ContainerService")
   , mEvents(mClient.getEvents())
   {
      mDb << "CREATE TABLE IF NOT EXISTS Containers (Name text, IsPublic boolean, Properties text)";
      std::cout << "Init ok\n";
   }

   void AddContainer(
      ::google::protobuf::RpcController* controller,
      const ::container::Container* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);

         if(!request->name().empty() && !hasContainer(request->name()))
         {
            mDb << "INSERT INTO Containers VALUES (?, ?, ?);"
               << request->name() 
               << request->is_public()
               << request->icon_id().guid();

            mDb << "CREATE TABLE " + request->name() + "Items" + " (id text primary key, json text, bin blob);";

            raiseChangedEvent(request->name());

            response->set_success(true);
         }
      }

  void GetPublicContainers(
     ::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::container::Containers* response,
      ::google::protobuf::Closure* done)
      {
         mDb << "SELECT * FROM Containers WHERE IsPublic = true" 
            >> [&](std::string name, bool isPublic, std::string id) 
            {
               auto c = response->add_containers();
               c->set_name(name);
               c->set_is_public(true);
               c->mutable_icon_id()->set_guid(id);
            };
      }

   void ClearContainer(
      ::google::protobuf::RpcController* controller,
      const ::common::StringMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         if(!request->content().empty() && hasContainer(request->content()))
         {
            mDb << "DELETE FROM " + request->content() + "Items";

            raiseChangedEvent(request->content());

            response->set_success(true);
         }
      }

   void DeleteContainer(
     ::google::protobuf::RpcController* controller,
      const ::common::StringMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         if(!request->content().empty() && hasContainer(request->content()))
         {
            mDb << "DELETE FROM Containers WHERE name = ?" << request->content();
            std::cout << "Delete ok\n";
            mDb << "DROP TABLE IF EXISTS " + request->content() + "Items";
            std::cout << "Drop ok\n";

            raiseChangedEvent(request->content());

            response->set_success(true);
         }
      }

  void GetItems(
     ::google::protobuf::RpcController* controller,
      const ::common::StringMessage* request,
      ::container::Items* response,
      ::google::protobuf::Closure* done)
      {
         if(!request->content().empty() && hasContainer(request->content()))
         {
            mDb << "SELECT * FROM " + request->content() + "Items"
               >> [&](std::string id, std::string json, std::vector<char> bin) 
               {
                  auto c = response->add_items();
                  c->mutable_id()->set_guid(id);
                  c->set_content(json);

                  std::cout << " bin_size: " << bin.size() << "\n";
                  if(!bin.empty())
                  {
                     c->mutable_blob()->resize(bin.size());
                     std::copy(bin.begin(), bin.end(), c->mutable_blob()->begin());
                  }
               };
         }
      }

  void InsertItems(
     ::google::protobuf::RpcController* controller,
      const ::container::ItemsOfContainer* request,
      ::common::IdSet* response,
      ::google::protobuf::Closure* done)
      {
         if(!request->container_name().empty() && hasContainer(request->container_name()))
         {
            auto ps = mDb << "INSERT INTO " + request->container_name() + "Items VALUES (?, ?, ?);";
            for(auto x : request->items().items())
            {
               auto newId = to_string(generator()); 
               ps << newId;
               ps << x.content();
               
               std::vector<char> blob;
               blob.resize(x.blob().size());
               std::copy(x.blob().begin(), x.blob().end(), blob.begin());

               ps << blob;

               response->add_idset()->set_guid(newId);
               ps++;
            }

            raiseChangedEvent(request->container_name());
         }
      }

  void ReplaceItems(
     ::google::protobuf::RpcController* controller,
      const ::container::ItemsOfContainer* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         if(!request->container_name().empty() && hasContainer(request->container_name()))
         {
            auto ps = mDb << "REPLACE INTO " + request->container_name() + "Items VALUES (?, ?, ?);";
            for(auto x : request->items().items())
            {
               ps << x.id().guid();
               ps << x.content();
               
               if(x.blob().size() > 0)
               {
                  std::vector<char> blob;
                  blob.resize(x.blob().size());
                  std::copy(x.blob().begin(), x.blob().end(), blob.begin());

                  ps << blob;
               }

               ps++;
            }

            raiseChangedEvent(request->container_name());
            response->set_success(true);
         }
      }

  void DeleteItems(
     ::google::protobuf::RpcController* controller,
      const ::container::DeleteItemsRequest* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         if(!request->container_name().empty() && hasContainer(request->container_name()))
         {
            auto ps = mDb << "DELETE FROM " + request->container_name() + "Items WHERE id = ?";
            for(auto x : request->id_set().idset())
            {
               ps << x.guid();
               ps++;
            }

            raiseChangedEvent(request->container_name());
            response->set_success(true);
         }
      }

  void Fetch(
     ::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::container::Backup* response,
      ::google::protobuf::Closure* done)
      {
         std::ifstream f("db.blob", std::ios::binary);
         response->mutable_blob()->insert(response->mutable_blob()->begin(), std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>());
      }

   void ExecFunc(
     ::google::protobuf::RpcController* controller,
      const ::container::Func* request,
      ::container::FuncResult* response,
      ::google::protobuf::Closure* done)
      {
         if(hasContainer(request->container_name()))
         {
            common::StringMessage msg;
            msg.set_content(request->container_name());

            container::Items items;

            GetItems(nullptr, &msg, &items, nullptr);

            switch(request->func_type())
            {
               case container::Sum:
               {
                  int sum = 0;
                  for(auto x : items.items())
                  {
                     try
                     {
                        sum += boost::lexical_cast<int>(x.content());
                     }
                     catch(...)
                     {

                     }
                  }
                  response->set_value(sum);
                  response->mutable_op_result()->set_success(true);
                  break;
               }

               case container::Count:
               {
                  response->set_value(items.items_size());
                  response->mutable_op_result()->set_success(true);
                  break;
               }

               default:
                  response->mutable_op_result()->set_success(false);
            }
         }
         else
         {
            response->mutable_op_result()->set_success(false);
         }
      }

private:
   bool hasContainer(const std::string& name)
   {
      bool result = false;
      mDb << "SELECT COUNT(*) FROM Containers WHERE name = ?" << name >> result;

      return result;
   }

   void raiseChangedEvent(const std::string& containerName)
   {
      namespace pt = boost::posix_time;

      ContainerUpdatedEvent ev;
      ev.type = EventType::ContainerUpdated;
      ev.containerName = containerName;
      ev.timestamp = pt::second_clock::local_time();

      mEvents.putEvent<ContainerUpdatedEvent>(ev);
   }

   sqlite::database mDb;
   materia::MateriaClient mClient;
   materia::Events& mEvents;
};

}

int main(int argc, char *argv[])
{
   try
   {
      sqlite::database db("db.blob");
      materia::ContainerServiceImpl serviceImpl(db);
      materia::InterprocessService<materia::ContainerServiceImpl> service(serviceImpl);
      
      service.provideAt("*:" + gContainerPort, "ContainerService");
   }
   catch(sqlite::sqlite_exception ex)
   {
      std::cout << ex.what();
   }
   
   return 0;
}