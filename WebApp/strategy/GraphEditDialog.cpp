#include "GraphEditDialog.hpp"
#include "GraphView.hpp"
#include <Wt/WToolBar.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>

GraphEditDialog::GraphEditDialog(const StrategyModel::Goal& goal, StrategyModel& model, TOnOkCallback cb)
: BasicDialog("Edit graph", true)
, mId(goal.id)
, mModel(model)
, mGoal(goal)
{
   auto tb = contents()->addWidget(std::make_unique<Wt::WToolBar>());
   tb->setMargin(5, Wt::Side::Bottom);

   auto addBtn = std::make_unique<Wt::WPushButton>("Add");
   addBtn->setStyleClass("btn-primary");
   addBtn->clicked().connect(this, &GraphEditDialog::createNode);
   tb->addButton(std::move(addBtn));

   auto linkBtn = std::make_unique<Wt::WPushButton>("-->");
   linkBtn->setInline(true);
   linkBtn->setMargin(5, Wt::Side::Left);
   linkBtn->setStyleClass("btn-primary");
   linkBtn->clicked().connect(this, &GraphEditDialog::linkNodesRequest);
   tb->addButton(std::move(linkBtn));

   mGraphView = contents()->addWidget(std::make_unique<GraphView>(mModel));

   refreshGraph();

   setWidth("50%");
   setHeight("70%");

   finished().connect(std::bind([=]() {
      cb(*mModel.getGraph(mId), mGoal);
      delete this;
   }));
}

void GraphEditDialog::refreshGraph()   
{
   setWindowTitle(mGoal.title);
   mGraph = *mModel.getGraph(mId);
   mGraphView->assign(mId, mGraph, "");
}

void GraphEditDialog::createNode()
{
   mModel.createNode(mId);
   refreshGraph();
}

inline bool nodesSorter(const StrategyModel::Node& a, const StrategyModel::Node& b)
{
   return a.descriptiveTitle > b.descriptiveTitle;
}

void GraphEditDialog::linkNodesRequest()
{
   auto nodes = mGraph.nodes;

   std::sort(nodes.begin(), nodes.end(), nodesSorter);

   std::vector<std::string> choises;
   for(auto g : nodes)
   {
      choises.push_back(g.descriptiveTitle);
   }

   CommonDialogManager::showDoubleComboDialog(choises, choises, [=](auto selected1index, auto selected2index) 
   {
      mModel.createLink(mId, nodes[selected1index].id, nodes[selected2index].id);
      refreshGraph();
   });
}