#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <Core/ICore3.hpp>

class NewAPITest
{
public:
   NewAPITest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});

      boost::property_tree::ptree createType;
      createType.put("operation", "create");
      createType.put("type.domain", "core");
      createType.put("type.name", "type");
      createType.put("params.type.domain", "test");
      createType.put("params.type.name", "tp");

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
    boost::property_tree::ptree createType;
    createType.put("operation", "create");
    createType.put("type.domain", "test");
    createType.put("type.name", "tp");
    createType.put("params.some", "a");

    BOOST_CHECK(expectId(mCore->executeCommandJson(writeJson(createType))));

    //SNAKE: check
}