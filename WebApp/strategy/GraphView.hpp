#pragma once

#include <Wt/WCompositeWidget.h>
#include <Wt/WEvent.h>
#include "../materia/StrategyModel.hpp"
#include "../materia/FreeDataModel.hpp"
#include <boost/signals2/signal.hpp>
#include "NodeEditDialog.hpp"

class IGraphElement;
class NodeOperationProvider: public IOperationProvider
{
public:
   boost::signals2::signal<void()> OnDone;

   NodeOperationProvider(const materia::Id id, StrategyModel& model);

   void modify(const StrategyModel::Node& n) override;
   void clone(const StrategyModel::Node& n) override;
   void split(const StrategyModel::Node& n) override;

private:
   const materia::Id mId;
   StrategyModel& mModel;
};

class GraphView: public Wt::WCompositeWidget
{
public:
   boost::signals2::signal<void(Wt::WMouseEvent)> OnCaptionClicked;

   GraphView(StrategyModel& model, FreeDataModel& freeData, ChallengeModel& chModel);

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
   ChallengeModel& mChModel;
   materia::Id mId;
   std::unique_ptr<NodeOperationProvider> mOpProvider;
};