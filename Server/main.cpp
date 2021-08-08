#include <iostream>
#include <zmq.hpp>
#include <cstdlib>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <thread>
#include <mutex>
#include <map>
#include <fmt/format.h>
#include "Common/Codec.hpp"
#include "Common/Password.hpp"
#include "Common/Base64.hpp"
#include "Core/ICore3.hpp"
#include <served/served.hpp>

std::string string_to_hex(const std::string& input)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    std::string output;
    output.reserve(input.length() * 2);
    for (unsigned char c : input)
    {
        output.push_back(hex_digits[c >> 4]);
        output.push_back(hex_digits[c & 15]);
    }
    return output;
}

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

DoubleLogger logger("m3server.log");
std::mutex gMainMutex;
static bool shutdownFlag = false;

void timerFunc(materia::ICore3* core)
{
    int cicleCooldown = 0;
    
    while(true)
    {
        if(shutdownFlag)
        {
            return;
        }

        auto t = std::time(NULL);
        auto tm_struct = localtime(&t);

        if(cicleCooldown > 0)
        {
            cicleCooldown--;
        }

        if(tm_struct->tm_hour == 3 && tm_struct->tm_min == 0 && cicleCooldown == 0)
        {
            std::unique_lock<std::mutex> lock(gMainMutex);
            core->onNewDay(boost::gregorian::day_clock::local_day());

            t = std::time(NULL);
            tm_struct = localtime(&t);

            if(tm_struct->tm_wday == 1 /*Monday*/)
            {
                core->onNewWeek();
            }

            cicleCooldown = 3600;//Make sure we will never hit in at least one hour

            core->executeCommandJson("{\"operation\":\"push\", \"params\":{\"listId\": \"inbox\", \"value\": \"Core daily updated.\"}}");
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}


void newFunc(std::string password, materia::ICore3* core)
{
    Codec codec(password);

    zmq::context_t context (1);
    zmq::socket_t clientSocket (context, ZMQ_REP);
    clientSocket.bind ("tcp://*:5756");

    while(true)
    {
        zmq::message_t clientMessage;
        (void)clientSocket.recv (clientMessage, zmq::recv_flags::none);

        std::string received(static_cast<const char *>(clientMessage.data()), clientMessage.size());
        std::string decoded;
        
        try
        {
            decoded = codec.decrypt(received);
        }
        catch(...)
        {
            logger << "Decription failed\n";
            logger << "against " << string_to_hex(codec.encrypt("test")) << "\n";
            clientSocket.send (clientMessage, zmq::send_flags::none);
            continue;
        }
        
        logger << "In: " << decoded << "\n";

        std::string result;
        if(decoded == "shutdown")
        {
            result = "shuting down";
            shutdownFlag = true;
        }
        else
        {
            std::unique_lock<std::mutex> lock(gMainMutex);
            result = core->executeCommandJson(decoded);
        }

        logger << "Out: " << result << "\n";

        std::string encoded = codec.encrypt(result);

        zmq::message_t msgToSend (encoded.data(), encoded.size());
        clientSocket.send (msgToSend, zmq::send_flags::none);

        if(shutdownFlag)
        {
            break;
        }
    }
}

void newFunc2(std::string password, materia::ICore3* core)
{
    try
    {
        Codec codec(password);
        served::net::server* server = nullptr; 
        served::multiplexer mux;

        mux.handle("/api")
          .post([&codec, core, server](served::response & res, const served::request & req) {
                std::string decoded;

                std::string toSend = "";

                try
                {
                    decoded = codec.decrypt(base64_decode(req.body()));
                }
                catch(...)
                {
                    logger << "Decription failed\n";
                    toSend = req.body();
                }

                logger << "In: " << decoded << "\n";

                std::string result;
                if(decoded == "shutdown")
                {
                    toSend = "shuting down";
                    shutdownFlag = true;
                }
                else
                {
                    std::unique_lock<std::mutex> lock(gMainMutex);
                    toSend = core->executeCommandJson(decoded);
                }

                logger << "Out: " << toSend << "\n";

                toSend = base64_encode(codec.encrypt(toSend));

                if(shutdownFlag)
                {
                    server->stop();
                }

                res << toSend;    
		});

        served::net::server srv("0.0.0.0", "5754", mux);
        server = &srv;
        srv.run(4);
    }
    catch(std::exception& e)
    {
        logger << "Server has crashed with an error: " << e.what();
    }
    catch(...)
    {
        logger << "Server has crashed for unknown reason";
    }
}

int main(int argc, char *argv[])
{
    std::string password = materia::getPassword();

    auto core = materia::createCore({"/materia/materia.db"});

    std::thread webThread(&newFunc, password, core.get());
    std::thread webThread2(&newFunc2, password, core.get());
    std::thread timerThread(&timerFunc, core.get());
    
    webThread.join();
    webThread2.join();
    timerThread.join();

    return 0;
}
