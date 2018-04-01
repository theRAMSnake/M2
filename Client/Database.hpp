#pragma once

#include "MateriaServiceProxy.hpp"
#include "Id.hpp"

#include "messages/database.pb.h"

#include <boost/optional.hpp>

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
   string key;
   string value;
   string value2;
   QueryElementType type;
};

enum class IdMode
{
   Provided,
   Generate
};

class Database
{
public:
   Database(materia::ZmqPbChannel& channel);

   void setCategory(const std::string& category);
   std::vector<Document> getDocuments();
   std::vector<Document> fetch();
   std::vector<Document> fts(const std::string& keyword);
   boost::optional<Document> getDocument(const Id& id);
   std::vector<Document> queryDocuments(const std::vector<QueryElement>& query);
   bool deleteDocument(const Id& id);
   bool replaceDocument(const Document& doc);
   Id insertDocument(const Document& doc, const IdMode idMode);

private:
   std::string mCategory;
   MateriaServiceProxy<database::DatabaseService> mProxy;
};

}