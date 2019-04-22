#include "GraphEditDialog.hpp"
#include "GraphView.hpp"

GraphEditDialog::GraphEditDialog(const StrategyModel::Goal& goal, const StrategyModel::Graph& graph)
: BasicDialog("Edit graph", true)
{
   auto gv = contents()->addWidget(std::make_unique<GraphView>());

   gv->assign(graph);

   setWidth("75%");
   setHeight("75%");

   finished().connect(std::bind([=]() {
      delete this;
   }));
}