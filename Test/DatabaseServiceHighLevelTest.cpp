#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <Client/MateriaClient.hpp>

#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include "TestHelpers.hpp"

class DatabaseTest
{
public:
   DatabaseTest()
   : mClient("test")
   , mService(mClient.getDatabase())
   {
      mongocxx::instance instance{}; 
      mongocxx::client client{mongocxx::uri{}};

      client["materia"].drop();

      mService.setCategory("somecat");

      for(int i = 0; i < 10; ++i)
      {
         std::string body = "{\"doc\":" + boost::lexical_cast<std::string>(i) + ", \"someval\":"
            + boost::lexical_cast<std::string>(i) + "}";

         mSampleIds.push_back(mService.insertDocument({materia::Id::Invalid, body}, materia::IdMode::Generate));
      }
   }

protected:

   void addTestDoc(const std::string& category, const std::string& body)
   {
      mService.setCategory(category);
      mService.insertDocument({materia::Id::Invalid, body}, materia::IdMode::Generate);
   }

   materia::MateriaClient mClient;
   materia::Database& mService;
   std::vector<materia::Id> mSampleIds;
};

bool spacelessEquals(const std::string& a, const std::string& b)
{
   auto a_ = a;
   auto b_ = b;
   a_.erase( std::remove_if( a_.begin(), a_.end(), ::isspace ), a_.end() );
   b_.erase( std::remove_if( b_.begin(), b_.end(), ::isspace ), b_.end() );

   return a_ == b_;
}

BOOST_FIXTURE_TEST_CASE( Add, DatabaseTest )  
{
   //invalid doc creation
   BOOST_CHECK_EQUAL(mService.insertDocument({materia::Id::Invalid, "invalid body"}, materia::IdMode::Generate), materia::Id::Invalid);

   //valid doc creation
   auto id = materia::Id("key");
   auto body = "{\"somevalue\":5}";
   BOOST_CHECK_EQUAL(mService.insertDocument({id, body}, materia::IdMode::Provided), id);

   auto id2 = materia::Id("key2");
   BOOST_CHECK_NE(mService.insertDocument({id2, body}, materia::IdMode::Generate), id2);


   //check if we can get it
   auto doc = mService.getDocument(id);
   BOOST_CHECK(doc);
   BOOST_CHECK_EQUAL(doc->id, id);

   

   BOOST_CHECK(spacelessEquals(doc->body, body));
}

BOOST_FIXTURE_TEST_CASE( Delete, DatabaseTest )  
{
   //delete unexist doc
   BOOST_CHECK(!mService.deleteDocument(materia::Id("dfsdgfdfg")));
   BOOST_CHECK_EQUAL(mSampleIds.size(), mService.getDocuments().size());

   //delete exist doc
   {
      const int sampleDocIndex = 2;
      BOOST_CHECK(mService.deleteDocument(mSampleIds[sampleDocIndex]));

      //check that all, but deleted documents are there
      auto docs = mService.getDocuments();
      BOOST_CHECK_EQUAL(mSampleIds.size() - 1, mService.getDocuments().size());

      BOOST_CHECK(std::find_if(docs.begin(), docs.end(), [&] (auto d)->bool {return d.id == mSampleIds[sampleDocIndex];})
         == docs.end());
   }

   //delete all
   for(auto x : mSampleIds)
   {
      mService.deleteDocument(x);
   }

   //nothing can be obtained anymore
   BOOST_CHECK(mService.getDocuments().empty());
}

BOOST_FIXTURE_TEST_CASE( Get, DatabaseTest )
{
   //get exist
   for(auto x : mSampleIds)
   {
      BOOST_CHECK(mService.getDocument(x));
   }

   //get non exist
   BOOST_CHECK(!mService.getDocument(materia::Id("jdkflsdf")));
}

std::string remove_spaces(std::string src)
{
   src.erase(std::remove_if(src.begin(), src.end(), ::isspace), src.end());
   return src;
}

BOOST_FIXTURE_TEST_CASE( Edit, DatabaseTest )
{
   //edit non exist
   {
      BOOST_CHECK(!mService.replaceDocument({materia::Id("dgkljsdg"), "{\"doc\":5"}));
   }

   //edit invalid json
   {
      BOOST_CHECK(!mService.replaceDocument({mSampleIds[0], "{\"doc\"5"}));
   }

   //edit valid
   {
      BOOST_CHECK(mService.replaceDocument({mSampleIds[0], "{\"doc\":100}"}));
   }

   //check validness of documents
   {
      for(std::size_t i = 0; i < mSampleIds.size(); ++i)
      {
         auto doc = mService.getDocument(mSampleIds[i]);

         BOOST_CHECK_EQUAL(doc->id, mSampleIds[i]);

         if(i != 0)
         {
            BOOST_CHECK_EQUAL("{\"doc\":" + boost::lexical_cast<std::string>(i) + ",\"someval\":"
            + boost::lexical_cast<std::string>(i) +
             "}",
             remove_spaces(doc->body));
         }
         else
         {
            BOOST_CHECK_EQUAL("{\"doc\":100}", remove_spaces(doc->body));
         }
      }
   }
}

