#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <Core/ICore3.hpp>
#include <boost/property_tree/ptree.hpp>
#include "../Core/private/JsonSerializer.hpp"
#include <chrono>

class CoreTest
{
public:
   CoreTest()
   {
      system("rm TestCore.db");
      mCore = materia::createCore({"TestCore.db"});
   }

protected:
   std::string readFile(const std::string& fileName) {
    // Open the file in text mode, which is the default mode for ifstream
    std::ifstream file(fileName);

    if (!file.is_open()) {
        throw std::runtime_error("Could not open file " + fileName);
    }

    // Create a stringstream buffer
    std::stringstream buffer;

    // Read the whole file into the buffer
    buffer << file.rdbuf();

    // Close the file (Optional, as the file will be closed in ifstream's destructor)
    file.close();

    // Convert the stringstream buffer into a string and return
    return buffer.str();
   }

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

BOOST_FIXTURE_TEST_CASE( NewDayTest, CoreTest )
{
   mCore->onNewDay(boost::gregorian::day_clock::local_day());
   BOOST_CHECK_EQUAL("1", run(readFile("../Test/dailytest.py")));
}

BOOST_FIXTURE_TEST_CASE( HealthcheckTest, CoreTest )
{
   mCore->healthcheck();
   mCore->healthcheck();
   mCore->healthcheck();
   mCore->healthcheck();
   mCore->healthcheck();
   mCore->healthcheck();
}
