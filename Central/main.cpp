#include <iostream>
#include <messages/common.pb.h>
#include <Common/ServiceWrapper.hpp>
#include <zmq.hpp>
#include <cstdlib>

#include "ComponentGateway.hpp"
#include "AdminServiceImpl.hpp"

class LocalProcessConnection : public materia::IComponentConnection
{
public:
   typedef std::function<common::MateriaMessage(const common::MateriaMessage&)> TRedirectMessage;
   LocalProcessConnection(const std::string& name, TRedirectMessage redirectTo)
   : mRedirectTo(redirectTo)
   , mName(name)
   {
   }
   
   virtual common::MateriaMessage sendMessage(const common::MateriaMessage& msg) override
   {
      return mRedirectTo(msg);
   }
   
   virtual std::string getName() const override
   {
      return mName;
   }
   
   virtual bool isConnected() const override
   {
      return true;
   }
   
private:
   TRedirectMessage mRedirectTo;
   std::string mName;
};

class TimeoutException : public std::exception{};

class RemoteProcessConnection : public materia::IComponentConnection
{
public:
   RemoteProcessConnection(const std::string& name)
   : mContext(1)
   , mSocket(mContext, ZMQ_REQ)
   , mName(name)
   {    
      mSocket.connect((std::string("ipc://") + name).c_str());
   }
   
   virtual common::MateriaMessage sendMessage(const common::MateriaMessage& msg) override
   {     
      zmq::message_t req (msg.ByteSizeLong());
      msg.SerializeToArray(req.data (), req.size());
      mSocket.send(req);
   
      zmq::message_t resp;
      mSocket.recv(&resp);
      
      common::MateriaMessage result;
      result.ParseFromArray(resp.data(), resp.size());
      
      return result;
   }
   
   virtual std::string getName() const override
   {
      return mName;
   }
   
   virtual bool isConnected() const override
   {
      return mSocket.connected();
   }
   
private:
   zmq::context_t mContext;
   zmq::socket_t mSocket;
   std::string mName;
};

void *worker_routine (void *arg)
{
   using namespace std::placeholders;
   
    zmq::context_t *context = (zmq::context_t *) arg;

    zmq::socket_t socket (*context, ZMQ_REP);
    socket.connect ("inproc://workers");
    
    materia::ComponentGateway componentGateway;
    
    componentGateway.addConnection("InboxService", new RemoteProcessConnection("InboxService"));
    
    materia::AdminServiceImpl adminServiceImpl(componentGateway);
    materia::ServiceWrapper<materia::AdminServiceImpl> adminServiceProvider(adminServiceImpl);
    componentGateway.addConnection("AdminService", new LocalProcessConnection(
       "AdminService",
       std::bind(&materia::ServiceWrapper<materia::AdminServiceImpl>::sendMessage, &adminServiceProvider, _1)));

    while (true) 
    {
        //  Wait for next request from client
        zmq::message_t request;
        socket.recv (&request);
        
        common::MateriaMessage requestMsg;
        requestMsg.ParseFromArray(request.data(), request.size());
        
        std::cout << "Received: " << requestMsg.ShortDebugString() << std::endl;
        
        common::MateriaMessage replyMsg;
        try
        {
           replyMsg = componentGateway.routeMessage(requestMsg);
        }
        catch(materia::CannotRouteException& ex)
        {
           replyMsg.set_from("central");
           replyMsg.set_error("Unable to route message: " + requestMsg.ShortDebugString());
        }
        catch(TimeoutException& ex)
        {
           replyMsg.set_from("central");
           replyMsg.set_error("No responce from service");
        }
        
        zmq::message_t responce (replyMsg.ByteSizeLong());
        replyMsg.SerializeToArray(responce.data (), responce.size());
        
        std::cout << "Sent: " << replyMsg.ShortDebugString() << std::endl;
      
        socket.send (responce);
    }
    return (NULL);
}

int main()
{
    zmq::context_t context (1);
    zmq::socket_t clients (context, ZMQ_ROUTER);
    clients.bind ("tcp://*:5000");
    zmq::socket_t workers (context, ZMQ_DEALER);
    workers.bind ("inproc://workers");

    //  Launch pool of worker threads
    for (int thread_nbr = 0; thread_nbr != 4; thread_nbr++) 
    {
        pthread_t worker;
        pthread_create (&worker, NULL, worker_routine, (void *) &context);
    }
    
    std::system(("./m2InboxService"));
    
    //  Connect work threads to client threads via a queue
    zmq::proxy (clients, workers, NULL);
    return 0;
}
