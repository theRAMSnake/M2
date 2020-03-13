#include <iostream>
#include <zmq.hpp>
#include <cstdlib>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <messages/common.pb.h>
#include "ServiceWrapper.hpp"
#include "InboxServiceImpl.hpp"
#include "CalendarServiceImpl.hpp"
#include "StrategyServiceImpl.hpp"
#include "JournalServiceImpl.hpp"
#include "AdminServiceImpl.hpp"
#include "RewardServiceImpl.hpp"
#include "FreeDataServiceImpl.hpp"
#include "FinanceServiceImpl.hpp"
#include "Common/Codec.hpp"

class DoubleLogger
{
public:
    DoubleLogger(const std::string& fileName)
    : mFile(fileName.c_str(), std::ofstream::out | std::ofstream::trunc)
    {
    }

    ~DoubleLogger()
    {
        mFile.close();
    }

    template<class T>
    DoubleLogger& operator << (const T& val)
    {
        std::cout << val;
        mFile << val;
        mFile.flush();

        return *this;
    }

private:
    std::ofstream mFile;
};

DoubleLogger logger("m2server.log");

std::map<std::string, std::shared_ptr<materia::IService>> gServices;

common::MateriaMessage handleMessage(const common::MateriaMessage& in)
{
    std::chrono::time_point<std::chrono::high_resolution_clock> started =
        std::chrono::high_resolution_clock::now();

    try
    {
        auto pos = gServices.find(in.to());
        if(pos != gServices.end())
        {
            auto result = pos->second->handleMessage(in);
            auto time_D_msec = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - started);

            logger << "Handled: " << in.to() << "::" << in.operationname() << " (" << std::to_string(time_D_msec.count()) << "ms)\n";

            return result;
        }
        else
        {
            logger << "Target service not found: " << in.to() << "\n";
            logger << "Details: " << in.DebugString() << "\n";
        }
    }
    catch(std::exception& ex)
    {
        logger << ex.what() << "\n";
    }

    common::MateriaMessage errorMsg;

    errorMsg.set_from("materia");
    errorMsg.set_to(in.from());
    errorMsg.set_error("Failed to handle message");

    return errorMsg;
}

static bool shutdownFlag = false;

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
       std::cout << "Please specify password";
       return -1;
    }

    std::string password = argv[1];
    Codec codec(password);

    zmq::context_t context (1);
    zmq::socket_t clientSocket (context, ZMQ_REP);
    clientSocket.bind ("tcp://*:5757");

    logger << "Creating core\n";
    auto core = materia::createCore({"/materia/materia.db"});

    logger << "Creating services\n";

    gServices.insert({"InboxService", std::make_shared<materia::ServiceWrapper<materia::InboxServiceImpl>>((*core))});
    gServices.insert({"RewardService", std::make_shared<materia::ServiceWrapper<materia::RewardServiceImpl>>((*core))});
    gServices.insert({"CalendarService", std::make_shared<materia::ServiceWrapper<materia::CalendarServiceImpl>>((*core))});
    gServices.insert({"JournalService", std::make_shared<materia::ServiceWrapper<materia::JournalServiceImpl>>((*core))});
    gServices.insert({"StrategyService", std::make_shared<materia::ServiceWrapper<materia::StrategyServiceImpl>>((*core))});
    gServices.insert({"FreeDataService", std::make_shared<materia::ServiceWrapper<materia::FreeDataServiceImpl>>((*core))});
    gServices.insert({"FinanceService", std::make_shared<materia::ServiceWrapper<materia::FinanceServiceImpl>>((*core))});
    gServices.insert({"AdminService", std::make_shared<materia::ServiceWrapper<materia::AdminServiceImpl>>(*core, shutdownFlag)});
    
    logger << "Start listening\n";
    while(true)
    {
        zmq::message_t clientMessage;
        clientSocket.recv (clientMessage, zmq::recv_flags::none);
        logger << "Received message\n";

        std::string received(static_cast<const char *>(clientMessage.data()), clientMessage.size());
        std::string decoded;
        
        try
        {
            decoded = codec.decrypt(received);
        }
        catch(...)
        {
            logger << "Decription failed\n";
            clientSocket.send (clientMessage, zmq::send_flags::none);
            continue;
        }

        common::MateriaMessage materiaMsg;
        if(!materiaMsg.ParseFromString(decoded))
        {
            logger << "Cannot parse message, sending it back\n";
            clientSocket.send (clientMessage, zmq::send_flags::none);
            continue;
        }

        auto resp = handleMessage(materiaMsg);
        std::string serialized;
        resp.SerializeToString(&serialized);
        std::string encoded = codec.encrypt(serialized);

        zmq::message_t msgToSend (encoded.data(), encoded.size());
        clientSocket.send (msgToSend, zmq::send_flags::none);
        logger << "Sending responce\n";

        if(shutdownFlag)
        {
            return 0;
        }
    }
    return 0;
}