BOOST_FIXTURE_TEST_CASE( Find_Category, DatabaseTest )
{
   {
      BOOST_CHECK_EQUAL(mSampleIds.size(), mService.getDocuments().size());
   }
   {
      mService.setCategory("somecat1");
      BOOST_CHECK_EQUAL(0, mService.getDocuments().size());
   }
} 

BOOST_FIXTURE_TEST_CASE( Find_Text, DatabaseTest )
{
   {
      auto result = mService.queryDocuments({{"doc", "3", "", materia::QueryElementType::Equals}});
      
      BOOST_CHECK_EQUAL(result.size(), 1);
      BOOST_CHECK_EQUAL("{\"doc\":3,\"someval\":3}", remove_spaces(result[0].body));
   }
   {
      auto result = mService.queryDocuments({{"error", "", "", materia::QueryElementType::Equals}});
      BOOST_CHECK_EQUAL(result.size(), 0);
   }
   {
      auto result = mService.queryDocuments({{"doc", "11", "", materia::QueryElementType::Equals}});
      BOOST_CHECK_EQUAL(result.size(), 0);
   }
   {
      auto result = mService.queryDocuments({
         {"doc", "4", "", materia::QueryElementType::Equals},
         {"someval", "4", "", materia::QueryElementType::Equals}
         });
      
      BOOST_CHECK_EQUAL(result.size(), 1);
      BOOST_CHECK_EQUAL("{\"doc\":4,\"someval\":4}", remove_spaces(result[0].body));
   }
   {
      auto result = mService.queryDocuments({{"someval", "3", "", materia::QueryElementType::Equals}});
      
      BOOST_CHECK_EQUAL(result.size(), 1);
      BOOST_CHECK_EQUAL("{\"doc\":3,\"someval\":3}", remove_spaces(result[0].body));
   }
} 

BOOST_FIXTURE_TEST_CASE( Query_Less, DatabaseTest )
{
   {
      auto result = mService.queryDocuments({{"someval", "5", "", materia::QueryElementType::Less}});
      
      BOOST_CHECK_EQUAL(result.size(), 5);
   }
   {
      auto result = mService.queryDocuments({{"someval", "1", "", materia::QueryElementType::Less}});
      
      BOOST_CHECK_EQUAL(result.size(), 1);
   }
}

BOOST_FIXTURE_TEST_CASE( Query_Greater, DatabaseTest )
{
   {
      auto result = mService.queryDocuments({{"someval", "1", "", materia::QueryElementType::Greater}});
      
      BOOST_CHECK_EQUAL(result.size(), 8);
   }
   {
      auto result = mService.queryDocuments({{"someval", "5", "", materia::QueryElementType::Greater}});
      
      BOOST_CHECK_EQUAL(result.size(), 4);
   }
}

BOOST_FIXTURE_TEST_CASE( Query_Between, DatabaseTest )
{
   {
      auto result = mService.queryDocuments({{"someval", "1", "5", materia::QueryElementType::Between}});
      
      BOOST_CHECK_EQUAL(result.size(), 3);
   }
   {
      auto result = mService.queryDocuments({{"someval", "5", "1", materia::QueryElementType::Between}});
      
      BOOST_CHECK_EQUAL(result.size(), 0);
   }
}

BOOST_FIXTURE_TEST_CASE( Fetch, DatabaseTest )
{
   mService.setCategory("someothercat");
   mSampleIds.push_back(mService.insertDocument({materia::Id("key"), "{\"somevalue\":5}"}, materia::IdMode::Generate));

   {
      auto docs = mService.fetch();
      BOOST_CHECK_EQUAL(mSampleIds.size(), docs.size());

      for(auto x : docs)
      {
         bool keyFound = std::find_if(mSampleIds .begin(), mSampleIds.end(), [&](auto y)->bool{return y == x.id;})
            != mSampleIds.end();

         BOOST_CHECK(keyFound);
      }
   }
}

/*BOOST_FIXTURE_TEST_CASE( FullTextSearch, DatabaseTest )
{
   addTestDoc("animals", "{\"name\":\"snake\", \"type\":\"animal\"}");
   addTestDoc("animals", "{\"name\":\"elephant\", \"type\":\"animal\"}");
   addTestDoc("animals", "{\"name\":\"chicken\", \"type\":\"animal\"}");
   addTestDoc("food", "{\"name\":\"apple\", \"type\":\"food\"}");
   addTestDoc("food", "{\"name\":\"fries\", \"type\":\"food\"}");
   addTestDoc("food", "{\"name\":\"chicken\", \"type\":\"food\"}");

   {
      auto result = mService.fts("food");

      BOOST_CHECK_EQUAL(result.size(), 3);
      for(auto x : result)
      {
         BOOST_CHECK(x.body.find("food") != std::string::npos);
      }
   }
   {
      auto result = mService.fts("name");

      BOOST_CHECK_EQUAL(result.size(), 6);
   }
   {
      auto result = mService.fts("chicken");

      BOOST_CHECK_EQUAL(result.size(), 2);
      for(auto x : result)
      {
         BOOST_CHECK(x.body.find("chicken") != std::string::npos);
      }
   }
   {
      auto result = mService.fts("bullshit");

      BOOST_CHECK_EQUAL(result.size(), 0);
   }
}*/