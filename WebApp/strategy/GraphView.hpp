#pragma once

#include <Wt/WCompositeWidget.h>
#include <Wt/WEvent.h>
#include "../materia/StrategyModel.hpp"
#include <boost/signals2/signal.hpp>

class IGraphElement;
class GraphView: public Wt::WCompositeWidget
{
public:
   boost::signals2::signal<void(Wt::WMouseEvent)> OnCaptionClicked;
   boost::signals2::signal<void(StrategyModel::Node, Wt::WMouseEvent)> OnNodeClicked;
   boost::signals2::signal<void(StrategyModel::Link, Wt::WMouseEvent)> OnLinkClicked;

   GraphView();

   void reset();
   void assign(const StrategyModel::Graph& g, const std::string& caption);

private:
   void OnElementClicked(Wt::WMouseEvent ev, const IGraphElement& element);

   Wt::WContainerWidget* mImpl;
};