#include <iostream>
#include <messages/common.pb.h>
#include <Common/ServiceWrapper.hpp>
#include <zmq.hpp>
#include <cstdlib>

#include "AdminServiceImpl.hpp"

class ConnectionManager : public materia::IComponentInfoProvider
{
public:
    ConnectionManager(zmq::context_t& context, zmq::socket_t& clientSocket)
    : mClientSocket(clientSocket)
    , mAdminServiceImpl(*this)
    , mAdminServiceProvider(mAdminServiceImpl)
    {
        std::shared_ptr<zmq::socket_t> inboxSocket (new zmq::socket_t(context, ZMQ_DEALER));
        inboxSocket->connect("tcp://localhost:5911");
        mSockets.insert(std::make_pair("InboxService", inboxSocket));
    }

    void routeMessage(const zmq::message_t& msg)
    {
        common::MateriaMessage materiaMsg;
        materiaMsg.ParseFromArray(msg.data(), msg.size());

        doRouting(materiaMsg);
    }

    void routeClientMessage(const zmq::message_t& id, const zmq::message_t& msg)
    {
        common::MateriaMessage materiaMsg;
        materiaMsg.ParseFromArray(msg.data(), msg.size());

        std::string id_str;
        id_str.resize(id.size());
        memcpy(&id_str[0], id.data(), id.size());

        materiaMsg.set_from(id_str);

        doRouting(materiaMsg);
    }

    virtual std::vector<materia::IComponentInfoProvider::Info> getComponentInfos() const override
    {
        std::vector<materia::IComponentInfoProvider::Info> result;

        return result;  
    }

    void fillPollSet(std::vector<zmq::pollitem_t>& pollItems)
    {
        for(auto x : mSockets)
        {
            pollItems.push_back({*x.second, 0, ZMQ_POLLIN, 0});
        }
    }

private:
    void doRouting(const common::MateriaMessage& materiaMsg)
    {
        std::cout << "Routing: " << materiaMsg.ShortDebugString() << std::endl;

        if(materiaMsg.to() == "AdminService")
        {
            doRouting(mAdminServiceProvider.sendMessage(materiaMsg));
        }
        else 
        {
            auto pos = mSockets.find(materiaMsg.to());
            if(pos != mSockets.end())
            {
                pos->second->send(zmq::message_t());

                zmq::message_t msgToSend (materiaMsg.ByteSizeLong());
                materiaMsg.SerializeToArray(msgToSend.data (), msgToSend.size());

                pos->second->send(msgToSend);
            }
            else //must be a client
            {
                zmq::message_t requestEndpoint(materiaMsg.to().size());
                memcpy(requestEndpoint.data(), &materiaMsg.to().front(), materiaMsg.to().size());
                mClientSocket.send (requestEndpoint);

                zmq::message_t delimeterMessage;
                mClientSocket.send (delimeterMessage);

                zmq::message_t msgToSend (materiaMsg.ByteSizeLong());
                materiaMsg.SerializeToArray(msgToSend.data (), msgToSend.size());
                mClientSocket.send (msgToSend);
            }
        }
    }

    zmq::socket_t& mClientSocket;
    materia::AdminServiceImpl mAdminServiceImpl;
    materia::ServiceWrapper<materia::AdminServiceImpl> mAdminServiceProvider;
    std::map<std::string, std::shared_ptr<zmq::socket_t>> mSockets;
};

int main()
{
    std::system(("./m2InboxService"));

    zmq::context_t context (1);
    zmq::socket_t clientSocket (context, ZMQ_ROUTER);
    clientSocket.bind ("tcp://*:5910");

    std::vector<zmq::pollitem_t> pollItems;
    pollItems.push_back({clientSocket, 0, ZMQ_POLLIN, 0});

    ConnectionManager connManager(context, clientSocket);
    connManager.fillPollSet(pollItems);
    
    while(true)
    {
        zmq::poll(pollItems);

        if (pollItems [0].revents & ZMQ_POLLIN)
        {
            //message came to the router part - so we have it split by 3 parts
            zmq::message_t requestEndpoint;
            clientSocket.recv (&requestEndpoint);
            zmq::message_t delimeterMessage;
            clientSocket.recv (&delimeterMessage);
            zmq::message_t clientMessage;
            clientSocket.recv (&clientMessage);

            connManager.routeClientMessage(requestEndpoint, clientMessage);
        }

        for(std::size_t i = 1; i < pollItems.size(); ++i)
        {
            //this messages came from DEALER sockets, so have delimiters
            if (pollItems [i].revents & ZMQ_POLLIN)
            {
                zmq::message_t delimeterMessage;
                reinterpret_cast<zmq::socket_t*>(pollItems[i].socket)->recv (&delimeterMessage);
                zmq::message_t cmpMessage;
                clientSocket.recv (&cmpMessage);

                connManager.routeMessage(cmpMessage);
            }
        }
    }
    return 0;
}
