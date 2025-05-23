#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <thread>
#include "../Core/private/JsonSerializer.hpp"
#include <Core/ICore3.hpp>
#include "Utils.hpp"

class NewAPITest
{
public:
   NewAPITest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
   }

protected:

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( TestInvalid, NewAPITest ) 
{  
    BOOST_CHECK(expectError(mCore->executeCommandJson("")));
    BOOST_CHECK(expectError(mCore->executeCommandJson("dsfhlgkldfhgksdfjhg")));
    BOOST_CHECK(expectError(mCore->executeCommandJson("{}")));

    BOOST_CHECK(expectError(mCore->executeCommandJson("{\"operation\":\"wrongOP\"}")));
}

BOOST_FIXTURE_TEST_CASE( TestCreate, NewAPITest ) 
{  
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "simple_list");
    create.put("params.value", "a");

    BOOST_CHECK(expectId(mCore->executeCommandJson(writeJson(create))));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(simple_list)");

    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    unsigned int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       (void)v;
       counter++;
    }

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestDelete, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");

    auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree destroy;
    destroy.put("operation", "destroy");
    destroy.put("id", r.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(destroy));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    
    query.put("filter", "IS(variable)");

    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    unsigned int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       (void)v;
       counter++;
    }

    BOOST_CHECK_EQUAL(0, counter);
}

BOOST_FIXTURE_TEST_CASE( TestModify, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");

    auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree obj;
    {
        boost::property_tree::ptree query;
        query.put("operation", "query");
        query.put("filter", "IS(variable)");

        auto result = mCore->executeCommandJson(writeJson(query));
        auto ol = readJson<boost::property_tree::ptree>(result);

        obj = ol.get_child("object_list").begin()->second;
    }

    boost::property_tree::ptree modify;
    modify.put("operation", "modify");
    modify.put_child("params", obj);
    modify.put("params.value", "b");
    modify.put("id", r.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(modify));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(variable)");

    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    for(auto& v : ol.get_child("object_list"))
    {
       BOOST_CHECK_EQUAL("b", v.second.get<std::string>("value"));
    }
}

BOOST_FIXTURE_TEST_CASE( TestQueryId, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");

    std::string r;
    for(std::size_t i = 0; i < 3; ++i)
    {
        r = mCore->executeCommandJson(writeJson(create));
    }

    auto tr = readJson<boost::property_tree::ptree>(r);

    boost::property_tree::ptree query;
    query.put("operation", "query");
    
    query.put("filter", "IS(variable) AND .id = \"" + tr.get<std::string>("result_id") + "\"");

    auto result = mCore->executeCommandJson(writeJson(query));
    

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    for(auto& v : ol.get_child("object_list"))
    {
       BOOST_CHECK_EQUAL(tr.get<std::string>("result_id"), v.second.get<std::string>("id"));
    }
}

BOOST_FIXTURE_TEST_CASE( TestQueryFilter, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");
    
    for(std::size_t i = 0; i < 5; ++i)
    {
        create.put("params.value", i);
        mCore->executeCommandJson(writeJson(create));
    }

    boost::property_tree::ptree create2;
    create2.put("operation", "create");
    create2.put("typename", "variable");
    mCore->executeCommandJson(writeJson(create2));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    
    query.put("filter", "IS(variable) AND .value < \"3\"");

    auto result = mCore->executeCommandJson(writeJson(query));
    auto ol = readJson<boost::property_tree::ptree>(result);
    
    for(auto& v : ol.get_child("object_list"))
    {
       BOOST_CHECK(v.second.get<int>("value") < 3);
    }
}

BOOST_FIXTURE_TEST_CASE( TestDescribe, NewAPITest ) 
{
    boost::property_tree::ptree d;
    d.put("operation", "describe");
    mCore->executeCommandJson(writeJson(d));
}

