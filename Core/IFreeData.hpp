#pragma once
#include <string>
#include <vector>

namespace materia
{

struct DataBlock
{
    std::string name;
    int value;
};

class IFreeData
{
public:
    virtual std::vector<DataBlock> get() = 0;

    virtual void remove(const std::string& name) = 0;
    virtual void set(const DataBlock& block) = 0;
    virtual void increment(const std::string& name, const int value) = 0;

    virtual ~IFreeData(){}
};

}