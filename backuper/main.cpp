#include <iostream>

#include <Client/MateriaClient.hpp>
#include <Client/IContainer.hpp>
#include <Common/PortLayout.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>

int main(int argc,  char** argv)
{
   materia::MateriaClient client("backuper", "188.116.57.62");

   {
      auto ctr = client.getContainer().fetch();
      std::ofstream f(boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) + ".db", std::ios::out | std::ofstream::binary);
      std::copy(ctr.begin(), ctr.end(), std::ostreambuf_iterator<char>(f));
   }
}