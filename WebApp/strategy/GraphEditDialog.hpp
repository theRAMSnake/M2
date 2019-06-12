#pragma once

#include "../dialog/CommonDialogManager.hpp"
#include "../materia/StrategyModel.hpp"

class GraphView;
class GraphEditDialog: public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Graph&, const StrategyModel::Goal&)> TOnOkCallback;
   GraphEditDialog(const StrategyModel::Goal& goal, StrategyModel& model, TOnOkCallback cb);

private:
   void refreshGraph();
   void createNode();
   void linkNodesRequest();
   void handleNodeClicked(StrategyModel::Node node, Wt::WMouseEvent ev);
   void handleLinkClicked(StrategyModel::Link link, Wt::WMouseEvent ev);

   const materia::Id mId;

   StrategyModel& mModel;
   StrategyModel::Graph mGraph;
   StrategyModel::Goal mGoal;
   GraphView* mGraphView;
};