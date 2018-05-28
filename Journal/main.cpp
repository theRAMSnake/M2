#include <iostream>
#include <fstream>
#include <Common/InterprocessService.hpp>
#include <Common/PortLayout.hpp>
#include <messages/journal.pb.h>
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

template<>
journal::IndexItem fromJson(const std::string& json)
{
   journal::IndexItem result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.mutable_journalitem()->mutable_id()->set_guid(pt.get<std::string> ("id"));
   result.mutable_journalitem()->mutable_folderid()->set_guid(pt.get<std::string> ("folder_parent_id"));
   result.mutable_journalitem()->set_title(pt.get<std::string> ("title"));
   result.set_modifiedtimestamp(pt.get<decltype(result.modifiedtimestamp())> ("modified"));
   result.set_ispage(pt.get<bool> ("isPage"));

   return result;
}

template<>
std::string toJson(const journal::IndexItem& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.journalitem().id().guid());
   pt.put ("folder_parent_id", from.journalitem().folderid().guid());
   pt.put ("title", from.journalitem().title());
   pt.put ("modified", from.modifiedtimestamp());
   pt.put ("isPage", from.ispage());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

template<>
journal::Page fromJson(const std::string& json)
{
   journal::Page result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.mutable_journalitem()->mutable_id()->set_guid(pt.get<std::string> ("id"));
   result.mutable_journalitem()->mutable_folderid()->set_guid(pt.get<std::string> ("folder_parent_id"));
   result.mutable_journalitem()->set_title(pt.get<std::string> ("title"));
   result.set_content(pt.get<std::string> ("content"));

   return result;
}

template<>
std::string toJson(const journal::Page& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.journalitem().id().guid());
   pt.put ("folder_parent_id", from.journalitem().folderid().guid());
   pt.put ("title", from.journalitem().title());
   pt.put ("content", from.content());

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

template<>
class RemoteCollectionItemTraits<journal::IndexItem>
{
public:
   static Id getId(const journal::IndexItem& item)
   {
      return fromProto(item.journalitem().id());
   }
};

template<>
class RemoteCollectionItemTraits<journal::Page>
{
public:
   static Id getId(const journal::Page& item)
   {
      return fromProto(item.journalitem().id());
   }
};

boost::uuids::random_generator generator;

class JournalServiceImpl : public journal::JournalService
{
public:
   JournalServiceImpl()
   : mClient("JournalService")
   , mIndex("journal_index", mClient.getContainer())
   , mPages("journal_pages", mClient.getContainer())
   {

   }

   virtual void InsertFolder(::google::protobuf::RpcController* controller,
      const ::journal::InsertFolderParams* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         materia::Id parentId = fromProto(request->parent_folder_id());
         
         constexpr bool isPage = false;
         response->set_guid(insertIndexItem(parentId, request->title(), isPage).getGuid());
      }

   virtual void InsertPage(::google::protobuf::RpcController* controller,
      const ::journal::InsertPageParams* request,
      ::common::UniqueId* response,
      ::google::protobuf::Closure* done)
      {
         materia::Id parentId = fromProto(request->parent_folder_id());
         
         constexpr bool isPage = true;
         auto id = insertIndexItem(parentId, request->title(), isPage).getGuid();

         journal::Page pg;
         pg.mutable_journalitem()->CopyFrom(mIndex.find(id)->journalitem());
         pg.set_content(request->content());

         mPages.insert(pg);

         response->set_guid(id);
      }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         auto iter = mIndex.find(fromProto(*request));
         if(iter != mIndex.end())
         {
            auto id = fromProto(iter->journalitem().id());

            if(iter->ispage())
            {
               mPages.erase(mPages.find(id));
            }
            else
            {
               std::vector<common::UniqueId> children;
               for(auto x : mIndex)
               {
                  if(fromProto(x.journalitem().folderid()) == id)
                  {
                     children.push_back(x.journalitem().id());
                  }
               }

               for(auto x : children)
               {
                  DeleteItem(nullptr, &x, response, nullptr);
               }
            }

            mIndex.erase(iter);
            response->set_success(true);
         }
      }

   virtual void UpdateFolder(::google::protobuf::RpcController* controller,
      const ::journal::JournalItem* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         auto iter = mIndex.find(fromProto(request->id()));
         if(iter != mIndex.end())
         {
            auto item = *iter;
            item.mutable_journalitem()->CopyFrom(*request);
            item.set_modifiedtimestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

            mIndex.update(item);
            response->set_success(true);
         }
      }

   virtual void UpdatePage(::google::protobuf::RpcController* controller,
      const ::journal::Page* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         response->set_success(false);
         auto id = fromProto(request->journalitem().id());
         auto iter = mIndex.find(id);
         if(iter != mIndex.end())
         {
            auto item = *iter;
            item.mutable_journalitem()->CopyFrom(request->journalitem());
            item.set_modifiedtimestamp(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));

            mIndex.update(item);

            auto page = *mPages.find(id);
            page.CopyFrom(*request);

            mPages.update(page);
            response->set_success(true);
         }
      }

   virtual void GetIndex(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::journal::Index* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mIndex)
         {
            response->add_items()->CopyFrom(x);
         }
      }

   virtual void Search(::google::protobuf::RpcController* controller,
      const ::common::StringMessage* request,
      ::journal::SearchResult* response,
      ::google::protobuf::Closure* done)
      {
         for(auto x : mPages)
         {
            auto pos = x.content().find(request->content());
            while(pos != std::string::npos)
            {
               response->add_pageid()->CopyFrom(x.journalitem().id());
               response->add_position(pos);

               pos = x.content().find(request->content(), pos + 1);
            }
         }
      }

   virtual void GetPage(::google::protobuf::RpcController* controller,
      const ::common::UniqueId* request,
      ::journal::Page* response,
      ::google::protobuf::Closure* done)
      {
         auto iter = mPages.find(fromProto(*request));
         if(iter != mPages.end())
         {
            response->CopyFrom(*iter);
         }
      }

   void Clear(::google::protobuf::RpcController* controller,
      const ::common::EmptyMessage* request,
      ::common::OperationResultMessage* response,
      ::google::protobuf::Closure* done)
      {
         mIndex.clear();
         mPages.clear();
      }

private:
   const Id insertIndexItem(const Id& parentId, const std::string& title, const bool isPage)
   {
      if(parentId == materia::Id::Invalid || isFolderExist(parentId))
      {
         std::string id = to_string(generator());

         journal::IndexItem newItem;
         newItem.mutable_journalitem()->mutable_id()->set_guid(id);
         newItem.mutable_journalitem()->mutable_folderid()->set_guid(parentId);
         newItem.mutable_journalitem()->set_title(title);
         newItem.set_modifiedtimestamp( std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) );
         newItem.set_ispage(isPage);

         mIndex.insert(newItem);

         return Id(id);   
      }

      return Id::Invalid;
   }

   bool isFolderExist(const materia::Id& id)
   {
      auto iter = mIndex.find(id);

      return iter != mIndex.end() && !iter->ispage();
   }

   materia::MateriaClient mClient;
   RemoteCollection<journal::IndexItem> mIndex;
   RemoteCollection<journal::Page> mPages;
};

}

int main(int argc, char *argv[])
{
   materia::JournalServiceImpl serviceImpl;
   materia::InterprocessService<materia::JournalServiceImpl> service(serviceImpl);
   
   service.provideAt("*:" + gJournalPort, "JournalService");
   
   return 0;
}