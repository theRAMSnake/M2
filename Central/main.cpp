#include <iostream>
#include <messages/common.pb.h>
#include <Common/ServiceWrapper.hpp>
#include <zmq.hpp>
#include <cstdlib>
#include <iomanip>

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

        std::shared_ptr<zmq::socket_t> actionsSocket (new zmq::socket_t(context, ZMQ_DEALER));
        actionsSocket->connect("tcp://localhost:5912");
        mSockets.insert(std::make_pair("ActionsService", actionsSocket));
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

        std::stringstream stream;
        
        for(std::size_t i = 0; i < id.size(); ++i)
        {
            stream << std::hex << (int)((char*)id.data())[i];
        }

        materiaMsg.set_from(stream.str());

        std::vector<char> rawIdentity;
        rawIdentity.resize(id.size());
        memcpy(&rawIdentity.front(), id.data(), id.size());
        mClientInfos[stream.str()] = rawIdentity;


        doRouting(materiaMsg);
    }

    virtual std::vector<materia::IComponentInfoProvider::Info> getComponentInfos() const override
    {
        std::vector<materia::IComponentInfoProvider::Info> result;

        materia::IComponentInfoProvider::Info self = {true, "AdminService"};
        result.push_back(self);

        for(auto x : mSockets)
        {
            result.push_back({true, x.first});
        }

        return result;  
    }

    void fillPollSet(std::vector<zmq::pollitem_t>& pollItems)
    {
        for(auto x : mSockets)
        {
            pollItems.push_back({*x.second.get(), 0, ZMQ_POLLIN, 0});
        }
    }

    void receiveFrom(const int index, zmq::message_t& out)
    {
        auto iter = mSockets.begin();
        std::advance(iter, index);
        if(iter != mSockets.end())
        {
            zmq::socket_t& socket = *iter->second;
            do
            {
                socket.recv (&out);
            }
            while(out.more());
        }
    }

private:
    void doRouting(const common::MateriaMessage& materiaMsg)
    {
        std::cout << "Routing: " << materiaMsg.ShortDebugString() << std::endl;

        if(materiaMsg.to() == "AdminService")
        {
            auto msg = mAdminServiceProvider.sendMessage(materiaMsg);
            msg.set_from("AdminService");
            msg.set_to(materiaMsg.from());
            doRouting(msg);
        }
        else 
        {
            auto pos = mSockets.find(materiaMsg.to());
            if(pos != mSockets.end())
            {
                pos->second->send(zmq::message_t(), ZMQ_SNDMORE);

                zmq::message_t msgToSend (materiaMsg.ByteSizeLong());
                materiaMsg.SerializeToArray(msgToSend.data (), msgToSend.size());

                pos->second->send(msgToSend);
            }
            else //must be a client
            {
                auto clientId = mClientInfos.find(materiaMsg.to());
                if(clientId != mClientInfos.end())
                {
                    const std::vector<char>& rawIdentity = clientId->second;
                    zmq::message_t requestEndpoint(rawIdentity.size());               
                    memcpy(requestEndpoint.data(), &rawIdentity.front(), rawIdentity.size());
                    mClientSocket.send (requestEndpoint, ZMQ_SNDMORE);

                    zmq::message_t delimeterMessage;
                    mClientSocket.send (delimeterMessage, ZMQ_SNDMORE);

                    zmq::message_t msgToSend (materiaMsg.ByteSizeLong());
                    materiaMsg.SerializeToArray(msgToSend.data (), msgToSend.size());
                    mClientSocket.send (msgToSend);
                }
                else
                {
                    std::cout << "Routing error: unknown destination" << std::endl;
                }
            }
        }
    }

    zmq::socket_t& mClientSocket;
    materia::AdminServiceImpl mAdminServiceImpl;
    materia::ServiceWrapper<materia::AdminServiceImpl> mAdminServiceProvider;
    std::map<std::string, std::shared_ptr<zmq::socket_t>> mSockets;
    std::map<std::string, std::vector<char>> mClientInfos;
};

int main()
{
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
                zmq::message_t cmpMessage;
                connManager.receiveFrom(i - 1, cmpMessage);
                connManager.routeMessage(cmpMessage);
            }
        }
    }
    return 0;
}
