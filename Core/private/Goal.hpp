#pragma once
#include <boost/signals2/signal.hpp>
#include <boost/property_tree/ptree.hpp>
#include "TConnectedObject.hpp"
#include "Objective.hpp"

namespace materia
{
namespace strategy
{

class Goal
{
public:
   boost::signals2::signal<void ()> OnAchievedChanged;
   boost::signals2::signal<void (const Goal&)> OnChanged;

   Goal(const materia::Goal& props);
   Goal(const std::string& json);

   Id getId() const;

   void accept(const materia::Goal& props);

   const materia::Goal& getProps() const;

   void connect(const std::shared_ptr<Objective>& obj);
   void disconnect(const std::shared_ptr<Objective>& obj);

   void OnObjReachedChanged();
   
   std::vector<Id> getObjectives();

   std::string toJson() const;

private:
   
   void UpdateAndSaveAchieved();
   bool updateAchieved();
   bool calculateAchieved();

   std::map<Id, std::shared_ptr<ConnectedObject<std::shared_ptr<Objective>, boost::signals2::connection>>> mObjectives;
   materia::Goal mImpl;
};

}
}