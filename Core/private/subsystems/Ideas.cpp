#include "Ideas.hpp"
#include <Common/Utils.hpp>
#include "../ObjectManager.hpp"
#include "../ExceptionsUtil.hpp"
#include <boost/algorithm/string.hpp>

namespace materia
{

constexpr std::size_t LIMIT = 50;

IdeasSS::IdeasSS(ObjectManager& objMan)
: mOm(objMan)
{

}

void IdeasSS::onNewDay(const boost::gregorian::date& date)
{

}

void IdeasSS::onNewWeek()
{
   
}

std::vector<TypeDef> IdeasSS::getTypes()
{
    std::vector<TypeDef> result;

    result.push_back({"idea", "ideas", {
        {"content", Type::String},
        {"title", Type::String},
        {"hashtags", Type::StringArray},
        {"links", Type::StringArray}
        }});

    return result;
}

std::vector<CommandDef> IdeasSS::getCommandDefs()
{
    return {{"getRelatedIdeas", std::bind(&IdeasSS::parseGetRelatedIdeas, this, std::placeholders::_1)},
        {"searchIdeas", std::bind(&IdeasSS::parseSearchIdeas, this, std::placeholders::_1)}};
}

struct keyOnlyCompare
{
   constexpr bool operator()(const std::pair<int, Object>& a, const std::pair<int, Object>& b) const
   {
      return a.first < b.first;
   }
};

class GetRelatedIdeasCommand : public Command
{
public:
   GetRelatedIdeasCommand(const Id& id)
   : mId(id)
   {

   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      std::map<Id, std::optional<Object>> result;
      auto idea = objManager.get(mId);

      //Get all linked ideas
      for(auto& linkedId : idea["links"].get<Type::StringArray>())
      {
         try
         {
            result[Id(linkedId)] = objManager.get(linkedId);
         }
         catch(...)
         {
            //The idea was deleted
         }
      }

      auto allIdeas = objManager.getAll("idea");
      erase_if(allIdeas, [this](auto x){return x.getId() == mId;});

      //If < limit: get best hashtag matched ones
      if(result.size() <= LIMIT)
      {
         auto searchResult = hashTagSearch(toSet(idea["hashtags"].get<Type::StringArray>()), allIdeas);

         for(std::size_t i = 0; i < searchResult.size(); ++i)
         {
            result[searchResult[i].getId()] = searchResult[i];
            if(result.size() >= LIMIT)
            {
               break;
            }
         }
      }

      //If < limit: search content in other titles
      if(result.size() <= LIMIT)
      {
         auto searchResult = contentInTitleSearch(idea["content"].get<Type::String>(), allIdeas);
         for(std::size_t i = 0; i < searchResult.size(); ++i)
         {
            result[searchResult[i].getId()] = searchResult[i];
            if(result.size() >= LIMIT)
            {
               break;
            }
         }
      }

      //If < limit: search title in other contents
      if(result.size() <= LIMIT)
      {
         auto searchResult = titleInContentSearch(idea["title"].get<Type::String>(), allIdeas);
         for(std::size_t i = 0; i < searchResult.size(); ++i)
         {
            result[searchResult[i].getId()] = searchResult[i];
            if(result.size() >= LIMIT)
            {
               break;
            }
         }
      }

      std::vector<Object> ideasOnly;

      for(auto& kv : result)
      {
         ideasOnly.push_back(*kv.second);
      }

      return ideasOnly;
   }

private:

   std::vector<Object> hashTagSearch(const std::set<std::string>& hashTags, const std::vector<Object>& ideas)
   {
      std::multiset<std::pair<int, Object>, keyOnlyCompare> rankedIdeas;

      for(auto& i : ideas)
      {
         auto otherHashTags = toSet(i["hashtags"].get<Type::StringArray>());
         auto numIdentical = calcNumIdentical(hashTags, otherHashTags);
         if(numIdentical > 0)
         {
            rankedIdeas.insert({numIdentical, i});
         }
      }

      std::vector<Object> result;
      std::transform(rankedIdeas.begin(), rankedIdeas.end(), std::back_inserter(result), [](auto x){return x.second;});
      std::reverse(result.begin(), result.end());

      return result;
   }

   std::vector<Object> contentInTitleSearch(const std::string& content, const std::vector<Object>& ideas)
   {
      std::vector<Object> result;

      for(auto& i : ideas)
      {
         if(content.find(i["title"].get<Type::String>()) != std::string::npos)
         {
            result.push_back(i);
         }
      }

      return result;
   }

   std::vector<Object> titleInContentSearch(const std::string& title, const std::vector<Object>& ideas)
   {
      std::vector<Object> result;

      for(auto& i : ideas)
      {
         if(i["content"].get<Type::String>().find(title) != std::string::npos)
         {
            result.push_back(i);
         }
      }

      return result;
   }

   const Id mId;
};

class SearchIdeasCommand : public Command
{
public:
   SearchIdeasCommand(const std::string& searchString)
   : mSearchString(searchString)
   { 
      boost::split(mIndividualKeyWords, mSearchString, boost::is_any_of(" "));
      for(auto& x : mIndividualKeyWords)
      {
         boost::algorithm::to_lower(x);
      }
   }

   ExecutionResult execute(ObjectManager& objManager) override
   {
      std::multiset<std::pair<int, Object>, keyOnlyCompare> rankedIdeas;

      for(auto& o : objManager.getAll("idea"))
      {
         int rank = rankIdea(o);
         if(rank > 0)
         {
            if(rankedIdeas.size() <= LIMIT)
            {
               rankedIdeas.insert({rank, o});
            }
            else if(rankedIdeas.begin()->first < rank)
            {
               rankedIdeas.erase(rankedIdeas.begin());
               rankedIdeas.insert({rank, o});
            }
         }
      }

      std::vector<Object> result;
      std::transform(rankedIdeas.begin(), rankedIdeas.end(), std::back_inserter(result), [](auto x){return x.second;});
      std::reverse(result.begin(), result.end());

      return result;
   }

private:

   int rankIdea(const Object& idea)
   {
      int result = 0;

      auto ideaHashes = idea["hashtags"].get<Type::StringArray>();
      for(auto& x : ideaHashes)
      {
         boost::algorithm::to_lower(x);
      }

      for(auto& i : mIndividualKeyWords)
      {
         if(!i.empty())
         {
            if(i[0] == '#')
            {
               if(contains(ideaHashes, i))
               {
                  result += 100;
               }
            }
            else
            {
               if(contains(ideaHashes, "#" + i))
               {
                  result += 100;
               }

               auto title = idea["title"].get<Type::String>();
               boost::algorithm::to_lower(title);
               if(title.find(i) != std::string::npos)
               {
                  result += 50;
               }

               for(auto x : ideaHashes)
               {
                  if(x.find(i) != std::string::npos)
                  {
                     result += 35;
                     break;
                  }
               }

               auto content = idea["content"].get<Type::String>();
               boost::algorithm::to_lower(content);
               if(content.find(i) != std::string::npos)
               {
                  result += 25;
               }
            }
         }
      }

      return result;
   }

   std::vector<std::string> mIndividualKeyWords;
   const std::string mSearchString;
};

Command* IdeasSS::parseGetRelatedIdeas(const boost::property_tree::ptree& src)
{
   auto id = getOrThrow<Id>(src, "id", "Id is not specified");

   return new GetRelatedIdeasCommand(id);
}

Command* IdeasSS::parseSearchIdeas(const boost::property_tree::ptree& src)
{
   auto searchString = getOrThrow<std::string>(src, "searchString", "searchString is not specified");

   return new SearchIdeasCommand(searchString);
}

}