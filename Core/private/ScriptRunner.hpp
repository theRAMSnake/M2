#pragma once
#include <exception>
#include <string>

namespace materia
{

class ObjectManager;

std::string runScript(const std::string& code, ObjectManager& om);

}
