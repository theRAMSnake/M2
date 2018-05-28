#include "Journal.hpp"
#include "ProtoConverter.hpp"

namespace materia
{

Journal::Journal(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

Id Journal::insertFolder(const Id& parentFolderId, const std::string& name)
{
   journal::InsertFolderParams params;
   params.mutable_parent_folder_id()->CopyFrom(toProto(parentFolderId));
   params.set_title(name);

   common::UniqueId id;
   mProxy.getService().InsertFolder(nullptr, &params, &id, nullptr);

   return fromProto(id);
}

Id Journal::insertPage(const Id& parentFolderId, const std::string& name, const std::string& content)
{
   journal::InsertPageParams params;
   params.mutable_parent_folder_id()->CopyFrom(toProto(parentFolderId));
   params.set_title(name);
   params.set_content(content);

   common::UniqueId id;
   mProxy.getService().InsertPage(nullptr, &params, &id, nullptr);

   return fromProto(id);
}

bool Journal::deleteItem(const Id& id)
{
   common::UniqueId protoid = toProto(id);  
   common::OperationResultMessage result;

   mProxy.getService().DeleteItem(nullptr, &protoid, &result, nullptr);
   return result.success();
}

journal::JournalItem toProto(const materia::JournalItem& x)
{
   journal::JournalItem result;

   result.set_title(x.title);
   result.mutable_folderid()->CopyFrom(toProto(x.parentFolderId));
   result.mutable_id()->CopyFrom(toProto(x.id));

   return result;
}

bool Journal::updateFolder(const JournalItem& item)
{
   auto protoItem = toProto(item);
   common::OperationResultMessage result;
   
   mProxy.getService().UpdateFolder(nullptr, &protoItem, &result, nullptr);
   return result.success();
}

journal::Page toProto(const JournalPage& item)
{
   journal::Page result;

   auto base = toProto(static_cast<const JournalItem&>(item));

   result.mutable_journalitem()->CopyFrom(base);
   result.set_content(item.content);

   return result;
}

bool Journal::updatePage(const JournalPage& item)
{
   auto protoItem = toProto(item);
   common::OperationResultMessage result;
   
   mProxy.getService().UpdatePage(nullptr, &protoItem, &result, nullptr);
   return result.success();
}

IndexItem fromProto(const journal::IndexItem& src)
{
   IndexItem result;

   result.id = fromProto(src.journalitem().id());
   result.parentFolderId = fromProto(src.journalitem().folderid());
   result.title = src.journalitem().title();
   result.modified = src.modifiedtimestamp();
   result.isPage = src.ispage();

   return result;
}

std::vector<IndexItem> Journal::getIndex()
{
   common::EmptyMessage r;

   journal::Index items;

   mProxy.getService().GetIndex(nullptr, &r, &items, nullptr);

   std::vector<IndexItem> result(items.items_size());
   std::transform(items.items().begin(), items.items().end(), result.begin(), [] (auto x)-> auto { return fromProto(x); });

   return result; 
}

std::vector<SearchResult> Journal::search(const std::string& keyword)
{
   common::StringMessage r;
   r.set_content(keyword);

   journal::SearchResult items;
   mProxy.getService().Search(nullptr, &r, &items, nullptr);

   std::vector<SearchResult> result(items.pageid_size());

   if(items.pageid_size() == items.position_size())
   {
      for(std::size_t i = 0; i < result.size(); ++i)
      {
         auto& item = result[i];

         item.pageId = fromProto(items.pageid()[i]);
         item.position = items.position()[i];
      }
   }

   return result; 
}

JournalPage fromProto(const journal::Page& src)
{
   JournalPage result;

   result.id = fromProto(src.journalitem().id());
   result.parentFolderId = fromProto(src.journalitem().folderid());
   result.title = src.journalitem().title();
   result.content = src.content();

   return result;
}

std::optional<JournalPage> Journal::getPage(const Id& id)
{
   auto protoId = toProto(id);
   journal::Page out;

   mProxy.getService().GetPage(nullptr, &protoId, &out, nullptr);

   if(fromProto(out.journalitem().id()) != Id::Invalid)
   {
      return fromProto(out);
   }

   return std::optional<JournalPage>();
}

bool IndexItem::operator == (const IndexItem& other) const
{
   return id == other.id && parentFolderId == other.parentFolderId && modified == other.modified && title == other.title;
}

void Journal::clear()
{
   common::EmptyMessage r;
   common::OperationResultMessage responce;

   mProxy.getService().Clear(nullptr, &r, &responce, nullptr);
}

}