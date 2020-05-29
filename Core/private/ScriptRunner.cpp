#include "ScriptRunner.hpp"
#include <iostream>

namespace materia
{

ScriptRunner::ScriptRunner()
{
   Py_Initialize();
   mMainModule = boost::python::import("__main__");
}

bool ScriptRunner::exec(const std::string& code)
{
   try
   {
      boost::python::dict main_namespace = boost::python::extract<boost::python::dict>(mMainModule.attr("__dict__"));
      main_namespace.clear();
      boost::python::object result = boost::python::exec(code.c_str(), main_namespace);

      return true;
   }
   catch(...)
   {
      PyErr_Print();

      return false;
   }
}

}