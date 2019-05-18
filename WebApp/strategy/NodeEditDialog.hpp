#pragma once

#include "../dialog/CommonDialogManager.hpp"
#include "../materia/StrategyModel.hpp"

class INodeTypeSpecifics;
class NodeEditDialog: public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Node&)> TOnOkCallback;
   NodeEditDialog(const StrategyModel::Node& node, TOnOkCallback cb);

private:
    std::unique_ptr<INodeTypeSpecifics> mNodeTypeSpecifics;
};