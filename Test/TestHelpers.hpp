#pragma once

#include <iostream>
#include <Common/Id.hpp>

namespace std
{

std::ostream& operator << (std::ostream& str, const materia::Id& id);

}
