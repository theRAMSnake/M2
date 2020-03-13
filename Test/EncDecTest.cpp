#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <string>
#include <Common/Codec.hpp>

class EncDecTest
{
public:
   EncDecTest()
   {
      
   }
};

BOOST_FIXTURE_TEST_CASE( Test, EncDecTest ) 
{  

    // plaintext, ciphertext, recovered text
    std::string ptext = "Now is the time for all good men to come to the aide of their country";
    std::string ctext, rtext;
  
    Codec c("password");

    ctext = c.encrypt(ptext);
    rtext = c.decrypt(ctext);
    
    BOOST_CHECK_EQUAL(ptext, rtext);
}