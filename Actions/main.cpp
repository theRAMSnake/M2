#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace materia
{

boost::uuids::random_generator generator;
boost::filesystem::path g_dir("actions_service_data");

class ActionsServiceImpl : public actions::ActionsService
{
public:
   ActionsServiceImpl()
   {
      boost::filesystem::create_directory(g_dir);
   }

   void GetChildren(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      for (boost::filesystem::directory_iterator iter(g_dir); iter != boost::filesystem::directory_iterator(); ++iter)
      {
         std::ifstream file(iter->path().string());

         actions::ActionInfo info;
         info.ParseFromIstream(&file);

         if(info.parentid().guid() == request->guid())
         {
            response->mutable_list()->Add()->CopyFrom(info);
         }

         file.close();
      }
   }

   void GetParentlessElements(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      for (boost::filesystem::directory_iterator iter(g_dir); iter != boost::filesystem::directory_iterator(); ++iter)
      {
         std::ifstream file(iter->path().string());

         actions::ActionInfo info;
         info.ParseFromIstream(&file);

         if(info.parentid().guid().empty())
         {
            response->mutable_list()->Add()->CopyFrom(info);
         }

         file.close();
      }
   }

   void AddElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      if(request->parentid().guid().empty() || boost::filesystem::exists(g_dir / request->parentid().guid()))
      {
         std::string id = to_string(generator());
         
         actions::ActionInfo newItem(*request);
         newItem.mutable_id()->set_guid(id);
         saveItem(id, newItem);

         response->set_guid(id);
      }
   }

   void DeleteElement(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto id = request->guid();
      if(boost::filesystem::exists(g_dir / id))
      {
         boost::filesystem::remove(g_dir / id);

         actions::ActionsList children;
         GetChildren(0, request, &children, 0);
         for(int i = 0; i < children.list_size(); ++i)
         {
            common::OperationResultMessage dummy;
            DeleteElement(0, &children.list(i).id(), &dummy, 0);
         }

         response->set_success(true);
      }
      else
      {
         response->set_success(false);
      }
   }

   void EditElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto id = request->id().guid();
      auto parent_id = request->parentid().guid();

      if(id != parent_id)
      {
         if(boost::filesystem::exists(g_dir / id) &&
            (parent_id.empty() || boost::filesystem::exists(g_dir / parent_id)))
         {
            saveItem(request->id().guid(), *request);
            response->set_success(true);
            return;
         }
      }

      response->set_success(false);
   }

private:
   void saveItem(const std::string& fileName, const actions::ActionInfo& item)
   {
      std::ofstream f((g_dir / fileName).string());
      item.SerializeToOstream(&f);
      f.close();
   }
};

}

int main(int argc, char *argv[])
{
   materia::ActionsServiceImpl serviceImpl;
   materia::InterprocessService<materia::ActionsServiceImpl> service(serviceImpl);
   
   service.provideAt("*:5912", "ActionsService");
   
   return 0;
}