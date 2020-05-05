#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <Core/ICore3.hpp>

void putTraits(boost::property_tree::ptree& src, const std::string& trait)
{
    boost::property_tree::ptree subParams;

    boost::property_tree::ptree p;
    p.put("", trait);
    subParams.push_back({"", p});

    src.add_child("traits", subParams);
}

class NewAPITest
{
public:
   NewAPITest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});

      boost::property_tree::ptree createType;
      createType.put("operation", "create");
      putTraits(createType, "trait");
      createType.put("params.name", "tp");

      mCore->executeCommandJson(writeJson(createType));
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
    auto r = t.get_optional<std::string>("id").has_value();

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
    
    putTraits(create, "tp");
    create.put("params.some", "a");

    BOOST_CHECK(expectId(mCore->executeCommandJson(writeJson(create))));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(tp)");

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
    
    putTraits(create, "tp");
    create.put("params.some", "a");

    auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree destroy;
    destroy.put("operation", "destroy");
    destroy.put("id", r.get<std::string>("id"));

    mCore->executeCommandJson(writeJson(destroy));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    
    query.put("filter", "IS(tp)");

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
    
    putTraits(create, "tp");
    create.put("params.some", "a");

    auto r = readJson<boost::property_tree::ptree>(mCore->executeCommandJson(writeJson(create)));

    boost::property_tree::ptree obj;
    {
        boost::property_tree::ptree query;
        query.put("operation", "query");
        query.put("filter", "IS(tp)");

        auto result = mCore->executeCommandJson(writeJson(query));
        auto ol = readJson<boost::property_tree::ptree>(result);

        obj = ol.get_child("object_list").begin()->second;
    }

    boost::property_tree::ptree modify;
    modify.put("operation", "modify");
    modify.put_child("params", obj);
    modify.put("params.some", "b");
    modify.put("id", r.get<std::string>("id"));

    mCore->executeCommandJson(writeJson(modify));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(tp)");

    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    for(auto& v : ol.get_child("object_list"))
    {
       BOOST_CHECK_EQUAL("b", v.second.get<std::string>("some"));
    }
}

BOOST_FIXTURE_TEST_CASE( TestQueryId, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    
    putTraits(create, "tp");
    create.put("params.some", "a");

    std::string r;
    for(std::size_t i = 0; i < 3; ++i)
    {
        r = mCore->executeCommandJson(writeJson(create));
    }

    auto tr = readJson<boost::property_tree::ptree>(r);

    boost::property_tree::ptree query;
    query.put("operation", "query");
    
    query.put("filter", "IS(tp) AND .id = \"" + tr.get<std::string>("id") + "\"");

    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    for(auto& v : ol.get_child("object_list"))
    {
       BOOST_CHECK_EQUAL(tr.get<std::string>("id"), v.second.get<std::string>("id"));
    }
}

BOOST_FIXTURE_TEST_CASE( TestQueryFilter, NewAPITest ) 
{ 
    boost::property_tree::ptree create;
    create.put("operation", "create");
    
    putTraits(create, "tp");
    
    for(std::size_t i = 0; i < 5; ++i)
    {
        create.put("params.some", i);
        mCore->executeCommandJson(writeJson(create));
    }

    boost::property_tree::ptree create2;
    create2.put("operation", "create");
    create2.put("trait", "tp");
    mCore->executeCommandJson(writeJson(create2));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    
    query.put("filter", "IS(tp) AND .some < 3");

    auto result = mCore->executeCommandJson(writeJson(query));

    auto ol = readJson<boost::property_tree::ptree>(result);
    
    for(auto& v : ol.get_child("object_list"))
    {
       BOOST_CHECK(v.second.get<int>("some") < 3);
    }
}