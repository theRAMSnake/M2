#include <iostream>
#include "materia/Environment.hpp"

#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>

#include <messages/actions.pb.h>

template<class TService>
class MateriaServiceProvider
{
public:
   MateriaServiceProvider(const std::string& address)
   : mContext(1)
   , mSocket(mContext, ZMQ_REQ)
   , mChannel(mSocket, "Legacy")
   , mService(&mChannel)
   {
      mSocket.connect("tcp://" + address + ":" + gCentralPort);
   }
   
   typename TService::Stub& getService()
   {
      return mService;
   }
   
private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   materia::ZmqPbChannel mChannel;
   typename TService::Stub mService;
};

struct LegacyItem
{
   std::string name;
   std::string desc;
   std::vector<LegacyItem> children;
};

int main(int argc,  char** argv)
{
   materia::Environment env("Snake.materia");

   std::vector<LegacyItem> result;

   for(auto x : env.getProjectManager())
   {
      result.push_back({x.getName().ToString(), x.getNotes().ToString()});
      LegacyItem& p = result.back();

      for(auto y : x.getObjectives())
      {
         p.children.push_back({y.first.ToString()});
      }
     
      for(auto y : x.getSubTasks())
      {
         auto task = env.getTaskList().getTask(y);
         p.children.push_back({task.getObjective().ToString()});
      }
   }

   MateriaServiceProvider<actions::ActionsService> pr(argv[1]);
   auto& service = pr.getService();

   actions::ActionInfo rootActionItem;
   rootActionItem.set_title("Legacy");
   common::UniqueId rootId;

   service.AddElement(nullptr, &rootActionItem, &rootId, nullptr);

   for(auto x : result)
   {
      actions::ActionInfo prjActionItem;
      prjActionItem.set_title(x.name);
      prjActionItem.set_description(x.desc);
      prjActionItem.mutable_parentid()->set_guid(rootId.guid());
      common::UniqueId prjId;

      service.AddElement(nullptr, &prjActionItem, &prjId, nullptr);

      for(auto y : x.children)
      {
         actions::ActionInfo chActionItem;
         chActionItem.set_title(y.name);
         chActionItem.set_description(y.desc);
         chActionItem.mutable_parentid()->set_guid(prjId.guid());
         common::UniqueId dummy;

         service.AddElement(nullptr, &chActionItem, &dummy, nullptr);
      }
   }
}