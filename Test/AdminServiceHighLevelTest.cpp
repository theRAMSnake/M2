#define BOOST_TEST_MODULE Admin
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <messages/admin.pb.h>
#include <Client/MateriaClient.hpp>
#include <Client/IAdmin.hpp>

namespace materia
{

bool operator!= (const materia::ServiceStatus& a, const materia::ServiceStatus& b)
{
   return !memcmp(&a, &b, sizeof(materia::ServiceStatus));
}

}

namespace std
{
   std::ostream& operator << (std::ostream& str, const materia::ServiceStatus& a)
   {
      str << a.name << " " << a.isAvailable;
      return str;
   }
}

BOOST_AUTO_TEST_CASE( Admin_GetComponentInfo ) 
{
   materia::MateriaClient client("test");
   auto& service = client.getAdmin();

   auto statusList = service.getServiceStatus();

   std::vector<materia::ServiceStatus> EXPECTED_LIST = {
      {"ActionsService", true},
      {"AdminService", true},
      {"CalendarService", true},
      {"ContainerService", true},
      {"DatabaseService", true},
      {"EventsService", true},
      {"InboxService", true},
      {"StrategyService", true}
   };
   
   BOOST_CHECK_EQUAL_COLLECTIONS(statusList.begin(), statusList.end(), 
      EXPECTED_LIST.begin(), EXPECTED_LIST.end());
}
