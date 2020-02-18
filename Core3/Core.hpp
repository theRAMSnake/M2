#pragma once
#include "Database.hpp"
#include "Transport.hpp"
#include "Session.hpp"
#include "persistent/ObjectManager.hpp"

namespace materia3
{

struct CoreConfig
{
    std::string dbFileName;
};

class Core
{
public:
   Core(const CoreConfig& cfg);

   Session& createSession(const std::string& sessionName);

private:
   Database mDb;
   ObjectManager mOm;
   Transport mTransport;
};

}