BOOST_FIXTURE_TEST_CASE( TestCompletable, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "calendar_item");
    create.put("defined_id", "id");
    create.put("params.text", "a");
    create.put("params.timestamp", 10);
    create.put("params.recurrency", "7d");

    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree complete;
    complete.put("operation", "complete");
    complete.put("id", tr.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(complete));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(calendar_item)");
    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       auto ts = v.second.get<std::time_t>("timestamp");
       BOOST_CHECK_EQUAL(604810, ts);
       counter++;
    }

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestCompletable2, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "calendar_item");
    create.put("defined_id", "id");
    create.put("params.text", "a");
    create.put("params.timestamp", 10);
    create.put("params.recurrency", "None");

    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree complete;
    complete.put("operation", "complete");
    complete.put("id", tr.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(complete));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(calendar_item)");
    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
        (void)v;
        counter++;
    }
    BOOST_CHECK_EQUAL(0, counter);
}

BOOST_FIXTURE_TEST_CASE( TestCompletable3, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "calendar_item");
    create.put("defined_id", "id");
    create.put("params.text", "a");
    create.put("params.timestamp", 10);
    create.put("params.recurrency", "7m");

    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree complete;
    complete.put("operation", "complete");
    complete.put("id", tr.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(complete));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(calendar_item)");
    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       auto ts = v.second.get<std::time_t>("timestamp");
       BOOST_CHECK_EQUAL(18316810, ts);
       counter++;
    }

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestCompletable4, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "calendar_item");
    create.put("defined_id", "id");
    create.put("params.text", "a");
    create.put("params.timestamp", 10);
    create.put("params.recurrency", "7y");

    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree complete;
    complete.put("operation", "complete");
    complete.put("id", tr.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(complete));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(calendar_item)");
    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       auto ts = v.second.get<std::time_t>("timestamp");
       BOOST_CHECK_EQUAL(220924810, ts);
       counter++;
    }

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestCompletable5, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "calendar_item");
    create.put("defined_id", "id");
    create.put("params.text", "a");
    create.put("params.timestamp", 10);
    create.put("params.recurrency", "1m7d");

    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree complete;
    complete.put("operation", "complete");
    complete.put("id", tr.get<std::string>("result_id"));

    mCore->executeCommandJson(writeJson(complete));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(calendar_item)");
    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       auto ts = v.second.get<std::time_t>("timestamp");
       BOOST_CHECK_EQUAL(3283210, ts);
       counter++;
    }

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestJournalIndexUpdate, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "journal_header");
    create.put("defined_id", "id");

    mCore->executeCommandJson(writeJson(create));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(journal_header)");
    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(query)));

    boost::property_tree::ptree createPage;
    createPage.put("operation", "create");
    createPage.put("typename", "journal_content");
    createPage.put("defined_id", "page_id");
    createPage.put("params.content", "sdfgsdfg");
    mCore->executeCommandJson(writeJson(createPage));
    
    createConnection(*mCore, "id", "page_id", "Extension");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    boost::property_tree::ptree modifyPage;
    modifyPage.put("operation", "modify");
    modifyPage.put("id", "page_id");
    modifyPage.put("params.content", "sdghdfkhgsdfkg");
    mCore->executeCommandJson(writeJson(modifyPage));

    auto r = mCore->executeCommandJson(writeJson(query));

    int oldTs = 555;
    tr = readJson<boost::property_tree::ptree>(r);
    for(auto& v : tr.get_child("object_list"))
    {
        std::cout << v.second.get<int>("modified");
        BOOST_CHECK(oldTs != v.second.get<int>("modified"));
        oldTs = v.second.get<int>("modified");
    }

}

BOOST_FIXTURE_TEST_CASE( TestJournalIndexDelete, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "journal_header");
    create.put("defined_id", "id");
    create.put("params.isPage", false);
    create.put("params.modified", 555);

    mCore->executeCommandJson(writeJson(create));

    boost::property_tree::ptree createPage;
    createPage.put("operation", "create");
    createPage.put("typename", "journal_content");
    createPage.put("defined_id", "page_id");
    mCore->executeCommandJson(writeJson(createPage));

    createConnection(*mCore, "id", "page_id", "Extension");

    boost::property_tree::ptree deleteHeader;
    deleteHeader.put("operation", "destroy");
    deleteHeader.put("id", "id");
    mCore->executeCommandJson(writeJson(deleteHeader));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "true");
    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
        (void)v;
        counter++;
    }
    BOOST_CHECK_EQUAL(0, counter);
}

