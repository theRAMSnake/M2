#pragma once

#include "MateriaServiceProxy.hpp"
#include "../Id.hpp"

#include "messages/journal.pb.h"

#include <boost/optional.hpp>

namespace materia
{

class Journal
{
public:
   Journal(materia::ZmqPbChannel& channel);

private:
   MateriaServiceProxy<journal::JournalService> mProxy;
};

}