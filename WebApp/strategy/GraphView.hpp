#pragma once

#include <Wt/WCompositeWidget.h>
#include <Wt/WEvent.h>
#include "../materia/StrategyModel.hpp"
#include <boost/signals2/signal.hpp>

class GraphView: public Wt::WCompositeWidget
{
public:
   boost::signals2::signal<void(Wt::WMouseEvent)> OnCaptionClicked;
   boost::signals2::signal<void(StrategyModel::Node, Wt::WMouseEvent)> OnNodeClicked;

   GraphView();

   void reset();
   void assign(const StrategyModel::Graph& g);
   void setCaption(const std::string& caption);
   void layoutNodes(const std::vector<StrategyModel::Node>& nodes);

private:
   Wt::WContainerWidget* mImpl;

   Wt::WLabel* mName;
};