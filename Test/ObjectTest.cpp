#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <memory>
#include "../Core/private/Object.hpp"
#include "../Core/private/JsonRestorationProvider.hpp"

class ObjectTest
{
public:
   ObjectTest()
   {
      mType.name = "test";
      mType.fields = std::vector<materia::FieldDef>{
          {"f1", materia::Type::Timestamp}, {"f2", materia::Type::Choice, {"o1", "o2"}},
          {"f3", materia::Type::Money}, {"f4", materia::Type::Reference}
          };

      mObject = std::make_shared<materia::Object>(mType, materia::Id::generate());
   }

protected:
   materia::TypeDef mType; 
   std::shared_ptr<materia::Object> mObject;
};

BOOST_FIXTURE_TEST_CASE( TestTimestamp, ObjectTest ) 
{
    (*mObject)["f1"] = materia::Time {55};
    BOOST_CHECK_EQUAL(55, (*mObject)["f1"].get<materia::Type::Timestamp>().value);

    materia::JsonRestorationProvider p(mObject->toJson());
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(55, (*mObject)["f1"].get<materia::Type::Timestamp>().value);
}

BOOST_FIXTURE_TEST_CASE( TestOption, ObjectTest ) 
{
    (*mObject)["f2"] = "1";
    BOOST_CHECK_EQUAL("1", (*mObject)["f2"].get<materia::Type::Choice>());

    materia::JsonRestorationProvider p(mObject->toJson());
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL("1", (*mObject)["f2"].get<materia::Type::Choice>());
}

BOOST_FIXTURE_TEST_CASE( TestMoney, ObjectTest ) 
{
    (*mObject)["f3"] = 1;
    BOOST_CHECK_EQUAL(1, (*mObject)["f3"].get<materia::Type::Money>());

    materia::JsonRestorationProvider p(mObject->toJson());
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(1, (*mObject)["f3"].get<materia::Type::Money>());
}

BOOST_FIXTURE_TEST_CASE( TestReference, ObjectTest ) 
{
    auto id = materia::Id::generate();

    (*mObject)["f4"] = id;
    BOOST_CHECK_EQUAL(id, (*mObject)["f4"].toId());

    materia::JsonRestorationProvider p(mObject->toJson());
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(id, (*mObject)["f4"].toId());
}