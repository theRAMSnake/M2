#include "Password.hpp"
#include <fstream>
#include <streambuf>

namespace materia
{

std::string getPassword()
{
    std::ifstream t("/materia/passwd");
    std::string str((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());
    return str;
}

}