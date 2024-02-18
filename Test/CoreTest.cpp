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

BOOST_FIXTURE_TEST_CASE(TestUpdateFunction_NotEnoughAmbitions, CoreTest)
{
    // Script to test the behavior of the 'update' function when there are not enough ambitions.
    std::string script_result = run(R"(
import ambitions
import reward
import m4
import datetime

# Setup the 'reward.coins' object with a certain number of coins.
initial_coins = 10  # for example
initial = m4.MateriaObject()
initial.Red = initial_coins
initial.Blue = initial_coins
initial.Yellow = initial_coins
initial.Green = initial_coins
initial.Purple = initial_coins
m4.create('reward.coins', 'object', initial)

# Execute the 'update' function which is expected to apply penalties due to a lack of ambitions.
today = datetime.datetime.now()
ambitions.update(today)

# Retrieve the 'reward.coins' object after the update.
coins_after_update = m4.query_ids(['reward.coins'])[0]

# Calculate the total number of coins after the update.
total_coins_after = int(coins_after_update.Red) + int(coins_after_update.Blue) + int(coins_after_update.Yellow) + int(coins_after_update.Green) + int(coins_after_update.Purple)

# Determine the expected total based on the penalties described in your system's rules.
expected_total_after_penalty = 5 * initial_coins - 2

# Prepare the result message based on whether the total coins match the expected amount after penalties.
if total_coins_after == expected_total_after_penalty:
    result = 'Penalty applied correctly for not enough ambitions.'
else:
    result = f'Error: Expected {expected_total_after_penalty} coins, got {total_coins_after}.'
    )");

    // Check the script's execution result.
    BOOST_CHECK_EQUAL("Penalty applied correctly for not enough ambitions.", script_result);
}

BOOST_FIXTURE_TEST_CASE(TestUpdateFunction_AmbitionExpiry, CoreTest)
{
    // Script to test the 'update' function's behavior with ambition expiry.
    std::string script_result = run(R"(
import ambitions
import reward
import m4
import datetime
import collection

expiry_imminent = datetime.datetime.now() + datetime.timedelta(minutes=1)
expiry_future = datetime.datetime.now() + datetime.timedelta(days=10)

ambitions.create_ambition('Ambition 1', 'Red', expiry_future)
ambitions.create_ambition('Ambition 2', 'Blue', expiry_imminent)  # This ambition is set to expire.
ambitions.create_ambition('Ambition 3', 'Green', expiry_future)

initial = m4.MateriaObject()
initial.Red = 10
initial.Blue = 10
initial.Yellow = 10
initial.Green = 10
initial.Purple = 10
m4.create('reward.coins', 'object', initial)

# Execute the 'update' function for today. It shouldn't apply penalties yet as the ambition is not expired at the start of the day.
today = datetime.datetime.now() + datetime.timedelta(minutes=1)
ambitions.update(today)

# Simulate the passage of time to the next day when the ambition has expired.
next_day = today + datetime.timedelta(days=1)

# Execute the 'update' function again, which should now recognize the expired ambition and apply penalties.
ambitions.update(next_day)

# Retrieve the 'reward.coins' object after the updates.
coins_after_update = m4.query_ids(['reward.coins'])[0]

# Calculate the total number of coins after the update.
total_coins_after = int(coins_after_update.Red) + int(coins_after_update.Blue) + int(coins_after_update.Yellow) + int(coins_after_update.Green) + int(coins_after_update.Purple)

# Here, you need to determine the expected total based on what penalties or rewards should have been applied.
# This depends on your system's logic for handling expired ambitions.
expected_total_after_changes = 50 - 2 - 2 # For example, if you expect a 2-coin penalty for the expired ambition.

# Build the result message.
if total_coins_after == expected_total_after_changes and len(collection.Collection('ambitions').get_items()) == 3:
    result = 'Update function handled ambition expiry correctly.'
else:
    result = f'Error: Expected {expected_total_after_changes} coins, got {total_coins_after}.'
    )");

    // Check the script's execution result.
    BOOST_CHECK_EQUAL("Update function handled ambition expiry correctly.", script_result);
}

BOOST_FIXTURE_TEST_CASE(TestUpdateFunction_AmbitionCompleted, CoreTest)
{
    // Script to test the 'update' function's behavior with ambition expiry.
    std::string script_result = run(R"(
import ambitions
import reward
import m4
import datetime
import collection

expiry_future = datetime.datetime.now() + datetime.timedelta(days=10)

ambitions.create_ambition('Ambition 1', 'Red', expiry_future)
ambitions.create_ambition('Ambition 2', 'Blue', expiry_future)
ambitions.create_ambition('Ambition 3', 'Green', expiry_future)

# Execute the 'update' function for today. It shouldn't apply penalties yet as the ambition is not expired at the start of the day.
today = datetime.datetime.now()
ambitions.update(today)

ambitions_collection = collection.Collection('ambitions')
ambitions.complete_ambition(ambitions_collection.get_items()[0].id)

# Simulate the passage of time to the next day when the ambition has expired.
next_day = today + datetime.timedelta(days=1)

# Execute the 'update' function again, which should now recognize the expired ambition and apply penalties.
ambitions.update(next_day)
ambitions.update(next_day)
ambitions.update(next_day)

#+6 here

# Retrieve the 'reward.coins' object after the updates.
coins_after_update = m4.query_ids(['reward.coins'])[0]

# Calculate the total number of coins after the update.
total_coins_after = int(coins_after_update.Red) + int(coins_after_update.Blue) + int(coins_after_update.Yellow) + int(coins_after_update.Green) + int(coins_after_update.Purple)

# Here, you need to determine the expected total based on what penalties or rewards should have been applied.
# This depends on your system's logic for handling expired ambitions.
expected_total_after_changes = 6  # For example, if you expect a 2-coin penalty for the expired ambition.

# Build the result message.
if total_coins_after == expected_total_after_changes and len(collection.Collection('ambitions').get_items()) == 3:
    result = 'Update function handled ambition expiry correctly.'
else:
    result = f'Error: Expected {expected_total_after_changes} coins, got {total_coins_after}.'
    )");

    // Check the script's execution result.
    BOOST_CHECK_EQUAL("Update function handled ambition expiry correctly.", script_result);
}

BOOST_FIXTURE_TEST_CASE(TestUpdateFunction_AmbitionCompletedDissapear, CoreTest)
{
    // Script to test the 'update' function's behavior with ambition expiry.
    std::string script_result = run(R"(
import ambitions
import reward
import m4
import datetime
import collection

expiry_future = datetime.datetime.now() + datetime.timedelta(days=10)

ambitions.create_ambition('Ambition 1', 'Red', expiry_future)
ambitions.create_ambition('Ambition 2', 'Blue', expiry_future)
ambitions.create_ambition('Ambition 3', 'Green', expiry_future)

# Execute the 'update' function for today. It shouldn't apply penalties yet as the ambition is not expired at the start of the day.
today = datetime.datetime.now()
ambitions.update(today)

ambitions_collection = collection.Collection('ambitions')
for amb in ambitions_collection.get_items():
    ambitions.complete_ambition(amb.id)

# Simulate the passage of time to the next day when the ambition has expired.
next_day = today + datetime.timedelta(days=1)

# Execute the 'update' function again, which should now recognize the expired ambition and apply penalties.
ambitions.update(next_day)
ambitions.update(next_day + datetime.timedelta(days=1000))

result = len(collection.Collection('ambitions').get_items())
    )");

    // Check the script's execution result.
    BOOST_CHECK_EQUAL("0", script_result);
}
