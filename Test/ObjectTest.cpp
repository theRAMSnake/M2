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
          {"f1", materia::Type::Timestamp}, {"f2", materia::Type::Option, {"o1", "o2"}},
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
    (*mObject)["f1"] = 55;
    BOOST_CHECK_EQUAL(55, static_cast<int>((*mObject)["f1"]));

    materia::JsonRestorationProvider p(toJson(*mObject));
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(55, static_cast<int>((*mObject)["f1"]));
}

BOOST_FIXTURE_TEST_CASE( TestOption, ObjectTest ) 
{
    (*mObject)["f2"] = 1;
    BOOST_CHECK_EQUAL(1, static_cast<int>((*mObject)["f2"]));

    materia::JsonRestorationProvider p(toJson(*mObject));
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(1, static_cast<int>((*mObject)["f2"]));
}

BOOST_FIXTURE_TEST_CASE( TestMoney, ObjectTest ) 
{
    (*mObject)["f3"] = 1;
    BOOST_CHECK_EQUAL(1, static_cast<int>((*mObject)["f3"]));

    materia::JsonRestorationProvider p(toJson(*mObject));
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(1, static_cast<int>((*mObject)["f3"]));
}

BOOST_FIXTURE_TEST_CASE( TestReference, ObjectTest ) 
{
    auto id = materia::Id::generate();

    (*mObject)["f4"] = id;
    BOOST_CHECK_EQUAL(id, (*mObject)["f4"].toId());

    materia::JsonRestorationProvider p(toJson(*mObject));
    p.populate(*mObject);   

    BOOST_CHECK_EQUAL(id, (*mObject)["f4"].toId());
}