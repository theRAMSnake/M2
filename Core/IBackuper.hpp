#pragma once
#include <string>
#include <vector>

namespace materia
{

class IBackuper
{
public:
    virtual void start() = 0;
    virtual bool getNextChunk(std::vector<char>& out) = 0;

    virtual ~IBackuper(){}
};

}