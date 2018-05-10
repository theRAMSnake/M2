#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"

#include "messages/container.pb.h"

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <functional>

namespace materia
{

class Container : public IContainer
{
public:
   Container(materia::ZmqPbChannel& channel);

   bool addContainer(const ContainerDefinition& def) override;
   std::vector<ContainerDefinition> getPublicContainers() override;
   bool clearContainer(const std::string& name) override;
   bool deleteContainer(const std::string& name) override;

   boost::optional<int> execFunc(const Func& func) override;

   std::vector<ContainerItem> getItems(const std::string& containerName) override;
   std::vector<Id> insertItems(const std::string& containerName, const std::vector<ContainerItem>& items) override;
   bool replaceItems(const std::string& containerName, const std::vector<ContainerItem>& items) override;
   bool deleteItems(const std::string& containerName, const std::vector<Id>& ids) override;

   std::vector<char> fetch() override;

   ContainerSlot acquireSlot(const std::string& containerName) override;
   ContainerSlot acquireSlot(const std::string& containerName, const Id& slotId) override;

private:
   MateriaServiceProxy<container::ContainerService> mProxy;
};

}