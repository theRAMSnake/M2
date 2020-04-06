#pragma once

#include "../dialog/CommonDialogManager.hpp"
#include "../materia/StrategyModel.hpp"
#include "../materia/FreeDataModel.hpp"

class GraphView;
class GraphEditDialog: public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Graph&, const StrategyModel::Goal&)> TOnOkCallback;
   GraphEditDialog(const StrategyModel::Goal& goal, StrategyModel& model, FreeDataModel& fd, ChallengeModel& chModel, TOnOkCallback cb);

private:
   void refreshGraph();
   void createNode();
   void linkNodesRequest();

   const materia::Id mId;

   StrategyModel& mModel;
   StrategyModel::Graph mGraph;
   StrategyModel::Goal mGoal;
   GraphView* mGraphView;
};