#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"
#include "Common/Utils.hpp"

using namespace materia;

class IdeasTest
{
public:
   IdeasTest() 
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});

      createIdea("Idea0", "Content", {}, {});
      createIdea("Idea1", "Content", {}, {});
      createIdea("Idea2", "Content", {}, {});
      createIdea("Idea3", "Content", {}, {});
      createIdea("Idea4", "Content", {}, {});
      createIdea("Idea5", "Content", {}, {});
      createIdea("Idea6", "Content", {}, {});
      createIdea("Idea7", "Content", {}, {});
      createIdea("Idea8", "Content", {}, {});
      createIdea("Idea9", "Content", {}, {});

      for(int i = 0; i < 20; ++i)
      {
         createIdea("Placeholder" + std::to_string(i), "Content", {}, {});
      }
      

      auto c1 = createIdea("Red", "Red is a great color", {"#colors"}, {});
      auto c2 = createIdea("Green", "Green is a great color", {"#colors"}, {});
      auto c3 = createIdea("Blue", "Blue is a great color", {"#colors"}, {});
      auto c4 = createIdea("Purple", "Purple is a great color", {"#colors"}, {});
      auto c5 = createIdea("White", "White is a great color", {"#colors"}, {});

      createIdea("Colors", "There are different colors", {"#colors"}, {c1, c2, c3, c4, c5});
      createIdea("Printer", "Printer is a device to print on paper, it uses different colors", {"#colors", "#devices", "#computers"}, {});
      createIdea("Sprinter", "Sprinter is a sports runner", {"#sports", "#human"}, {});
      createIdea("Mouse", "Mouse is either an animal or a pointer device", {"#animals", "#devices", "#computers"}, {});
      createIdea("Car", "Car is a device to drive", {"#devices", "#roads"}, {});
      createIdea("Elephant", "Elephant is a big animal", {"#animals"}, {});
      createIdea("Resource", "Resource is something you do not have enough :)", {"#managing"}, {});
   }

protected:
   std::string createIdea(
      const std::string& title, 
      const std::string& content, 
      const std::vector<std::string>& hashtags, 
      const std::vector<std::string>& linkedIdeas 
      )
   {
      boost::property_tree::ptree create;
      create.put("operation", "create");
      create.put("typename", "idea");
      create.put("params.title", title);
      create.put("params.content", content);
      putArray(create, "params.hashtags", hashtags);
      putArray(create, "params.links", linkedIdeas);

      auto r = mCore->executeCommandJson(writeJson(create));
      auto t = readJson<boost::property_tree::ptree>(r);
      return t.get<std::string>("result_id");  
   }

   std::vector<std::string> getRelatedIdeas(const std::string& ideaTitle)
   {
      std::vector<std::string> result;

      auto idea = readJson<boost::property_tree::ptree>(queryCondition("idea", ".title = \"" + ideaTitle + "\"", *mCore));
      auto id = idea.get_child("object_list").begin()->second.get<std::string>("id");

      boost::property_tree::ptree query;
      query.put("operation", "getRelatedIdeas");
      query.put("id", id);

      auto ol = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(query)));

      for(auto& v : ol.get_child("object_list"))
      {
         result.push_back(v.second.get<std::string>("title"));
      }

      return result;
   }

   std::vector<std::string> search(const std::string& keyword)
   {
      std::vector<std::string> result;

      boost::property_tree::ptree query;
      query.put("operation", "searchIdeas");
      query.put("searchString", keyword);

      auto ol = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(query)));

      for(auto& v : ol.get_child("object_list"))
      {
         result.push_back(v.second.get<std::string>("title"));
      }

      return result;
   }

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( GetRelatedIdeas_LinkedMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = getRelatedIdeas("Colors"); 

   BOOST_CHECK(contains(ideas, "Red"));
   BOOST_CHECK(contains(ideas, "Green"));
   BOOST_CHECK(contains(ideas, "Blue"));
   BOOST_CHECK(contains(ideas, "Purple"));
   BOOST_CHECK(contains(ideas, "White"));
}

BOOST_FIXTURE_TEST_CASE( GetRelatedIdeas_HashTagMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = getRelatedIdeas("Mouse"); 

   BOOST_CHECK(contains(ideas, "Printer"));
   BOOST_CHECK(contains(ideas, "Car"));
   BOOST_CHECK(contains(ideas, "Elephant"));
}

BOOST_FIXTURE_TEST_CASE( GetRelatedIdeas_ContentInTitlesMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = getRelatedIdeas("Printer"); 
   BOOST_CHECK(contains(ideas, "Colors"));
}

BOOST_FIXTURE_TEST_CASE( GetRelatedIdeas_TitleInContentsMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = getRelatedIdeas("Colors"); 
   BOOST_CHECK(contains(ideas, "Printer"));
}

BOOST_FIXTURE_TEST_CASE( Search_NoMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = search("ghdhdgdg"); 
   //std::cout << ideas[0];
   BOOST_CHECK(ideas.empty());
}

BOOST_FIXTURE_TEST_CASE( Search_TitleMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = search("prin"); 
   BOOST_CHECK(contains(ideas, "Printer"));
   BOOST_CHECK(contains(ideas, "Sprinter"));
}

BOOST_FIXTURE_TEST_CASE( Search_HashTagMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = search("man");
   BOOST_CHECK(contains(ideas, "Resource"));
   BOOST_CHECK(contains(ideas, "Sprinter"));
}

BOOST_FIXTURE_TEST_CASE( Search_ContentMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = search("Mouse is either an animal or a pointer device");
   BOOST_CHECK(ideas[0] == "Mouse");
}

BOOST_FIXTURE_TEST_CASE( Search_HashTagMatchTwoWords, IdeasTest ) 
{
   std::vector<std::string> ideas = search("#devices #computers");
   BOOST_CHECK(ideas[0] == "Printer");
   BOOST_CHECK(ideas[1] == "Mouse");
   BOOST_CHECK(ideas[2] == "Car");
}

BOOST_FIXTURE_TEST_CASE( Search_ComboMatch, IdeasTest ) 
{
   std::vector<std::string> ideas = search("COLORS");
   BOOST_CHECK(ideas[0] == "Colors");
   BOOST_CHECK(contains(ideas, "Printer"));
   BOOST_CHECK(contains(ideas, "Red"));
   BOOST_CHECK(contains(ideas, "Green"));
   BOOST_CHECK(contains(ideas, "Blue"));
   BOOST_CHECK(contains(ideas, "Purple"));
   BOOST_CHECK(contains(ideas, "White"));
}