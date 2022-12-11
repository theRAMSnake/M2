#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include "../Core/private/Expressions2.hpp"
#include "../Core/private/Database.hpp"

class Expressions2Test
{
public:
   Expressions2Test()
   {
      system("rm Test.db");
      mDb.reset(new materia::Database("Test.db"));
      mConnections = std::make_unique<materia::Connections>(*mDb);
      mCtx = std::make_unique<materia::v2::InterpreterContext>(*mConnections);
   }

protected:

   template<class T>
   T eval(const std::string& src)
   {
       auto expr = materia::v2::parseExpression(src);
       if(!expr)
       {
           BOOST_REQUIRE_MESSAGE(expr, "Unable to parse: " + src);
       }

       return std::get<T>(expr->evaluate(*mCtx));
   }

   template<class T>
   T eval(const std::string& src, const materia::Object& o)
   {
       mCtx->setObject(o);

       auto expr = materia::v2::parseExpression(src);
       if(!expr)
       {
           BOOST_REQUIRE_MESSAGE(expr, "Unable to parse: " + src);
       }

       return std::get<T>(expr->evaluate(*mCtx));
   }

   std::unique_ptr<materia::Database> mDb;
   std::unique_ptr<materia::Connections> mConnections;
   std::unique_ptr<materia::v2::InterpreterContext> mCtx;
};


BOOST_FIXTURE_TEST_CASE( TestBoolean, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("false"));
}

