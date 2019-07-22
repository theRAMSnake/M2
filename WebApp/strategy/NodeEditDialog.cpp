#include "NodeEditDialog.hpp"
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>
#include <Wt/WTimeEdit.h>
#include <Wt/WDateEdit.h>
#include <Wt/WPushButton.h>
#include "../WtConverters.hpp"

class INodeTypeSpecifics
{
public:
   virtual void updateNode(StrategyModel::Node& node) const = 0;
   virtual void cleanUp(Wt::WContainerWidget& contents) = 0;
   virtual bool verify() = 0;

   virtual ~INodeTypeSpecifics(){}
};

class TaskNodeSpecifics : public INodeTypeSpecifics
{
public:
   TaskNodeSpecifics(const StrategyModel::Node& node, Wt::WContainerWidget& contents)
   {
      mIsDone = contents.addWidget(std::make_unique<Wt::WCheckBox>("Done"));
      mIsDone->setChecked(node.isDone);
   }

   void updateNode(StrategyModel::Node& node) const override
   {
      node.isDone = mIsDone->isChecked();
   }

   void cleanUp(Wt::WContainerWidget& contents) override
   {
      contents.removeChild(mIsDone);
   }

   bool verify() override
   {
      return true;
   }

private:
   Wt::WCheckBox* mIsDone;
};

class CounterNodeSpecifics : public INodeTypeSpecifics
{
public:
   CounterNodeSpecifics(const StrategyModel::Node& node, Wt::WContainerWidget& contents)
   {
      mCurrent = contents.addWidget(std::make_unique<Wt::WLineEdit>(std::to_string(node.progress.first)));

      mTotal = contents.addWidget(std::make_unique<Wt::WLineEdit>(std::to_string(node.progress.second)));
      mTotal->setInline(true);
   }

   void updateNode(StrategyModel::Node& node) const override
   {
      try
      {
         node.progress = std::make_pair(std::stoi(mCurrent->text()), std::stoi(mTotal->text()));
      }
      catch(std::invalid_argument& ex)
      {

      }
   }

   void cleanUp(Wt::WContainerWidget& contents) override
   {
      contents.removeChild(mCurrent);
      contents.removeChild(mTotal);
   }

   bool verify() override
   {
      try
      {
         std::stoi(mCurrent->text());
         std::stoi(mTotal->text());
         return true;
      }
      catch(std::invalid_argument& ex)
      {
         return false;
      }
   }

private:
   Wt::WLineEdit* mCurrent;
   Wt::WLineEdit* mTotal;
};

class ConditionNodeSpecifics : public INodeTypeSpecifics
{
public:
   ConditionNodeSpecifics(const StrategyModel::Node& node, std::function<bool(std::string)> verifier, Wt::WContainerWidget& contents)
   : mVerifier(verifier)
   {
      mText = contents.addWidget(std::make_unique<Wt::WLineEdit>(node.condition));
   }

   void updateNode(StrategyModel::Node& node) const override
   {
      node.condition = mText->text().narrow();
   }

   void cleanUp(Wt::WContainerWidget& contents) override
   {
      contents.removeChild(mText);
   }

   bool verify() override
   {
      return mVerifier(mText->text().narrow());
   }

private:
   std::function<bool(std::string)> mVerifier;
   Wt::WLineEdit* mText;
};

template<class TReferencedItem>
class ReferencedNodeSpecifics : public INodeTypeSpecifics
{
public:
   typedef materia::Id StrategyModel::Node::*TMemberPtr;

   ReferencedNodeSpecifics(const StrategyModel::Node& node, TMemberPtr memberPtr, const std::vector<TReferencedItem>& items, Wt::WContainerWidget& contents)
   : mItems(items)
   , mMemberPtr(memberPtr)
   {
      mItemsSelector = contents.addWidget(std::make_unique<Wt::WComboBox>());
      mItemsSelector->addItem("None");

      for(auto x : mItems)
      {
         mItemsSelector->addItem(x.title);
         if(node.*mMemberPtr == x.id)
         {
            mItemsSelector->setCurrentIndex(mItemsSelector->count() - 1);
         }
      }
   }

   void updateNode(StrategyModel::Node& node) const override
   {
      if(mItemsSelector->currentIndex() != 0)
      {
         node.*mMemberPtr = mItems[mItemsSelector->currentIndex() - 1].id;
      }
      else
      {
         node.*mMemberPtr = materia::Id::Invalid;
      }
   }

   void cleanUp(Wt::WContainerWidget& contents) override
   {
      contents.removeChild(mItemsSelector);
   }

   bool verify() override
   {
      return true;
   }

private:
   Wt::WComboBox* mItemsSelector;
   const std::vector<TReferencedItem> mItems;
   const TMemberPtr mMemberPtr;
};

class NoNodeSpecifics : public INodeTypeSpecifics
{
public:
   NoNodeSpecifics()
   {

   }

   void updateNode(StrategyModel::Node& node) const override
   {

   }

   void cleanUp(Wt::WContainerWidget& contents) override
   {
   }

   bool verify() override
   {
      return true;
   }
};

class WaitNodeSpecifics : public INodeTypeSpecifics
{
public:
   WaitNodeSpecifics(const StrategyModel::Node& node, Wt::WContainerWidget& contents)
   {
      auto timestamp = node.requiredTimestamp > 0 ? node.requiredTimestamp : std::time(0);

      mTimeEdit = contents.addWidget(Wt::cpp14::make_unique<Wt::WTimeEdit>());
      mTimeEdit->setTime(timestampToWtTime(timestamp));
      mTimeEdit->setWidth(Wt::WLength("15%"));
      mTimeEdit->addStyleClass("col-md-6");
      mTimeEdit->setMargin(Wt::WLength("35%"), Wt::Side::Left);

      mDateEdit = contents.addWidget(Wt::cpp14::make_unique<Wt::WDateEdit>());
      mDateEdit->setDate(timestampToWtDate(timestamp));
      mDateEdit->setWidth(Wt::WLength("15%"));
      mDateEdit->addStyleClass("col-md-6");
   }

