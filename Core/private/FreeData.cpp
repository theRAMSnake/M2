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

std::vector<std::pair<std::string, int>> FreeData::get()
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

}

void FreeData::set(const DataBlock& block)
{

}

void FreeData::increment(const std::string& name, const int value)
{

}

}
}