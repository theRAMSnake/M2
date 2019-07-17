#pragma once
#include "../IFreeData.hpp"
#include "Database.hpp"

namespace materia
{

namespace freedata
{

class FreeData : public IFreeData
{
public:
    FreeData(Database& db);

    std::vector<DataBlock> get() override;

    void remove(const std::string& name) override;
    void set(const DataBlock& block) override;
    void increment(const std::string& name, const int value) override;

private:
    std::unique_ptr<DatabaseTable> mStorage;
};


}

}