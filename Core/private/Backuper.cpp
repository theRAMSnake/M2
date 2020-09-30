#include "Backuper.hpp"
#include "Logger.hpp"

namespace materia
{

Backuper::Backuper(const std::string& name)
: mDbName(name)
{
    LOG("Start bu init");
}

void Backuper::start()
{
    if(mStream.is_open())
    {
        mStream.close();
    }
    
    system(("sqlite3 " + mDbName + " \".backup '/materia/materia.back'\"").c_str());
    mStream.open("/materia/materia.back", std::ios::in | std::ios::binary);
}

bool Backuper::getNextChunk(std::vector<char>& out)
{
    bool hasMore = false;
    constexpr std::size_t chunkSize = 1024*1024;
    char buf[chunkSize];
    if(!mStream.eof())
    {
        mStream.read(buf, chunkSize);
        auto readSize = mStream.gcount();
        out.resize(readSize);
        std::copy(buf, buf + readSize, out.begin());

        hasMore = readSize == chunkSize;
    }

    if(!hasMore)
    {
        mStream.close();
    }

    return hasMore;
}

}