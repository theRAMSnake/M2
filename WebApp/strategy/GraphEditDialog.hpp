#pragma once

#include "../dialog/CommonDialogManager.hpp"
#include "../materia/StrategyModel.hpp"

class GraphEditDialog: public BasicDialog
{
public:
   GraphEditDialog(const StrategyModel::Goal& goal, const StrategyModel::Graph& graph);
};