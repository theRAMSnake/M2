#pragma once

#include <messages/journal.pb.h>
#include <Core/IJournal.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

JournalItem fromProto(const journal::JournalItem& x)
{
   return {fromProto(x.id()), fromProto(x.folderid()), x.title()};
}

JournalPage fromProto(const journal::Page& x)
{
   JournalPage p;
   static_cast<JournalItem&>(p) = fromProto(x.journalitem());
   p.content = x.content();
   return p;
}

journal::JournalItem toProto(const JournalItem& x)
{
   journal::JournalItem result;

   result.mutable_id()->CopyFrom(toProto(x.id));
   result.mutable_folderid()->CopyFrom(toProto(x.parentFolderId));
   result.set_title(x.title);

   return result;
}

journal::IndexItem toProto(const IndexItem& x)
{
   journal::IndexItem result;

   result.mutable_journalitem()->CopyFrom(toProto(static_cast<const JournalItem&>(x)));
   result.set_modifiedtimestamp(x.modified);
   result.set_ispage(x.isPage);

   return result;
}

journal::Page toProto(const JournalPage& x)
{
   journal::Page result;

   result.mutable_journalitem()->CopyFrom(toProto(static_cast<const JournalItem&>(x)));
   result.set_content(x.content);

   return result;
}

class JournalServiceImpl : public journal::JournalService
{
public:
   JournalServiceImpl(ICore& core)
   : mJournal(core.getJournal())
   {
   }

   virtual void InsertFolder(::google::protobuf::RpcController* controller,
                       const ::journal::InsertFolderParams* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mJournal.insertFolder(fromProto(request->parent_folder_id()), request->title())));
   }

   virtual void InsertPage(::google::protobuf::RpcController* controller,
                       const ::journal::InsertPageParams* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      response->CopyFrom(toProto(mJournal.insertPage(fromProto(request->parent_folder_id()), request->title(), request->content())));
   }

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mJournal.deleteItem(fromProto(*request));
      response->set_success(true);
   }

   virtual void UpdateFolder(::google::protobuf::RpcController* controller,
                       const ::journal::JournalItem* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mJournal.updateFolder(fromProto(*request));
      response->set_success(true);
   }

   virtual void UpdatePage(::google::protobuf::RpcController* controller,
                       const ::journal::Page* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mJournal.updateFolder(fromProto(*request));
      response->set_success(true);
   }

   virtual void GetIndex(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::journal::Index* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mJournal.getIndex())
      {
         response->add_items()->CopyFrom(toProto(x));
      }
   }

   virtual void Search(::google::protobuf::RpcController* controller,
                       const ::common::StringMessage* request,
                       ::journal::SearchResult* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mJournal.search(request->content()))
      {
         response->add_pageid()->CopyFrom(toProto(x.pageId));
         response->add_position(x.position);
      }
   }

   virtual void GetPage(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::journal::Page* response,
                       ::google::protobuf::Closure* done)
   {
      auto p = mJournal.getPage(fromProto(*request));
      if(p)
      {
         response->CopyFrom(toProto(*p));
      }
   }

private:
   IJournal& mJournal;
};

}