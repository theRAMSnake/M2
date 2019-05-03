#include "GraphEditDialog.hpp"
#include "GraphView.hpp"
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

   auto fromNodeCombo = std::make_unique<Wt::WComboBox>();
   fromNodeCombo->setInline(true);
   fromNodeCombo->setWidth(50);
   fromNodeCombo->setMargin(5, Wt::Side::Left);
   tb->addWidget(std::move(fromNodeCombo));

   auto linkBtn = std::make_unique<Wt::WPushButton>("-->");
   linkBtn->setInline(true);
   linkBtn->setStyleClass("btn-primary");
   tb->addButton(std::move(linkBtn));

   auto toNodeCombo = std::make_unique<Wt::WComboBox>();
   toNodeCombo->setInline(true);
   toNodeCombo->setWidth(50);
   toNodeCombo->setMargin(5, Wt::Side::Left);
   tb->addWidget(std::move(toNodeCombo));

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
   mGraphView->assign(*mModel.getGraph(mId), "");
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