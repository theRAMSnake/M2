#pragma once

#include "Id.hpp"
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

class ContainerSlot
{
public:
   typedef std::function<Id(const ContainerItem& item)> TCreateImpl;
   typedef std::function<void(const ContainerItem& item)> TUpdateImpl;
   typedef std::function<void(const Id& id)> TEraseImpl;

   ContainerSlot(TCreateImpl create, TUpdateImpl update, TEraseImpl erase, const Id& id = Id::Invalid);
   ContainerSlot(ContainerSlot&& other);
   ContainerSlot(const ContainerSlot& other) = delete;
   void operator = (const ContainerSlot& other) = delete;
   void put(const std::string& content);
   ~ContainerSlot();

private:
   TCreateImpl mCreate;
   TUpdateImpl mUpdate;
   TEraseImpl mErase;

   Id mId;
};

class IContainer
{
public:
   virtual bool addContainer(const ContainerDefinition& def) = 0;
   virtual std::vector<ContainerDefinition> getPublicContainers() = 0;
   virtual bool clearContainer(const std::string& name) = 0;
   virtual bool deleteContainer(const std::string& name) = 0;

   virtual std::optional<int> execFunc(const Func& func) = 0;

   virtual std::vector<ContainerItem> getItems(const std::string& containerName) = 0;
   virtual std::vector<Id> insertItems(const std::string& containerName, const std::vector<ContainerItem>& items) = 0;
   virtual bool replaceItems(const std::string& containerName, const std::vector<ContainerItem>& items) = 0;
   virtual bool deleteItems(const std::string& containerName, const std::vector<Id>& ids) = 0;

   virtual std::vector<char> fetch() = 0;

   virtual ContainerSlot acquireSlot(const std::string& containerName) = 0;
   virtual ContainerSlot acquireSlot(const std::string& containerName, const Id& slotId) = 0;

   virtual ~IContainer(){}
};

}