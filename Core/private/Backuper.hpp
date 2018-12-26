#pragma once
#include <Core/IBackuper.hpp>
#include <fstream>

namespace materia
{

class Backuper : public IBackuper
{
public:
    Backuper(const std::string& name);
    void start() override;
    bool getNextChunk(std::vector<char>& out) override;

private:
    const std::string mDbName;
    std::ifstream mStream;
};

}