BOOST_FIXTURE_TEST_CASE( TestContains, NewAPITest ) 
{
    {
        boost::property_tree::ptree create;
        create.put("operation", "create");
        create.put("typename", "object");
        create.put("defined_id", "parent");
        create.put("params.caption", "sabbbb");

        mCore->executeCommandJson(writeJson(create));
    }
   
    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", ".caption contains \"bb\"");
    BOOST_CHECK_EQUAL(1, count(mCore->executeCommandJson(writeJson(query))));

    query.put("filter", ".caption contains \"aa\"");
    BOOST_CHECK_EQUAL(0, count(mCore->executeCommandJson(writeJson(query))));
}

BOOST_FIXTURE_TEST_CASE( TestRandom, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");

    mCore->executeCommandJson(writeJson(create));

    create.put("params.value", "b");
    mCore->executeCommandJson(writeJson(create));

    create.put("params.value", "c");
    mCore->executeCommandJson(writeJson(create));

    create.put("params.value", "d");
    mCore->executeCommandJson(writeJson(create));

    create.put("params.value", "e");
    mCore->executeCommandJson(writeJson(create));

    boost::property_tree::ptree query;
    query.put("operation", "random");
    query.put("typename", "variable");
    BOOST_CHECK_EQUAL(1, count(mCore->executeCommandJson(writeJson(query))));
}

BOOST_FIXTURE_TEST_CASE( TestCount, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");
    
    for(std::size_t i = 0; i < 5; ++i)
    {
        create.put("params.value", i);
        mCore->executeCommandJson(writeJson(create));
    }

    boost::property_tree::ptree create2;
    create2.put("operation", "create");
    create2.put("typename", "variable");
    mCore->executeCommandJson(writeJson(create2));

    boost::property_tree::ptree query;
    query.put("operation", "count");
    
    query.put("filter", "IS(variable) AND .value < \"3\"");

    auto result = mCore->executeCommandJson(writeJson(query));
    auto ol = readJson<boost::property_tree::ptree>(result);
    
    BOOST_CHECK_EQUAL(3, ol.get<std::size_t>("result"));
}

BOOST_FIXTURE_TEST_CASE( TestQueryWithConnections, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "variable");
    create.put("params.value", "a");

    for(std::size_t i = 0; i < 3; ++i)
    {
        create.put("params.defined_id", "id" + std::to_string(i));
        expectId(mCore->executeCommandJson(writeJson(create)));
    }

    create.put("typename", "connection");
    create.put("params.A", "id0");
    create.put("params.B", "id1");
    create.put("params.type", "Reference");

    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("params.A", "id2");

    expectId(mCore->executeCommandJson(writeJson(create)));

    for(int i = 0; i < 3; ++i)
    {
        std::string id = "id" + std::to_string(i);
        boost::property_tree::ptree query;
        query.put("operation", "query");
        query.put("filter", "IS(variable) AND .id = \"" + id + "\"");

        auto result = mCore->executeCommandJson(writeJson(query));
        
        auto ol = readJson<boost::property_tree::ptree>(result);
        
        for(auto& v : ol.get_child("connection_list"))
        {
           BOOST_CHECK(id == v.second.get<std::string>("a") || id == v.second.get<std::string>("b"));
           BOOST_CHECK_EQUAL("Reference", v.second.get<std::string>("type"));
        }
    }
}

BOOST_FIXTURE_TEST_CASE( TestMoney2, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "finance_dataPoint");
    create.put("params.totalPortfolioValue", "sadhlfsdhfjksad");
    
    expectError(mCore->executeCommandJson(writeJson(create)));

    create.put("params.totalPortfolioValue", "555");

    expectError(mCore->executeCommandJson(writeJson(create)));

    create.put("params.totalPortfolioValue", "555.55");

    expectError(mCore->executeCommandJson(writeJson(create)));

    create.put("params.totalPortfolioValue", "555.55EUR");

    expectId(mCore->executeCommandJson(writeJson(create)));

    create.put("defined_id", "id");
    create.put("params.totalPortfolioValue", "-555.55EUR");

    expectId(mCore->executeCommandJson(writeJson(create)));

    auto result = query("id", *mCore);
    BOOST_CHECK_EQUAL(result->get<std::string>("totalPortfolioValue"), "-555.55EUR");
}
