#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <thread>
#include "../Core/private/JsonSerializer.hpp"
#include <Core/ICore3.hpp>

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

bool expectError(const std::string& responce)
{
    auto t = readJson<boost::property_tree::ptree>(responce);
    return t.get_optional<std::string>("error").has_value();
}

bool expectId(const std::string& responce)
{
    auto t = readJson<boost::property_tree::ptree>(responce);
    auto r = t.get_optional<std::string>("result_id").has_value();

    if(r)
    {
        return true;
    }
    else
    {
        std::cout << "Expected id but was " << responce << std::endl;
        return false;
    }
}

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

BOOST_FIXTURE_TEST_CASE( TestChType, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "object");
    create.put("defined_id", "id");
    create.put("params.value", "a");

    mCore->executeCommandJson(writeJson(create));

    boost::property_tree::ptree chType;
    chType.put("operation", "change_type");
    chType.put("typename", "variable");
    chType.put("id", "id");

    mCore->executeCommandJson(writeJson(chType));

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

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestCompletable, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "calendar_item");
    create.put("defined_id", "id");
    create.put("params.text", "a");
    create.put("params.timestamp", 10);
    create.put("params.reccurencyType", 1);

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
    create.put("params.reccurencyType", 0);

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

BOOST_FIXTURE_TEST_CASE( TestJournalIndexUpdate, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "journal_header");
    create.put("defined_id", "id");
    create.put("params.isPage", false);
    create.put("params.modified", 555);

    mCore->executeCommandJson(writeJson(create));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(journal_header)");
    auto tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(query)));

    for(auto& v : tr.get_child("object_list"))
    {
        BOOST_CHECK_EQUAL(555, v.second.get<int>("modified"));
    }

    boost::property_tree::ptree createPage;
    createPage.put("operation", "create");
    createPage.put("typename", "journal_content");
    createPage.put("defined_id", "page_id");
    createPage.put("params.headerId", "id");
    mCore->executeCommandJson(writeJson(createPage));

    tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(query)));

    auto r = mCore->executeCommandJson(writeJson(query));
    //std::cout << r;
    tr = readJson<boost::property_tree::ptree>(r);

    int oldTs = 0;
    for(auto& v : tr.get_child("object_list"))
    {
        BOOST_CHECK(555 != v.second.get<int>("modified"));
        BOOST_CHECK(true == v.second.get<bool>("isPage"));
        oldTs = v.second.get<int>("modified");
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    boost::property_tree::ptree modifyPage;
    modifyPage.put("operation", "modify");
    modifyPage.put("id", "page_id");
    modifyPage.put("params.content", "sdghdfkhgsdfkg");
    modifyPage.put("params.headerId", "id");
    std::cout << mCore->executeCommandJson(writeJson(modifyPage));

    r = mCore->executeCommandJson(writeJson(query));
    //std::cout << r;
    tr = readJson<boost::property_tree::ptree>(r);

    tr = readJson<boost::property_tree::ptree>(r);
    for(auto& v : tr.get_child("object_list"))
    {
        BOOST_CHECK(oldTs != v.second.get<int>("modified"));
        BOOST_CHECK(true == v.second.get<bool>("isPage"));
        oldTs = v.second.get<int>("modified");
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    boost::property_tree::ptree deletePage;
    deletePage.put("operation", "destroy");
    deletePage.put("id", "page_id");
    mCore->executeCommandJson(writeJson(deletePage));

    tr = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(query)));
    for(auto& v : tr.get_child("object_list"))
    {
        BOOST_CHECK(oldTs != v.second.get<int>("modified"));
        BOOST_CHECK(false == v.second.get<bool>("isPage"));
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
    createPage.put("params.headerId", "id");
    mCore->executeCommandJson(writeJson(createPage));

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

BOOST_FIXTURE_TEST_CASE( TestJournalIndexDeleteChildren, NewAPITest ) 
{
    boost::property_tree::ptree create;
    create.put("operation", "create");
    create.put("typename", "journal_header");
    create.put("defined_id", "parent");
    create.put("params.isPage", false);
    create.put("params.modified", 555);

    mCore->executeCommandJson(writeJson(create));

    create.erase("defined_id");
    create.put("params.parentFolderId", "parent");

    for(int i = 0; i < 3; ++i)
    {
        mCore->executeCommandJson(writeJson(create));
    }

    {
        boost::property_tree::ptree query;
        query.put("operation", "query");
        query.put("filter", "IS(journal_header)");
        auto result = mCore->executeCommandJson(writeJson(query));

        auto ol = readJson<boost::property_tree::ptree>(result);
        
        int counter = 0;
        for(auto& v : ol.get_child("object_list"))
        {
            (void)v;
            counter++;
        }
        BOOST_CHECK_EQUAL(4, counter);
    }

    boost::property_tree::ptree deleteHeader;
    deleteHeader.put("operation", "destroy");
    deleteHeader.put("id", "parent");
    mCore->executeCommandJson(writeJson(deleteHeader));

    {
        boost::property_tree::ptree query;
        query.put("operation", "query");
        query.put("filter", "IS(journal_header)");
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
}