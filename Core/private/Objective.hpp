#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/property_tree/ptree.hpp>
#include "TConnectedObject.hpp"
#include "Resource.hpp"

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

   Id getId() const;

   void accept(const materia::Objective& props);

   const materia::Objective& getProps() const;

   void connect(Resource& res);
   void disconnect(const Resource& res);

   std::string toJson() const;

   ~Objective();

private:

   bool updateReached(const bool oldReached);
   void OnMeasValueChanged(const strategy::Resource::TValue value);

   boost::signals2::connection mMeasConnection;
   strategy::Resource::TValue mLastKnowMeasValue = 0;
   materia::Objective mImpl;
};

}
}