#include <Wt/WServer.h>
#include "WebApp.hpp"

std::unique_ptr<Wt::WApplication> createApplication(const Wt::WEnvironment& env)
{
   return std::unique_ptr<Wt::WApplication>(new WebApp(env));
}

int main(int argc, char *argv[])
{
   Wt::WRun(argc, argv, &createApplication);
}