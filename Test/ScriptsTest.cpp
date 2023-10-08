#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include "Utils.hpp"

class ScriptsTest
{
public:
   ScriptsTest()
   {
      system("rm Test.db");
      mCore = materia::createCore({"Test.db"});
       {
          boost::property_tree::ptree create;
          create.put("operation", "create");
          create.put("typename", "object");
          create.put("defined_id", "reward.coins");
          create.put("params.Red", 0);
          create.put("params.Blue", 0);
          create.put("params.Yellow", 0);
          create.put("params.Purple", 0);
          create.put("params.Green", 0);

          mCore->executeCommandJson(writeJson(create));
       }
   }

protected:
   std::string run(const std::string& code)
   {
       boost::property_tree::ptree cmd;
       cmd.put("operation", "run");
       cmd.put("script", code);

       auto result = mCore->executeCommandJson(writeJson(cmd));
       auto ol = readJson<boost::property_tree::ptree>(result);
       auto val = ol.get_optional<std::string>("result");

       if(val)
       {
           return *val;
       }
       else
       {
           return ol.get<std::string>("error");
       }
   }

   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE(TestNoApi, ScriptsTest)
{
    BOOST_CHECK_EQUAL("5", run("result = 2+3"));
    BOOST_CHECK_EQUAL("'division by zero'", run("result = 5/0"));
    BOOST_CHECK_EQUAL("No 'result' key found in global dictionary.", run("\"jjj\""));
}

BOOST_FIXTURE_TEST_CASE(TestCreateApi, ScriptsTest)
{
    BOOST_CHECK_EQUAL("id_preset", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
result = m4.create("id_preset", "object", empty_obj)
     )"));

    BOOST_CHECK(query("id_preset", *mCore));
    BOOST_CHECK_EQUAL("object", query("id_preset", *mCore)->get<std::string>("typename"));
}
BOOST_FIXTURE_TEST_CASE(TestCreateApiWithAttrs, ScriptsTest)
{
    BOOST_CHECK_EQUAL("id_preset", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
empty_obj.intval = 5
empty_obj.strval = "str"
result = m4.create("id_preset", "object", empty_obj)
     )"));

    BOOST_CHECK(query("id_preset", *mCore));
    BOOST_CHECK_EQUAL("object", query("id_preset", *mCore)->get<std::string>("typename"));
    BOOST_CHECK_EQUAL("5", query("id_preset", *mCore)->get<std::string>("intval"));
    BOOST_CHECK_EQUAL("str", query("id_preset", *mCore)->get<std::string>("strval"));
}
BOOST_FIXTURE_TEST_CASE(TestCreateInvalid, ScriptsTest)
{
    BOOST_CHECK_EQUAL("'argument 1 must be str, not None'", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
result = m4.create(None, "object", empty_obj)
     )"));

    BOOST_CHECK_EQUAL("'Wrong type while creating object '", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
result = m4.create("id_preset", "", empty_obj)
     )"));

    BOOST_CHECK_EQUAL("'Expected an object with attributes'", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
result = m4.create("id_preset", "object", None)
     )"));

    BOOST_CHECK_EQUAL("'argument 2 must be str, not None'", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
result = m4.create("id_preset", None, empty_obj)
     )"));

    BOOST_CHECK_EQUAL("'argument 1 must be str, not None'", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
result = m4.create(None, None, empty_obj)
     )"));
}
BOOST_FIXTURE_TEST_CASE(TestCreateApiLogicalError, ScriptsTest)
{
    BOOST_CHECK_EQUAL("'Object with id id_preset already exist'", run(R"(
class EmptyObject:
    pass

empty_obj = EmptyObject()
empty_obj.intval = 5
empty_obj.strval = "str"
result = m4.create("id_preset", "object", empty_obj)
result = m4.create("id_preset", "object", empty_obj)
     )"));

    // There still should be object
    BOOST_CHECK(query("id_preset", *mCore));
    BOOST_CHECK_EQUAL("object", query("id_preset", *mCore)->get<std::string>("typename"));
    BOOST_CHECK_EQUAL("5", query("id_preset", *mCore)->get<std::string>("intval"));
    BOOST_CHECK_EQUAL("str", query("id_preset", *mCore)->get<std::string>("strval"));
}
