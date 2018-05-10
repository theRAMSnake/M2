#include "Database.hpp"

namespace materia
{

Database::Database(materia::ZmqPbChannel& channel)
: mProxy(channel)
{

}

void Database::setCategory(const std::string& category)
{
   mCategory = category;
}

std::vector<Document> Database::getDocuments()
{
   return queryDocuments({});
}

database::QueryElementType toProto(const QueryElementType src)
{
   switch(src)
   {
   case QueryElementType::Less:
      return database::QueryElementType::Less;

   case QueryElementType::Greater:
      return database::QueryElementType::Greater;

   case QueryElementType::Equals:
      return database::QueryElementType::Equals;

   case QueryElementType::Between:
      return database::QueryElementType::Between;

   default:
      throw -1;
   }
}

Document convert(database::Document d)
{
   return {d.header().key(), d.body()};
}

std::vector<Document> Database::queryDocuments(const std::vector<QueryElement>& query)
{
   database::DocumentQuery request;
   request.set_category(mCategory);

   for(auto q : query)
   {
      auto kvl = request.add_query();
      kvl->set_key(q.key);
      kvl->set_value(q.value);
      kvl->set_value2(q.value2);
      kvl->set_type(toProto(q.type));
   }

   database::Documents result;
   mProxy.getService().SearchDocuments(nullptr, &request, &result, nullptr);

   std::vector<Document> docs(result.result_size());
   std::transform(result.result().begin(), result.result().end(), docs.begin(), [] (auto x)-> auto { return convert(x); });

   return docs;
}

boost::optional<Document> Database::getDocument(const Id& id)
{
   boost::optional<Document> resultDoc;

   database::DocumentHeader header;
   header.set_category(mCategory);
   header.set_key(id.getGuid());

   database::Documents result;
   mProxy.getService().GetDocument(nullptr, &header, &result, nullptr);

   if(result.result().size() > 0)
   {
      auto d = result.result()[0];
      resultDoc.reset(convert(d));
   }

   return resultDoc;
}

bool Database::deleteDocument(const Id& id)
{
   database::DocumentHeader header;
   header.set_category(mCategory);
   header.set_key(id.getGuid());

   common::OperationResultMessage result;
   mProxy.getService().DeleteDocument(nullptr, &header, &result, nullptr);

   return result.success();
}

bool Database::replaceDocument(const Document& doc)
{
   database::Document document;
   document.mutable_header()->set_category(mCategory);
   document.mutable_header()->set_key(doc.id.getGuid());
   document.set_body(doc.body);

   common::OperationResultMessage result;
   mProxy.getService().ModifyDocument(nullptr, &document, &result, nullptr);

   return result.success();
}

Id Database::insertDocument(const Document& doc, const IdMode idMode)
{
   database::Document document;
   document.mutable_header()->set_category(mCategory);
   document.mutable_header()->set_key(doc.id.getGuid());
   document.set_body(doc.body);

   database::AddDocumentRequest request;
   request.mutable_doc()->CopyFrom(document);
   request.set_useprovidedid(idMode == IdMode::Provided);

   common::UniqueId result;
   mProxy.getService().AddDocument(nullptr, &request, &result, nullptr);

   return result;
}

std::vector<Document> Database::fetch()
{
   common::EmptyMessage message;
   database::Documents result;
   mProxy.getService().Fetch(nullptr, &message, &result, nullptr);

   std::vector<Document> docs(result.result_size());
   std::transform(result.result().begin(), result.result().end(), docs.begin(), [] (auto x)-> auto { return convert(x); });

   return docs;
}

std::vector<Document> Database::fts(const std::string& keyword)
{
   database::FullTextSearchParameters in;
   in.set_fulltext(keyword);

   database::FullTextSearchResult out;
   mProxy.getService().FullTextSearch(nullptr, &in, &out, nullptr);

   std::vector<Document> docs(out.result_size());
   std::transform(out.result().begin(), out.result().end(), docs.begin(), [] (auto x)-> auto { return convert(x.doc()); });

   return docs;
}

}