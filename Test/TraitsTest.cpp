#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <Core/ICore3.hpp>

static void putTraits(boost::property_tree::ptree& src, const std::string& trait)
{
    boost::property_tree::ptree subParams;

    boost::property_tree::ptree p;
    p.put("", trait);
    subParams.push_back({"", p});

    src.add_child("traits", subParams);
}

static void putRequires(boost::property_tree::ptree& src, const std::string& field, const std::string& type)
{
    boost::property_tree::ptree subParams;

    boost::property_tree::ptree p;
    p.put("field", field);
    p.put("type", type);
    subParams.push_back({"", p});

    src.add_child("params.requires", subParams);
}

class TraitsTest
{
public:
   TraitsTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});

      boost::property_tree::ptree createType;
      createType.put("operation", "create");
      putTraits(createType, "trait");
      createType.put("params.name", "tp");
      putRequires(createType, "value", "int");

      std::cout << writeJson(createType);
      mCore->executeCommandJson(writeJson(createType));
   }

protected:
   std::shared_ptr<materia::ICore3> mCore;
};

static bool expectError(const std::string& responce)
{
    auto t = readJson<boost::property_tree::ptree>(responce);
    return t.get_optional<std::string>("error").has_value();
}

bool expectSuccess(const std::string& responce)
{
    auto t = readJson<boost::property_tree::ptree>(responce);
    return t.get_optional<std::string>("success").has_value();
}

static bool expectId(const std::string& responce)
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

BOOST_FIXTURE_TEST_CASE( TestInvalid_Trait, TraitsTest ) 
{  
   boost::property_tree::ptree create;
   create.put("operation", "create");
   
   putTraits(create, "tp");
   create.put("params.some", "a");

   BOOST_CHECK(expectError(mCore->executeCommandJson(writeJson(create))));

   create.put("params.value", "a");

   BOOST_CHECK(expectError(mCore->executeCommandJson(writeJson(create))));
}

BOOST_FIXTURE_TEST_CASE( TestCreate_Trait, TraitsTest ) 
{  
    boost::property_tree::ptree create;
    create.put("operation", "create");
    
    putTraits(create, "tp");
    create.put("params.value", 5);

    BOOST_CHECK(expectId(mCore->executeCommandJson(writeJson(create))));

    boost::property_tree::ptree query;
    query.put("operation", "query");
    query.put("filter", "IS(tp)");

    auto result = mCore->executeCommandJson(writeJson(query));

    std::cout << result;
    auto ol = readJson<boost::property_tree::ptree>(result);
    
    unsigned int counter = 0;
    for(auto& v : ol.get_child("object_list"))
    {
       (void)v;
       counter++;
    }

    BOOST_CHECK_EQUAL(1, counter);
}

BOOST_FIXTURE_TEST_CASE( TestModifyInvalid_Trait, TraitsTest ) 
{
   boost::property_tree::ptree create;
   create.put("operation", "create");
   
   putTraits(create, "tp");
   create.put("params.value", 5);

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
   modify.put("params.value", "b");
   modify.put("id", r.get<std::string>("id"));

   BOOST_CHECK(expectError(mCore->executeCommandJson(writeJson(modify))));
}

BOOST_FIXTURE_TEST_CASE( TestModifyValid_Trait, TraitsTest ) 
{
   boost::property_tree::ptree create;
   create.put("operation", "create");
   
   putTraits(create, "tp");
   create.put("params.value", 5);

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
   modify.put("params.value", 6);
   modify.put("id", r.get<std::string>("id"));

   BOOST_CHECK(expectSuccess(mCore->executeCommandJson(writeJson(modify))));
}