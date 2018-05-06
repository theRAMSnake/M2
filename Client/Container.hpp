#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"

#include "messages/container.pb.h"

#include <boost/optional.hpp>
#include <boost/noncopyable.hpp>
#include <functional>

namespace materia
{

struct ContainerDefinition
{
   std::string name;
   bool isPublic;
   Id iconId;

   bool operator == (const ContainerDefinition& other) const;
};

struct ContainerItem
{
   Id id;
   std::string content;
   std::vector<char> blob;

   bool operator == (const ContainerItem& other) const;
   bool operator != (const ContainerItem& other) const;
};

enum class FuncType
{
   Average,
   Sum,
   Count
};

struct Func
{
    FuncType funcType;
    std::string containerName;
};

class ContainerSlot : boost::noncopyable
{
public:
   typedef std::function<Id(const ContainerItem& item)> TCreateImpl;
   typedef std::function<void(const ContainerItem& item)> TUpdateImpl;
   typedef std::function<void(const Id& id)> TEraseImpl;

   ContainerSlot(TCreateImpl create, TUpdateImpl update, TEraseImpl erase, const Id& id = Id::Invalid);
   ContainerSlot(ContainerSlot&& other);
   void put(const std::string& content);
   ~ContainerSlot();

private:
   TCreateImpl mCreate;
   TUpdateImpl mUpdate;
   TEraseImpl mErase;

   Id mId;
};

class Container
{
public:
   Container(materia::ZmqPbChannel& channel);

   bool addContainer(const ContainerDefinition& def);
   std::vector<ContainerDefinition> getPublicContainers();
   bool clearContainer(const std::string& name);
   bool deleteContainer(const std::string& name);

   boost::optional<int> execFunc(const Func& func);

   std::vector<ContainerItem> getItems(const std::string& containerName);
   std::vector<Id> insertItems(const std::string& containerName, const std::vector<ContainerItem>& items);
   bool replaceItems(const std::string& containerName, const std::vector<ContainerItem>& items);
   bool deleteItems(const std::string& containerName, const std::vector<Id>& ids);

   std::vector<char> fetch();

   ContainerSlot acquireSlot(const std::string& containerName);
   ContainerSlot acquireSlot(const std::string& containerName, const Id& slotId);

private:
   MateriaServiceProxy<container::ContainerService> mProxy;
};

}