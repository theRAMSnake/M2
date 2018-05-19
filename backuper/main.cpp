#include <iostream>

#include <Client/MateriaClient.hpp>
#include <Client/IDatabase.hpp>
#include <Client/IContainer.hpp>
#include <Common/PortLayout.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

int main(int argc,  char** argv)
{
   materia::MateriaClient client("backuper", "188.116.57.62");

   auto docs = client.getDatabase().fetch();

   namespace pt = boost::posix_time;

   std::ofstream stream(pt::to_iso_string(pt::second_clock::local_time()));

   int entriesFetched = 0;
   for(auto x : docs)
   {
      boost::property_tree::ptree pt;

      pt.put ("id", x.id.getGuid());
      pt.put ("body", x.body);

      write_json (stream, pt, false);

      stream << std::endl;
      entriesFetched++;
   }

   stream.close();

   printf("Entries fetched: %d\n", entriesFetched);

   {
      auto ctr = client.getContainer().fetch();
      std::ofstream f(pt::to_iso_string(pt::second_clock::local_time()) + ".db", std::ios::out | std::ofstream::binary);
      std::copy(ctr.begin(), ctr.end(), std::ostreambuf_iterator<char>(f));
   }
}