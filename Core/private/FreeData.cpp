#include "FreeData.hpp"
#include "JsonSerializer.hpp"

BIND_JSON2(materia::DataBlock, name, value)

namespace materia
{

namespace freedata
{

FreeData::FreeData(Database& db)
: mStorage(db.getTable("freedata"))
{
    
}

std::vector<DataBlock> FreeData::get()
{
    std::vector<DataBlock> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {
        result.push_back(readJson<materia::DataBlock>(json));
    });

    return result;
}

void FreeData::remove(const std::string& name)
{
    mStorage->erase(name);
}

void FreeData::set(const DataBlock& block)
{
    mStorage->store(block.name, writeJson(block));
}

void FreeData::increment(const std::string& name, const int value)
{
    auto blocks = get();
    auto pos = std::find_if(blocks.begin(), blocks.end(), [&](auto x)->bool {return x.name == name;});
    if(pos != blocks.end())
    {
        pos->value += value;
        set(*pos);
    }
}

bool FreeData::checkExpression(const std::string& expr)
{
    return mInterpreter.compile(expr);
}

const Interpreter& FreeData::getInterpreter() const
{
    return mInterpreter;
}

}
}