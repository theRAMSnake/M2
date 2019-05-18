#include "NodeEditDialog.hpp"
#include <Wt/WLineEdit.h>
#include <Wt/WCheckBox.h>
#include <Wt/WComboBox.h>

class INodeTypeSpecifics
{
public:
   virtual void updateNode(StrategyModel::Node& node) const = 0;
   virtual void cleanUp(Wt::WContainerWidget& contents) = 0;

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

private:
   Wt::WLineEdit* mCurrent;
   Wt::WLineEdit* mTotal;
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
};

std::vector<std::pair<strategy::NodeType, std::string>> NODE_TYPES = {
   {strategy::NodeType::BLANK, "Unset"},
   {strategy::NodeType::TASK, "Task"},
   {strategy::NodeType::COUNTER, "Counter"},
};

INodeTypeSpecifics* createNodeSpecifics(const strategy::NodeType type, const StrategyModel::Node& node, Wt::WContainerWidget& contents)
{
   switch(type)
   {
      case strategy::NodeType::TASK:
         return new TaskNodeSpecifics(node, contents);

      case strategy::NodeType::COUNTER:
         return new CounterNodeSpecifics(node, contents);

      default:
         return new NoNodeSpecifics();
   }
}

NodeEditDialog::NodeEditDialog(const StrategyModel::Node& node, TOnOkCallback cb)
: BasicDialog("Edit node", true)
{
   setWindowTitle(node.descriptiveTitle);

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

   mNodeTypeSpecifics.reset(createNodeSpecifics(node.type, node, *contents()));

   types->changed().connect(std::bind([=](){
      mNodeTypeSpecifics->cleanUp(*contents());
      mNodeTypeSpecifics.reset(createNodeSpecifics(NODE_TYPES[types->currentIndex()].first, node, *contents()));
   }));

   setWidth("50%");
   setHeight("50%");

   finished().connect(std::bind([=]() 
   {
      StrategyModel::Node n = node;
      n.brief = brief->text().narrow();
      n.type = NODE_TYPES[types->currentIndex()].first;

      mNodeTypeSpecifics->updateNode(n);
      
      cb(n);
      delete this;
   }));
}