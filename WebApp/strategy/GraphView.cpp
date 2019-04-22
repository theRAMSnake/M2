#include "GraphView.hpp"
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>

void GraphView::reset()
{
   mImpl->clear();
   mImpl->hide();
}

void GraphView::assign(const StrategyModel::Graph& g)
{
   reset();

   //do layout code here
   mImpl->setStyleClass("GraphView");
   mImpl->show();

   mName = mImpl->addWidget(std::make_unique<Wt::WLabel>());
   mName->setStyleClass("GraphViewCaption");
   mName->clicked().connect([&](Wt::WMouseEvent e) { OnCaptionClicked(e); });
}

GraphView::GraphView()
{
   mImpl = setImplementation(std::make_unique<Wt::WContainerWidget>());
   mImpl->hide();
}

void GraphView::setCaption(const std::string& caption)
{
   mName->setText(caption);
}