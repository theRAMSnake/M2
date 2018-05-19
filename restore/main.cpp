#include <iostream>
#include <fstream>
#include <Client/MateriaClient.hpp>
#include <Client/IDatabase.hpp>
#include <Common/PortLayout.hpp>

#include <messages/database.pb.h>

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/range/algorithm/remove_if.hpp>
#include <boost/algorithm/string/classification.hpp>

int main(int argc,  char** argv)
{
   if(argc != 2)
   {
      std::cout << "Please specify bkacup file to restore\n" << std::endl;
      return -1;   
   }

   materia::MateriaClient client("restore");
   auto& service = client.getDatabase();

   std::string filename = argv[1];
   std::ifstream file(filename);
   std::string str;

   file.seekg(0, std::ios::end);   
   str.reserve(file.tellg());
   file.seekg(0, std::ios::beg);

   str.assign((std::istreambuf_iterator<char>(file)),
               std::istreambuf_iterator<char>());

   database::Documents src;

   //parse manually
   int entries = 0;

   std::cout << "\n";

   int headerPos = str.find("header {");
   while(headerPos != -1)
   {
      int keyStartPos = str.find("key: \"", headerPos) + 6; 
      int keyEndPos = str.find("\"", keyStartPos);

      std::string key = str.substr(keyStartPos, keyEndPos - keyStartPos);

      //std::cout << "key : " << key << "\n";

      int categoryStartPos = str.find("category: \"", headerPos) + 11; 
      int categoryEndPos = str.find("\"", categoryStartPos);

      std::string cat = str.substr(categoryStartPos, categoryEndPos - categoryStartPos);

      //std::cout << "category : " << cat << "\n";

      int bodyStartPos = str.find("body: \"", headerPos) + 7; 
      int bodyEndPos = str.find("}\"", bodyStartPos) + 1;

      std::string body = str.substr(bodyStartPos, bodyEndPos - bodyStartPos);

      body.erase(boost::remove_if(body, boost::is_any_of("\\")), body.end());
      std::cout << "body : " << body << "\n";

      service.setCategory(cat);
      auto id = service.insertDocument({key, body}, materia::IdMode::Provided);
            
      entries++;

      std::cout << id.getGuid() << "\n";

      headerPos = str.find("header {", bodyEndPos);
   }

   std::cout << "Num entries: " << entries << std::endl;
   std::cout << "Done\n";
}