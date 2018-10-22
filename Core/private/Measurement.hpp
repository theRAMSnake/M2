#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/property_tree/ptree.hpp>
#include "TConnectedObject.hpp"
#include "../IStrategy.hpp"

namespace materia
{
namespace strategy
{

class Measurement
{
public:
   typedef int TValue;

   boost::signals2::signal<void (const TValue& val)> OnValueChanged;
   boost::signals2::signal<void (const Measurement& meas)> OnChanged;

   Measurement(const materia::Measurement& props);
   Measurement(const std::string& json);

   void accept(const materia::Measurement& props);
   const materia::Measurement& getProps() const;

private:
   std::string toJson() const;

   materia::Measurement mImpl;
};

}
}