#pragma once
#include <string>
#include <mutex>
#include <fstream>

#define LOG(x) Logger::getInstance().Log(x);
#define LOG_WARNING(x) LOG(std::string("Warning: ") + x);

class Logger
{
public:
    static Logger& getInstance();
    void Log(const std::string& data);

private:
    std::mutex mMutex;
    std::ofstream mFile;
    Logger();
};