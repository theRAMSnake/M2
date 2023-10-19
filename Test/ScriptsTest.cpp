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
BOOST_FIXTURE_TEST_CASE(TestModifyApiWithAttrs, ScriptsTest)
{
    // First, we'll set up an object similar to how you did in the create test.
    run(R"(
class SetupObject:
    pass

initial_obj = SetupObject()
initial_obj.intval = 5
initial_obj.strval = "str"
m4.create("id_to_modify", "object", initial_obj)
)");

    // Now, let's modify that object using the API
    run(R"(
class SetupObject:
    pass

obj_to_modify = SetupObject()
obj_to_modify.intval = 10
obj_to_modify.strval = "modified_str"
result = m4.modify("id_to_modify", obj_to_modify)
)");

    // Now, let's check if the values have been modified.
    BOOST_CHECK(query("id_to_modify", *mCore));

    BOOST_CHECK_EQUAL("object", query("id_to_modify", *mCore)->get<std::string>("typename"));
    BOOST_CHECK_EQUAL("10", query("id_to_modify", *mCore)->get<std::string>("intval"));  // Note the change in expected value
    BOOST_CHECK_EQUAL("modified_str", query("id_to_modify", *mCore)->get<std::string>("strval"));  // Note the change in expected value
}
BOOST_FIXTURE_TEST_CASE(TestModifyApiWithAttrs2, ScriptsTest)
{
    // First, we'll set up an object similar to how you did in the create test.
    run(R"(
class SetupObject:
    pass

initial_obj = SetupObject()
initial_obj.intval = 5
initial_obj.strval = "str"
m4.create("id_to_modify", "object", initial_obj)
)");

    // Now, let's modify that object using the API
    run(R"(
obj_to_modify = m4.query_ids(["id_to_modify"])[0]
obj_to_modify.intval = 10
obj_to_modify.strval = "modified_str"
m4.modify("id_to_modify", obj_to_modify)
)");

    // Now, let's check if the values have been modified.
    BOOST_CHECK(query("id_to_modify", *mCore));

    BOOST_CHECK_EQUAL("object", query("id_to_modify", *mCore)->get<std::string>("typename"));
    BOOST_CHECK_EQUAL("10", query("id_to_modify", *mCore)->get<std::string>("intval"));  // Note the change in expected value
    BOOST_CHECK_EQUAL("modified_str", query("id_to_modify", *mCore)->get<std::string>("strval"));  // Note the change in expected value
}
BOOST_FIXTURE_TEST_CASE(TestEraseApi, ScriptsTest)
{
    // First, let's set up an object.
    run(R"(
class SetupObject:
    pass

initial_obj = SetupObject()
initial_obj.intval = 5
initial_obj.strval = "str"
m4.create("id_to_erase", "object", initial_obj)
)");

    // Now, let's erase that object using the API.
    run(R"(
result = m4.erase("id_to_erase")
)");

    // Check if the object has been erased.
    BOOST_CHECK(!query("id_to_erase", *mCore));  // Expecting the query to return null or equivalent.
}
BOOST_FIXTURE_TEST_CASE(TestQueryIdsApi, ScriptsTest)
{
    // Create the first object with a known id
    BOOST_CHECK_EQUAL("id_1", run(R"(
class SetupObject:
    pass

obj1 = SetupObject()
obj1.intval = "5"
obj1.strval = "string1"
result = m4.create("id_1", "object", obj1)
)"));

    // Create the second object with another known id
    BOOST_CHECK_EQUAL("id_2", run(R"(
class SetupObject:
    pass
obj2 = SetupObject()
obj2.intval = "10"
obj2.strval = "string2"
result = m4.create("id_2", "object", obj2)
)"));

    // Query the objects using their ids, validate the size of the result list, and verify their attributes
    BOOST_CHECK_EQUAL("2", run(R"(
queried_objects = m4.query_ids(["id_1", "id_2"])
result = len(queried_objects)
)"));

    // Check the integer values of the objects
    std::string checkIntvalScript = R"(
queried_objects = m4.query_ids(["id_1", "id_2"])
intvals = [queried_objects[0].intval, queried_objects[1].intval]
result = 'intvals' if '5' in intvals and '10' in intvals else 'Fail'
)";
    BOOST_CHECK_EQUAL("intvals", run(checkIntvalScript));
}

BOOST_FIXTURE_TEST_CASE(TestQueryExprApi, ScriptsTest)
{
    // Create the first object with a known id
    BOOST_CHECK_EQUAL("id_1", run(R"(
class SetupObject:
    pass

obj1 = SetupObject()
obj1.intval = "5"
obj1.strval = "string1"
result = m4.create("id_1", "object", obj1)
)"));

    // Create the second object with another known id, but with a different intval
    BOOST_CHECK_EQUAL("id_2", run(R"(
class SetupObject:
    pass
obj2 = SetupObject()
obj2.intval = "10"
obj2.strval = "string2"
result = m4.create("id_2", "object", obj2)
)"));

    // Query the objects using the expression ".intval = 5" and validate the size of the result list
    BOOST_CHECK_EQUAL("1", run(R"(
queried_objects = m4.query_expr(".intval = \"5\"")
result = len(queried_objects)
)"));

    // Verify the attributes of the returned object
    BOOST_CHECK_EQUAL("5", run(R"(
queried_objects = m4.query_expr(".intval = \"5\"")
queried_object = queried_objects[0]
result = queried_object.intval
)"));

    BOOST_CHECK_EQUAL("string1", run(R"(
queried_objects = m4.query_expr(".intval = \"5\"")
queried_object = queried_objects[0]
result = queried_object.strval
)"));
}

BOOST_FIXTURE_TEST_CASE(TestCreateWithParent, ScriptsTest) {
    // First, create a parent object.
    std::string parentCreationScript = R"(
class ParentObject:
    pass

parent_obj = ParentObject()
parent_obj.some_attribute = "value"
result = m4.create("pid", "object", parent_obj)
    )";
    std::string parent = run(parentCreationScript);
    BOOST_CHECK_EQUAL(parent, "pid");

    // Now, create a child object, specifying the parent's ID.
    std::string childCreationScript = R"(
class ChildObject:
    pass

child_obj = ChildObject()
child_obj.some_other_attribute = "other_value"
result = m4.create("", "object", child_obj, "pid")
    )";
    std::string childId = run(childCreationScript);
    BOOST_REQUIRE(!childId.empty());  // Check that the child was created successfully.

    std::string queryScript = R"(
children = m4.query_expr('ChildOf("pid")')
result = children[0].id
    )";

    std::string queriedChildrenResult = run(queryScript);

    BOOST_CHECK_EQUAL(queriedChildrenResult, childId);  // Check that the IDs match.
                                                        //
    queryScript = R"(
import common
children = common.queryChildren("pid")
result = children[0].id
    )";

    queriedChildrenResult = run(queryScript);

    BOOST_CHECK_EQUAL(queriedChildrenResult, childId);  // Check that the IDs match.
}
