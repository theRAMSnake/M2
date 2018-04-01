#include "Journal.hpp"

namespace materia
{

Journal::Journal(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

}