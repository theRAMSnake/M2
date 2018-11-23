#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/property_tree/ptree.hpp>
#include "TConnectedObject.hpp"
#include "../IStrategy.hpp"

namespace materia
{
namespace strategy
{

class Resource
{
public:
   typedef int TValue;

   boost::signals2::signal<void (const TValue& val)> OnValueChanged;
   boost::signals2::signal<void (const Resource& res)> OnChanged;

   Resource(const materia::Resource& props);
   Resource(const std::string& json);

   Id getId() const;

   void accept(const materia::Resource& props);
   const materia::Resource& getProps() const;

   std::string toJson() const;

private:

   materia::Resource mImpl;
};

}
}