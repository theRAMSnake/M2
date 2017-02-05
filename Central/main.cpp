#include <iostream>
#include <messages/common.pb.h>
#include <zmq.hpp>

common::MateriaMessage routeMessage(const common::MateriaMessage& msg)
{
   common::MateriaMessage result = msg;
   result.set_from("worker");
   return result;
}

void *worker_routine (void *arg)
{
    zmq::context_t *context = (zmq::context_t *) arg;

    zmq::socket_t socket (*context, ZMQ_REP);
    socket.connect ("inproc://workers");

    while (true) 
    {
        //  Wait for next request from client
        zmq::message_t request;
        socket.recv (&request);
        
        common::MateriaMessage requestMsg;
        requestMsg.ParseFromArray(request.data(), request.size());
        
        common::MateriaMessage replyMsg = routeMessage(requestMsg);
        
        zmq::message_t responce (replyMsg.ByteSizeLong());
        replyMsg.SerializeToArray(responce.data (), responce.size());
      
        socket.send (responce);
    }
    return (NULL);
}

int main()
{
   //  Prepare our context and sockets
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
    //  Connect work threads to client threads via a queue
    zmq::proxy (clients, workers, NULL);
    return 0;
}
