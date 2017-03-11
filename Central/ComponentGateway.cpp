#include "ComponentGateway.hpp"

namespace materia
{
   
common::MateriaMessage ComponentGateway::routeMessage(const common::MateriaMessage& msgToRoute)
{
   auto iter = mConnections.find(msgToRoute.to());
   if(iter == mConnections.end())
   {
      throw CannotRouteException();
   }
   
   return iter->second->sendMessage(msgToRoute);
}

void ComponentGateway::addConnection(const std::string& componentName, IComponentConnection* connection)
{
   mConnections.insert(std::make_pair(componentName, boost::shared_ptr<IComponentConnection>(connection)));
}

std::vector<ComponentGateway::Info> ComponentGateway::getComponentInfos() const
{
   std::vector<ComponentGateway::Info> result;
   
   for(auto x : mConnections)
   {
      Info item {x.second->isConnected(), x.second->getName()};
      result.push_back(item);
   }
   
   return result;
}

}
