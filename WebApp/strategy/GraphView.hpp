#pragma once

#include <Wt/WCompositeWidget.h>
#include <Wt/WEvent.h>
#include "../materia/StrategyModel.hpp"
#include "../materia/FreeDataModel.hpp"
#include <boost/signals2/signal.hpp>

class IGraphElement;
class GraphView: public Wt::WCompositeWidget
{
public:
   boost::signals2::signal<void(Wt::WMouseEvent)> OnCaptionClicked;

   GraphView(StrategyModel& model, FreeDataModel& freeData);

   void reset();
   void assign(const materia::Id& id, const StrategyModel::Graph& g, const std::string& caption);

   void refreshGraph();

private:
   void OnElementClicked(Wt::WMouseEvent ev, const IGraphElement& element);
   void OnNodeClicked(const StrategyModel::Node& node, const Wt::WMouseEvent ev);
   void OnLinkClicked(const StrategyModel::Link& link, const Wt::WMouseEvent ev);

   Wt::WContainerWidget* mImpl;
   StrategyModel& mModel;
   FreeDataModel& mFreeData;
   materia::Id mId;
};