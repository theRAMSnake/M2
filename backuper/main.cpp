#include <iostream>

#include <Common/ZmqPbChannel.hpp>
#include <Common/PortLayout.hpp>

#include <messages/database.pb.h>

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

template<class TService>
class MateriaServiceProvider
{
public:
   MateriaServiceProvider(const std::string& address)
   : mContext(1)
   , mSocket(mContext, ZMQ_REQ)
   , mChannel(mSocket, "Legacy")
   , mService(&mChannel)
   {
      mSocket.connect("tcp://" + address + ":" + gCentralPort);
   }
   
   typename TService::Stub& getService()
   {
      return mService;
   }
   
private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   materia::ZmqPbChannel mChannel;
   typename TService::Stub mService;
};

int main(int argc,  char** argv)
{
   MateriaServiceProvider<database::DatabaseService> pr("188.116.57.62");
   auto& service = pr.getService();

   common::EmptyMessage msg;
   database::Documents result;
   service.Fetch(nullptr, &msg, &result, nullptr);

   namespace pt = boost::posix_time;

   std::ofstream stream(pt::to_iso_string(pt::second_clock::local_time()));

   int entriesFetched = 0;
   for(auto x : result.result())
   {
      stream << x.DebugString() << std::endl;
      entriesFetched++;
   }

   stream.close();

   printf("Entries fetched: %d\n", entriesFetched);
}