BOOST_FIXTURE_TEST_CASE( TestNot, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("!false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!!false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("!!true"));
}

BOOST_FIXTURE_TEST_CASE( TestEq, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("true = true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true = true = false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true = false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("false = false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!true = true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true = !true"));
}

BOOST_FIXTURE_TEST_CASE( TestLogical, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("true OR true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("false OR false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("true OR false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("true AND true"));
   BOOST_CHECK_EQUAL(false, eval<bool>("false AND false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true AND false"));

   BOOST_CHECK_EQUAL(false, eval<bool>("true AND true AND false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("true AND true OR false"));

   BOOST_CHECK_EQUAL(true, eval<bool>("true AND true = true OR false"));
   BOOST_CHECK_EQUAL(false, eval<bool>("true AND true = false AND false"));
}

BOOST_FIXTURE_TEST_CASE( TestGroups, Expressions2Test )
{
   BOOST_CHECK_EQUAL(false, eval<bool>("(true AND true) AND false"));
   BOOST_CHECK_EQUAL(true, eval<bool>("(true AND true) OR (true OR false)"));
   BOOST_CHECK_EQUAL(true, eval<bool>("(true) OR (false)"));
   BOOST_CHECK_EQUAL(false, eval<bool>("!(true OR false)"));
}

BOOST_FIXTURE_TEST_CASE( TestConstants, Expressions2Test )
{
   BOOST_CHECK_EQUAL(false, eval<bool>("5 = 6"));
   BOOST_CHECK_EQUAL(true, eval<bool>("-8 = -8"));

   BOOST_CHECK_EQUAL(false, eval<bool>("5.8 = 6.44"));
   BOOST_CHECK_EQUAL(true, eval<bool>("8.99 = 8.99"));

   BOOST_CHECK_EQUAL(false, eval<bool>("\"str1\" = \"str2\""));
   BOOST_CHECK_EQUAL(true, eval<bool>("\"str1\" = \"str1\""));
   BOOST_CHECK_EQUAL(true, eval<bool>("\"str1 str\" = \"str1 str\""));

   BOOST_CHECK_EQUAL(false, eval<bool>("5d = 6d"));
   BOOST_CHECK_EQUAL(false, eval<bool>("7d = 1m"));
   BOOST_CHECK_EQUAL(true, eval<bool>("1m = 1m"));

   BOOST_CHECK_EQUAL(false, eval<bool>("id-577 = id-578"));
   BOOST_CHECK_EQUAL(true, eval<bool>("id-577 = id-577"));

   BOOST_CHECK_EQUAL(false, eval<bool>("53.25EUR = 102.00EUR"));
   BOOST_CHECK_EQUAL(false, eval<bool>("102.00USD = 102.00EUR"));
   BOOST_CHECK_EQUAL(true, eval<bool>("102.00EUR = 102.00EUR"));
}

BOOST_FIXTURE_TEST_CASE( TestComparisson, Expressions2Test )
{
   BOOST_CHECK_EQUAL(true, eval<bool>("5 < 6"));
   BOOST_CHECK_EQUAL(false, eval<bool>("7 < 6"));
   BOOST_CHECK_EQUAL(true, eval<bool>("-8 > -9"));
   BOOST_CHECK_EQUAL(false, eval<bool>("-10 > -9"));

   BOOST_CHECK_EQUAL(true, eval<bool>("5.5 < 6.5"));
   BOOST_CHECK_EQUAL(false, eval<bool>("7.5 < 6.5"));
   BOOST_CHECK_EQUAL(true, eval<bool>("-8.5 > -9.5"));
   BOOST_CHECK_EQUAL(false, eval<bool>("-10.5 > -9.5"));

   BOOST_CHECK_EQUAL(true, eval<bool>("5.55EUR < 6.55EUR"));
   BOOST_CHECK_EQUAL(false, eval<bool>("7.55EUR < 6.55EUR"));
   BOOST_CHECK_EQUAL(true, eval<bool>("-8.55EUR > -9.55EUR"));
   BOOST_CHECK_EQUAL(false, eval<bool>("-10.55EUR > -9.55EUR"));
}

BOOST_FIXTURE_TEST_CASE( TestFields, Expressions2Test )
{
    materia::Object obj({"object", "tbl", {{"val1", materia::Type::Int}, {"val3", materia::Type::Int}, {"vb1", materia::Type::Bool}, {"vb2", materia::Type::Bool}}}, materia::Id::generate());
    obj["val1"] = 5;
    obj["val3"] = 2;
    obj["val2"] = "str";
    obj["vb1"] = true;
    obj["vb2"] = true;

   BOOST_CHECK_EQUAL(5, eval<std::int64_t>(".val1", obj));
   BOOST_CHECK_EQUAL("str", eval<std::string>(".val2", obj));
   BOOST_CHECK_THROW(eval<std::string>(".wrongfield", obj), std::runtime_error);

   BOOST_CHECK_EQUAL(true, eval<bool>(".val1 < 6"));
   BOOST_CHECK_EQUAL(false, eval<bool>(".val2 = \"yyy\""));

   BOOST_CHECK_EQUAL(false, eval<bool>(".val1 = .val2", obj));

   BOOST_CHECK_EQUAL(false, eval<bool>(".val1 < .val3"));
   BOOST_CHECK_EQUAL(false, eval<bool>(".vb1 AND false"));
   BOOST_CHECK_EQUAL(true, eval<bool>(".vb1 AND .vb2"));
}
materia::TypeDef gType {
   "test",
   "test",
   {{"some", materia::Type::Int}}
};

BOOST_FIXTURE_TEST_CASE( TestValueExp, Expressions2Test ) 
{  
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;

   mCtx->setObject(p);
   std::cout << "Index: " << materia::v2::parseExpression("5")->evaluate(*mCtx).index() << std::endl;
   BOOST_CHECK_EQUAL(5, std::get<std::int64_t>(materia::v2::parseExpression("5")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(5.0, std::get<double>(materia::v2::parseExpression("5.0")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL("5", std::get<std::string>(materia::v2::parseExpression("\"5\"")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("true")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(6, std::get<std::int64_t>(materia::v2::parseExpression(".some")->evaluate(*mCtx)));
}

BOOST_FIXTURE_TEST_CASE( TestSimpleBinaryExp, Expressions2Test ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;

   mCtx->setObject(p);
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression(".some < 10")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression(".some < 5")->evaluate(*mCtx)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression(".some > 5")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression(".some > 10")->evaluate(*mCtx)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression(".some = 6")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("7 = .some")->evaluate(*mCtx)));
}

BOOST_FIXTURE_TEST_CASE( TestContainsExp, Expressions2Test ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["som"] = std::string("aaa");
   p["some1"] = std::string("aa");
   p["some2"] = std::string("bb");

   mCtx->setObject(p);
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression(".som contains .some1")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression(".som contains .some2")->evaluate(*mCtx)));
}

BOOST_FIXTURE_TEST_CASE( TestAndOrExp, Expressions2Test ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["some"] = 6;
   p["som"] = std::string("aaa");
   p["some1"] = std::string("aa");
   p["some3"] = std::string("bb");

   mCtx->setObject(p);
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("(.som contains .some1) AND (.some < 10)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("(.som contains .some1) AND (.some < 1)")->evaluate(*mCtx)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("(.som contains .some1) OR (.some < 10)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("(.som contains .some3) OR (.some < 1)")->evaluate(*mCtx)));
}

BOOST_FIXTURE_TEST_CASE( TestContainsString, Expressions2Test ) 
{
   materia::Object p(gType, materia::Id::Invalid);
   p["som"] = std::string("aaa");
   p["p"] = 5;

   mCtx->setObject(p);
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("IS(test)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("IS(test1)")->evaluate(*mCtx)));

   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression(".som contains \"aa\"")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("(.p = 5) AND (.som contains \"bb\")")->evaluate(*mCtx)));
}

BOOST_FIXTURE_TEST_CASE( TestCons, Expressions2Test ) 
{
   materia::Object p(gType, materia::Id("id1"));
   materia::Object p2(gType, materia::Id("id2"));
   materia::Object p3(gType, materia::Id("id3"));
   
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Hierarchy);
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Reference);
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Extension);
   mConnections->create(materia::Id("id1"), materia::Id("id2"), materia::ConnectionType::Requirement);

   mCtx->setObject(p);
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("ParentOf(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ChildOf(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("Refers(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ReferedBy(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("ExtendedBy(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Extends(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("Enables(id2)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Requires(id2)")->evaluate(*mCtx)));

   mCtx->setObject(p2);
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ParentOf(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("ChildOf(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Refers(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("ReferedBy(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ExtendedBy(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("Extends(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Enables(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(true, std::get<bool>(materia::v2::parseExpression("Requires(id1)")->evaluate(*mCtx)));

   mCtx->setObject(p3);
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ParentOf(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ChildOf(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Refers(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ReferedBy(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("ExtendedBy(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Extends(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Enables(id1)")->evaluate(*mCtx)));
   BOOST_CHECK_EQUAL(false, std::get<bool>(materia::v2::parseExpression("Requires(id1)")->evaluate(*mCtx)));
}
