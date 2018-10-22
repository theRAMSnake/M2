#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/property_tree/ptree.hpp>
#include "TConnectedObject.hpp"
#include "Measurement.hpp"

namespace materia
{
namespace strategy
{

class Objective
{
public:
   boost::signals2::signal<void (const bool val)> OnReachedChanged;
   boost::signals2::signal<void (const Objective&)> OnChanged;

   Objective(const materia::Objective& props);
   Objective(const std::string& json);

   void accept(const materia::Objective& props);

   const materia::Objective& getProps() const;

   void connect(Measurement& meas);
   void disconnect(const Measurement& meas);

   ~Objective();

private:
   std::string toJson() const;

   bool updateReached(const bool oldReached);
   void OnMeasValueChanged(const strategy::Measurement::TValue value);

   boost::signals2::connection mMeasConnection;
   strategy::Measurement::TValue mLastKnowMeasValue = 0;
   materia::Objective mImpl;
};

}
}