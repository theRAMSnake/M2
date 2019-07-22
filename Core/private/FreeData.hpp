#pragma once
#include "../IFreeData.hpp"
#include "Database.hpp"
#include "FreeDataInterpreter.hpp"

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
    bool checkExpression(const std::string& expr) override;

    const Interpreter& getInterpreter() const;

private:
    std::unique_ptr<DatabaseTable> mStorage;
    Interpreter mInterpreter;
};


}

}