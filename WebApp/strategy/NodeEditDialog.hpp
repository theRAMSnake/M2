#pragma once

#include "../dialog/CommonDialogManager.hpp"
#include "../materia/StrategyModel.hpp"

class INodeTypeSpecifics;
class NodeEditDialog: public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Node&)> TCallback;

   NodeEditDialog(
       const StrategyModel::Node& node,
       const std::vector<StrategyModel::WatchItem>& watchItems, 
       const std::vector<StrategyModel::Goal>& goals, 
       TCallback finishedCb,
       TCallback clonedCb
       );

private:

    std::unique_ptr<INodeTypeSpecifics> mNodeTypeSpecifics;
};