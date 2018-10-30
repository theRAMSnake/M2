#include "Journal.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <chrono>
#include "Logger.hpp"

namespace materia
{

std::string toJson(const IndexItem& from)
{
   boost::property_tree::ptree pt;

   pt.put ("id", from.id.getGuid());
   pt.put ("parent_folder_id", from.parentFolderId.getGuid());
   pt.put ("title", from.title);
   pt.put ("modified", from.modified);
   pt.put ("isPage", from.isPage);

   std::ostringstream buf; 
   write_json (buf, pt, false);
   return buf.str();
}

IndexItem createIndexItemFromJson(const std::string& json)
{
   IndexItem result;

   boost::property_tree::ptree pt;
   std::istringstream is (json);
   read_json (is, pt);
   
   result.id = pt.get<std::string> ("id");
   result.parentFolderId = pt.get<std::string> ("parent_folder_id");
   result.title = pt.get<std::string> ("title");
   result.modified = pt.get<std::time_t> ("modified");
   result.isPage = pt.get<bool> ("isPage");

   return result;
}

Journal::Journal(Database& db)
: mIndexStorage(db.getTable("journal_index"))
, mContentStorage(db.getTable("journal_content"))
{
    LOG("Initializing journal");
    mIndexStorage->foreach([&](std::string id, std::string json) 
    {
        mIndex.insert({id, createIndexItemFromJson(json)});
    });

    mContentStorage->foreach([&](std::string id, std::string json) 
    {
        mPageContents.insert({id, json});
    });
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
   mContentStorage->store(id, content);

   return id;
}

void Journal::deleteItem(const Id& id)
{
    auto pos = mIndex.find(id);
    if(pos != mIndex.end())
    {
        mIndex.erase(pos);
        mIndexStorage->erase(id);

        if(pos->second.isPage)
        {
            mPageContents.erase(id);
            mContentStorage->erase(id);
        }
        else
        {
            std::vector<Id> children;
            for(auto x : mIndex)
            {
                if(x.second.parentFolderId == id)
                {
                    children.push_back(x.second.id);
                }
            }

            for(auto x : children)
            {
                deleteItem(x);
            }
        }
    }
}

void Journal::updateIndexItem(const JournalItem& item)
{
   auto pos = mIndex.find(item.id);
   if(pos != mIndex.end())
   {
       auto& x = pos->second;
       x.parentFolderId = item.parentFolderId;
       x.title = item.title;
       x.modified = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

       mIndexStorage->store(x.id, toJson(x));
   }
}

void Journal::updateFolder(const JournalItem& item)
{
    if(item.parentFolderId != item.id)
    {
        updateIndexItem(item);
    }
}

void Journal::updatePage(const JournalPage& item)
{
   auto pos = mPageContents.find(item.id);
   if(pos != mPageContents.end())
   {
       updateIndexItem(item);

       pos->second = item.content;
       mContentStorage->store(item.id, item.content);
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

    for(auto& x : mPageContents)
    {
        auto pos = x.second.find(keyword);
        while(pos != std::string::npos)
        {
            result.push_back({x.first, pos});

            pos = x.second.find(keyword, pos + 1);
        }
    }

    return result;
}

std::optional<JournalPage> Journal::getPage(const Id& id)
{
    std::optional<JournalPage> result;

    auto pos = mIndex.find(id);
    if(pos != mIndex.end())
    {
        if(pos->second.isPage)
        {
            result = JournalPage{};
            static_cast<JournalItem&>(*result) = pos->second;
            result->content = mPageContents[id];
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

        mIndex.insert({newItem.id, newItem});
        mIndexStorage->store(newItem.id, toJson(newItem));

        return newItem.id;   
    }

    return Id::Invalid;
}

bool Journal::isFolderExist(const Id& id)
{
    auto iter = mIndex.find(id);

    return iter != mIndex.end() && !iter->second.isPage;
}

bool IndexItem::operator == (const IndexItem& other) const
{
   return id == other.id && parentFolderId == other.parentFolderId && modified == other.modified && title == other.title;
}

}