#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <messages/actions.pb.h>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <Client/MateriaClient.hpp>
#include <Client/IContainer.hpp>
#include <Client/RemoteCollection.hpp>
#include <Client/private/ProtoConverter.hpp>

namespace materia
{

boost::uuids::random_generator generator;

template<>
actions::ActionInfo fromJson(const std::string& json)
{
   actions::ActionInfo result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.mutable_id()->set_guid(pt.get<std::string> ("id"));
   result.mutable_parentid()->set_guid(pt.get<std::string> ("parent_id"));
   result.set_title(pt.get<std::string> ("title"));
   result.set_description(pt.get<std::string> ("description"));
   result.set_type(static_cast<actions::ActionType> (pt.get<int> ("type")));

   return result;
}

template<>
std::string toJson(const actions::ActionInfo& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id().guid());
   pt.put ("parent_id", from.parentid().guid());
   pt.put ("title", from.title());
   pt.put ("description", from.description());
   pt.put ("type", from.type());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

template<>
class RemoteCollectionItemTraits<actions::ActionInfo>
{
public:
   static Id getId(const actions::ActionInfo& item)
   {
      return fromProto(item.id());
   }
};

template<typename Iterator, typename Pred, typename Operation> void 
for_each_if(Iterator begin, Iterator end, Pred p, Operation op) 
{
    for(; begin != end; ++begin) 
    {
        if (p(*begin)) 
        {
            op(*begin);
        }
    }
}

class ActionsServiceImpl : public actions::ActionsService
{
public:
   ActionsServiceImpl()
   : mClient("ActionsService")
   , mItems("actions", mClient.getContainer())
   {
   }

   void GetChildren(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      getElementsWithParentIdEqualsTo(request->guid(), response);
   }

   void GetParentlessElements(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::actions::ActionsList* response,
                       ::google::protobuf::Closure* done)
   {
      getElementsWithParentIdEqualsTo(materia::Id::Invalid, response);
   }

   void AddElement(::google::protobuf::RpcController* controller,
                       const ::actions::ActionInfo* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      materia::Id parentId = fromProto(request->parentid());
      if(parentId == materia::Id::Invalid || is_item_exist(parentId))
      {
         std::string id = to_string(generator());

         actions::ActionInfo newItem(*request);
         newItem.mutable_id()->set_guid(id);

         mItems.insert(newItem);

         response->set_guid(id);
      }
   }

   bool is_item_exist(const materia::Id& id)
   {
      return mItems.find(id) != mItems.end();
   }

   void DeleteElement(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      auto item = fromProto(*request);
      auto pos = mItems.find(item);

      if(pos != mItems.end())
      {
         mItems.erase(pos);

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
      materia::Id id = request->id().guid();
      materia::Id parent_id = request->parentid().guid();

      if(id != parent_id)
      {
         if(parent_id == materia::Id::Invalid || is_item_exist(parent_id))
         {
            mItems.update(*request);
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
   void getElementsWithParentIdEqualsTo(materia::Id id, ::actions::ActionsList* response)
   {
      for_each_if(mItems.begin(), mItems.end(), 
         [&](auto x)->auto {return fromProto(x.parentid()) == id;},
         [&](auto x)->void {response->add_list()->CopyFrom(x);});
   }

   materia::MateriaClient mClient;
   RemoteCollection<actions::ActionInfo> mItems;
};

}

int main(int argc, char *argv[])
{
   materia::ActionsServiceImpl serviceImpl;
   materia::InterprocessService<materia::ActionsServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gActionsPort, "ActionsService");
   
   return 0;
}