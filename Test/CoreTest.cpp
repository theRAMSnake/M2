#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE a
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include <boost/property_tree/ptree.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <chrono>

static std::shared_ptr<materia::ICore3> createTestCore()
{
   return materia::createCore({"/Users/arodich/snake/materia.bu1675468828.2425034"});
}

class CoreTest
{
public:
   CoreTest()
   : mCore(createTestCore())
   {

   }

protected:
   std::shared_ptr<materia::ICore3> mCore;
};

BOOST_FIXTURE_TEST_CASE( NewDayTest, CoreTest )
{
   mCore->onNewDay(boost::gregorian::day_clock::local_day());
}

BOOST_FIXTURE_TEST_CASE( NewWeekTest, CoreTest )
{
   mCore->onNewWeek();
}

BOOST_FIXTURE_TEST_CASE( PerformanceTest, CoreTest )
{
   return;
   using namespace std::chrono;
   auto start = high_resolution_clock::now();
   for(int i = 0; i < 1000; ++i)
   {
       {
           boost::property_tree::ptree op;
           op.put("operation", "query");
           op.put("filter", "ReferedBy(\"8735f22a-1bf2-4a43-9cbf-26f9fee35b8b\")");

           mCore->executeCommandJson(writeJson(op));
       }
       {
           boost::property_tree::ptree op;
           op.put("operation", "query");
           op.put("filter", "IS(JournalHeader)");

           mCore->executeCommandJson(writeJson(op));
       }
       {
           boost::property_tree::ptree op;
           op.put("operation", "query");
           op.put("filter", "IS(finance_stock) AND .ticker = \"GOOG\"");

           mCore->executeCommandJson(writeJson(op));
       }
       {
           boost::property_tree::ptree op;
           op.put("operation", "reward");
           op.put("points", 0);

           mCore->executeCommandJson(writeJson(op));
       }
       {
           mCore->executeCommandJson("{\"operation\": \"query\", \"ids\": [\"data.snp\"]}");
       }
       {
           mCore->executeCommandJson("{\"operation\": \"query\", \"ids\": [\"portfolio_goal\"]}");
       }
   }
   auto stop = high_resolution_clock::now();
   auto duration = duration_cast<milliseconds>(stop - start);
   std::cout << "Total time of all queries: " << duration.count() << "ms" << std::endl;
}