   void updateNode(StrategyModel::Node& node) const override
   {
      node.requiredTimestamp = WtDateTimeToTimestamp(mDateEdit->date(), mTimeEdit->time());
   }

   void cleanUp(Wt::WContainerWidget& contents) override
   {
      contents.removeChild(mTimeEdit);
      contents.removeChild(mDateEdit);
   }

   bool verify() override
   {
      return true;
   }

private:
   Wt::WTimeEdit* mTimeEdit;
   Wt::WDateEdit* mDateEdit;
};

std::vector<std::pair<strategy::NodeType, std::string>> NODE_TYPES = {
   {strategy::NodeType::BLANK, "Unset"},
   {strategy::NodeType::TASK, "Task"},
   {strategy::NodeType::COUNTER, "Counter"},
   {strategy::NodeType::WATCH, "Watch"},
   {strategy::NodeType::WAIT, "Wait"},
   {strategy::NodeType::REFERENCE, "Reference"},
   {strategy::NodeType::MILESTONE, "Milestone"},
   {strategy::NodeType::CONDITION, "Condition"}
};

INodeTypeSpecifics* createNodeSpecifics(
   const strategy::NodeType type, 
   const StrategyModel::Node& node, 
   const std::vector<StrategyModel::WatchItem>& watchItems, 
   const std::vector<StrategyModel::Goal>& goals, 
   std::function<bool(std::string)> conditionVerifier,
   Wt::WContainerWidget& contents
   )
{
   switch(type)
   {
      case strategy::NodeType::TASK:
         return new TaskNodeSpecifics(node, contents);

      case strategy::NodeType::COUNTER:
         return new CounterNodeSpecifics(node, contents);

      case strategy::NodeType::WATCH:
         return new ReferencedNodeSpecifics<StrategyModel::WatchItem>(node, &StrategyModel::Node::watchItemReference, watchItems, contents);

      case strategy::NodeType::WAIT:
         return new WaitNodeSpecifics(node, contents);

      case strategy::NodeType::REFERENCE:
         return new ReferencedNodeSpecifics<StrategyModel::Goal>(node, &StrategyModel::Node::graphReference, goals, contents);

      case strategy::NodeType::CONDITION:
         return new ConditionNodeSpecifics(node, conditionVerifier, contents);

      default:
         return new NoNodeSpecifics();
   }
}

NodeEditDialog::NodeEditDialog(
   const StrategyModel::Node& node, 
   const std::vector<StrategyModel::WatchItem>& watchItems, 
   const std::vector<StrategyModel::Goal>& goals, 
   std::function<bool(std::string)> conditionVerifier,
   TCallback finishedCb,
   TCallback clonedCb,
   TCallback focusCb
   )
: BasicDialog("Edit node", true)
{
   setWindowTitle(node.descriptiveTitle);

   auto cloneBtn = std::make_unique<Wt::WPushButton>("Clone");
   cloneBtn->setInline(true);
   cloneBtn->setMargin(5, Wt::Side::Left);
   cloneBtn->setStyleClass("btn-primary");
   cloneBtn->clicked().connect(std::bind([=]() {
      clonedCb(node);
      reject();
   }));

   footer()->addWidget(std::move(cloneBtn));

   if((node.type == strategy::NodeType::TASK || node.type == strategy::NodeType::COUNTER) && !node.isDone)
   {
      auto focusBtn = std::make_unique<Wt::WPushButton>("Focus");
      focusBtn->setInline(true);
      focusBtn->setMargin(5, Wt::Side::Left);
      focusBtn->setStyleClass("btn-primary");
      focusBtn->clicked().connect(std::bind([=]() {
         focusCb(node);
         reject();
      }));

      footer()->addWidget(std::move(focusBtn));
   }
   

   auto brief = contents()->addWidget(std::make_unique<Wt::WLineEdit>(node.brief));
   brief->setMargin(5, Wt::Side::Bottom);

   auto types = contents()->addWidget(std::make_unique<Wt::WComboBox>());
   types->setMargin(5, Wt::Side::Bottom);

   for(auto x : NODE_TYPES)
   {
      types->addItem(x.second);
      if(x.first == node.type)
      {
         types->setCurrentIndex(types->count() - 1);
      }
   }

   mNodeTypeSpecifics.reset(createNodeSpecifics(node.type, node, watchItems, goals, conditionVerifier, *contents()));

   types->changed().connect(std::bind([=](){
      mNodeTypeSpecifics->cleanUp(*contents());
      mNodeTypeSpecifics.reset(createNodeSpecifics(NODE_TYPES[types->currentIndex()].first, node, watchItems, goals, conditionVerifier, *contents()));
   }));

   setWidth("50%");
   setHeight("50%");

   finished().connect(std::bind([=]() 
   {
      StrategyModel::Node n = node;
      n.brief = brief->text().narrow();
      n.type = NODE_TYPES[types->currentIndex()].first;

      mNodeTypeSpecifics->updateNode(n);
      
      finishedCb(n);
      delete this;
   }));
}

bool NodeEditDialog::verify()
{
   return mNodeTypeSpecifics->verify();
}