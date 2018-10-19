#include "Journal.hpp"

namespace materia
{

/*
journal::JournalItem toProto(const materia::JournalItem& x)
{
   journal::JournalItem result;

   result.set_title(x.title);
   result.mutable_folderid()->CopyFrom(toProto(x.parentFolderId));
   result.mutable_id()->CopyFrom(toProto(x.id));

   return result;
}

journal::Page toProto(const JournalPage& item)
{
   journal::Page result;

   auto base = toProto(static_cast<const JournalItem&>(item));

   result.mutable_journalitem()->CopyFrom(base);
   result.set_content(item.content);

   return result;
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

JournalPage fromProto(const journal::Page& src)
{
   JournalPage result;

   result.id = fromProto(src.journalitem().id());
   result.parentFolderId = fromProto(src.journalitem().folderid());
   result.title = src.journalitem().title();
   result.content = src.content();

   return result;
}
*/

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

Journal::Journal(Database& db)
: mDb(db)
{
    loadItems();
}

Id Journal::insertFolder(const Id& parentFolderId, const std::string& name)
{
   constexpr bool isPage = false;
   return insertIndexItem(parentFolderId, name, isPage);
}

Id Journal::insertPage(const Id& parentFolderId, const std::string& name, const std::string& content)
{
   constexpr bool isPage = true;
   auto id = insertIndexItem(parentFolderId, name, isPage);

   mPageContents.insert(std::make_pair(id, content));
   saveToDb();
}

void Journal::deleteItem(const Id& id)
{
    auto pos = find_by_id(mIndex, id);
    if(pos != mItems.end())
    {
        mIndex.erase(pos);
        eraseFromDb();

        if(pos->isPage)
        {
            mContents.erase(id);
            eraseFromDb();
        }
        else
        {
            std::vector<Id> children;
            for(auto x : mIndex)
            {
                if(x.parentFolderId == id)
                {
                    children.push_back(x.id);
                }
            }

            for(auto x : children)
            {
                deleteItem(x.id);
            }
        }
    }
}

void Journal::updateIndexItem(const JournalItem& item)
{
   auto pos = find_by_id(mIndex, item.id);
   if(pos != mIndex.end())
   {
       pos->parentFolderId = item.parentFolderId;
       pos->title = item.title;
       pos->modified = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
       saveToDb();
   }
}

void Journal::updateFolder(const JournalItem& item)
{
   updateIndexItem(item);
}

void Journal::updatePage(const JournalPage& item)
{
   auto pos = find_by_id(mContents, item.id);
   if(pos != mItems.end())
   {
       updateIndexItem(item);

       *pos = item.content;
       saveToDb();
   }
}

std::vector<IndexItem> Journal::getIndex()
{
   std::vector<IndexItem> result;

   for(auto x : mIndex)
   {
       result.push_back(x.second);
   }

   return result;
}

std::vector<SearchResult> Journal::search(const std::string& keyword)
{
    std::vector<SearchResult> result;

    for(auto x : mContents)
    {
        auto pos = x.second.find(keyword);
        while(pos != std::string::npos)
        {
            result.push_back({x.first, pos});

            pos = x.content().find(request->content(), pos + 1);
        }
    }

    return result;
}

std::optional<JournalPage> Journal::getPage(const Id& id)
{
    std::optional<JournalPage> result;

    auto pos = find_by_id(mIndex, item.id);
    if(pos == mIndex.end())
    {
        if(pos->isPage)
        {
            result.reset({});
            static_cast<JournalItem&>(*result) = *pos;
            result->content = mContens[item.id];
        }
    }

    return result;
}

const Id Journal::insertIndexItem(const Id& parentId, const std::string& title, const bool isPage)
{
    if(parentId == Id::Invalid || isFolderExist(parentId))
    {
        IndexItem newItem {};
        newItem.id = Id::generate();
        newItem.parentFolderId = parentId;
        newItem.title = title;
        newItem.modified = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        newItem.isPage = isPage;

        mIndex.insert(newItem);
        saveToDb();

        return newItem.id;   
    }

    return Id::Invalid;
}

bool Journal::isFolderExist(const Id& id)
{
    auto iter = mIndex.find(id);

    return iter != mIndex.end() && !iter->isPage;
}

bool IndexItem::operator == (const IndexItem& other) const
{
   return id == other.id && parentFolderId == other.parentFolderId && modified == other.modified && title == other.title;
}

}