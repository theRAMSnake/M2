#include <iostream>

#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>

#include <messages/actions.pb.h>
#include <messages/inbox.pb.h>

#include <boost/filesystem.hpp>

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

boost::filesystem::path g_dir("actions_service_data");

struct legacyActionItem
{
   actions::ActionInfo info;
   std::vector<legacyActionItem> children;
};

void addItem(const std::string& parentId, legacyActionItem& item, actions::ActionsService::Stub& service)
{
   item.info.mutable_parentid()->set_guid(parentId);

   common::UniqueId id;

   service.AddElement(nullptr, &item.info, &id, nullptr);

   for(auto x : item.children)
   {
      addItem(id.guid(), x, service);
   }
}

void fetchChildren(legacyActionItem& src, std::vector<legacyActionItem>& items)
{
   for (boost::filesystem::directory_iterator iter(g_dir); iter != boost::filesystem::directory_iterator(); ++iter)
   {
      std::ifstream file(iter->path().string());

      actions::ActionInfo info;
      info.ParseFromIstream(&file);

      file.close();

      if(info.parentid().guid() == src.info.id().guid())
      {
         legacyActionItem item = {info};
         fetchChildren(item, items);
      }
   }

   items.push_back(src);
}

int main(int argc,  char** argv)
{
   {
      MateriaServiceProvider<inbox::InboxService> pr("localhost");
      auto& service = pr.getService();
   
      boost::filesystem::path dir("inbox_service_data");
      for (boost::filesystem::directory_iterator iter(dir); iter != boost::filesystem::directory_iterator(); ++iter)
      {
         std::string content;
   
         std::ifstream file(iter->path().string());
   
         content = std::string((std::istreambuf_iterator<char>(file)),
                 std::istreambuf_iterator<char>());
   
         file.close();
   
         inbox::InboxItemInfo newItem;
         newItem.set_text(content);
   
         common::UniqueId id;
   
         service.AddItem(nullptr, &newItem, &id, nullptr);
      }
   }
   {
      MateriaServiceProvider<actions::ActionsService> pr("localhost");
      auto& service = pr.getService();

      std::vector<legacyActionItem> items;

      for (boost::filesystem::directory_iterator iter(g_dir); iter != boost::filesystem::directory_iterator(); ++iter)
      {
         std::ifstream file(iter->path().string());

         actions::ActionInfo info;
         info.ParseFromIstream(&file);

         file.close();

         if(info.parentid().guid().empty())
         {
            legacyActionItem item = {info};

            fetchChildren(item, items);
         }
      }

      for(auto x : items)
      {
         addItem("", x, service);
      }
   }
}