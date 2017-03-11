#include <iostream>
#include <Common/InterprocessService.hpp>
#include <messages/inbox.pb.h>

namespace materia
{
   
class InboxServiceImpl : public inbox::InboxService
{
public:
};

}

int main(int argc, char *argv[])
{
   materia::InboxServiceImpl serviceImpl;
   materia::InterprocessService<materia::InboxServiceImpl> service(serviceImpl);
   
   service.provideAt("InboxService");
   
   return 0;
}
