#pragma once

#include "Id.hpp"

#include <optional>
#include <string>
#include <vector>

namespace materia
{

struct Document
{
   Id id;
   std::string body;
};

enum class QueryElementType
{
   Equals,
   Less,
   Greater,
   Between
};

struct QueryElement
{
   std::string key;
   std::string value;
   std::string value2;
   QueryElementType type;
};

enum class IdMode
{
   Provided,
   Generate
};

class IDatabase
{
public:

   virtual void setCategory(const std::string& category) = 0;
   virtual std::vector<Document> getDocuments() = 0;
   virtual std::vector<Document> fetch() = 0;
   virtual std::vector<Document> fts(const std::string& keyword) = 0;
   virtual std::optional<Document> getDocument(const Id& id) = 0;
   virtual std::vector<Document> queryDocuments(const std::vector<QueryElement>& query) = 0;
   virtual bool deleteDocument(const Id& id) = 0;
   virtual bool replaceDocument(const Document& doc) = 0;
   virtual Id insertDocument(const Document& doc, const IdMode idMode) = 0;

   virtual ~IDatabase(){}
};

}