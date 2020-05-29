#include <boost/python.hpp>

namespace materia
{

class ScriptRunner
{
public:
   ScriptRunner();

   bool exec(const std::string& code);

private:
   boost::python::object mMainModule;
};

}