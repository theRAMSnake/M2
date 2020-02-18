#include "Core.hpp"

namespace materia3
{

Core::Core(const CoreConfig& cfg)
: mDb(cfg.dbFileName)
, mOm(mDb)
, mTransport(mOm)
{
   setTransport(mTransport);
}

Session& Core::createSession(const std::string& sessionName)
{
   return mOm.addTemporaryObject<Session>(sessionName);
}

}