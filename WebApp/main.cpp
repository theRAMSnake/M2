#include <Wt/WServer>
#include "WebApp.hpp"

Wt::WApplication *createApplication(const Wt::WEnvironment& env)
{
   return new WebApp(env);
}

int main(int argc, char *argv[])
{
   Wt::WRun(argc, argv, &createApplication);
}