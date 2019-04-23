#include "GraphEditDialog.hpp"
#include "GraphView.hpp"
#include <Wt/WToolBar.h>
#include <Wt/WContainerWidget.h>
#include <Wt/WPushButton.h>

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

   mGraphView = contents()->addWidget(std::make_unique<GraphView>());
   mGraphView->OnNodeClicked.connect(std::bind(&GraphEditDialog::handleNodeClicked, this, std::placeholders::_1, std::placeholders::_2));

   refreshGraph();

   setWidth("50%");
   setHeight("50%");

   finished().connect(std::bind([=]() {
      cb(*mModel.getGraph(mId));
      delete this;
   }));
}

void GraphEditDialog::refreshGraph()
{
   mGraphView->assign(*mModel.getGraph(mId));
}

void GraphEditDialog::createNode()
{
   mModel.createNode(mId);
   refreshGraph();
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
      else
      {
         
      }
   }
}