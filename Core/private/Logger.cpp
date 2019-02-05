#include "Logger.hpp"
#include <ctime>
#include <iomanip>

Logger& Logger::getInstance()
{
    static Logger l;
    return l;
}

void Logger::Log(const std::string& data)
{
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);

    mMutex.lock();
    mFile << '[' << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "]   " << data << std::endl;
    mFile.flush();
    mMutex.unlock();
}

Logger::Logger()
{
    mFile.open("m2core.log", std::ofstream::out | std::ofstream::trunc);
    mFile << std::endl << "-------------------------------------------------------------------------------" << std::endl;
}