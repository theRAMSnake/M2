#pragma once
#include <boost/shared_ptr.hpp>
#include <messages/common.pb.h>
#include "AdminServiceImpl.hpp"

namespace materia
{
   
class CannotRouteException : public std::exception{};
   
class IComponentConnection
{
public:
   virtual common::MateriaMessage sendMessage(const common::MateriaMessage& msgToRoute) = 0;
   virtual std::string getName() const = 0;
   virtual bool isConnected() const = 0;
   
   virtual ~IComponentConnection() {}
};
   
class ComponentGateway : public IComponentInfoProvider
{
public:
   common::MateriaMessage routeMessage(const common::MateriaMessage& msgToRoute);
   void addConnection(const std::string& componentName, IComponentConnection* connection);
   
   virtual std::vector<Info> getComponentInfos() const override;
   
private:
   std::map<std::string, boost::shared_ptr<IComponentConnection>> mConnections;
};

}
