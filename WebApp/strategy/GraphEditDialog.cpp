#include "GraphEditDialog.hpp"
#include "GraphView.hpp"
#include "NodeEditDialog.hpp"
#include <Wt/WToolBar.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>
#include <Wt/WComboBox.h>

GraphEditDialog::GraphEditDialog(const StrategyModel::Goal& goal, StrategyModel& model, TOnOkCallback cb)
: BasicDialog("Edit graph", true)
, mId(goal.id)
, mModel(model)
{
   setWindowTitle(goal.title);

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

   mGraphView = contents()->addWidget(std::make_unique<GraphView>());
   mGraphView->OnNodeClicked.connect(std::bind(&GraphEditDialog::handleNodeClicked, this, std::placeholders::_1, std::placeholders::_2));
   mGraphView->OnLinkClicked.connect(std::bind(&GraphEditDialog::handleLinkClicked, this, std::placeholders::_1, std::placeholders::_2));

   refreshGraph();

   setWidth("50%");
   setHeight("70%");

   finished().connect(std::bind([=]() {
      cb(*mModel.getGraph(mId));
      delete this;
   }));
}

void GraphEditDialog::refreshGraph()   
{
   mGraph = *mModel.getGraph(mId);
   mGraphView->assign(mGraph, "");
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

void GraphEditDialog::handleNodeClicked(StrategyModel::Node node, Wt::WMouseEvent ev)
{
   if(ev.button() == Wt::MouseButton::Left)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
         std::function<void()> elementDeletedFunc = [=] () {
            mModel.deleteNode(mId, node.id);
            refreshGraph();
         };

         CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
      else if(node.type != strategy::NodeType::GOAL)
      {
         std::function<void(const StrategyModel::Node)> callback = [=] (const StrategyModel::Node outNode) {
            mModel.updateNode(mId, outNode);
            refreshGraph();
         };

         NodeEditDialog* dlg = new NodeEditDialog(node, mModel.getWatchItems(), mModel.getGoals(), callback);
         dlg->show();
      }
   }
}

void GraphEditDialog::handleLinkClicked(StrategyModel::Link link, Wt::WMouseEvent ev)
{
   if(ev.button() == Wt::MouseButton::Left)
   {
      if(ev.modifiers().test(Wt::KeyboardModifier::Control))
      {
         std::function<void()> elementDeletedFunc = [=] () {
            mModel.deleteLink(mId, link.from, link.to);
            refreshGraph();
         };

         CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
      }
      else
      {
         
      }
   }
}