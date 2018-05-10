#pragma once

#include "MateriaServiceProxy.hpp"
#include "../IDatabase.hpp"

#include "messages/database.pb.h"

#include <boost/optional.hpp>

namespace materia
{

class Database : public IDatabase
{
public:
   Database(materia::ZmqPbChannel& channel);

   void setCategory(const std::string& category) override;
   std::vector<Document> getDocuments() override;
   std::vector<Document> fetch() override;
   std::vector<Document> fts(const std::string& keyword) override;
   std::optional<Document> getDocument(const Id& id) override;
   std::vector<Document> queryDocuments(const std::vector<QueryElement>& query) override;
   bool deleteDocument(const Id& id) override;
   bool replaceDocument(const Document& doc) override;
   Id insertDocument(const Document& doc, const IdMode idMode) override;

private:
   std::string mCategory;
   MateriaServiceProxy<database::DatabaseService> mProxy;
};

}