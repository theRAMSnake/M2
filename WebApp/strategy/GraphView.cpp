#include "GraphView.hpp"
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>

//---------------------------

class NodeWidget : public Wt::WPaintedWidget
{
public:
   NodeWidget()
   {
      resize(32, 32);
      setInline(true);
   }

protected:
   void paintEvent(Wt::WPaintDevice *paintDevice) 
   {
      Wt::WPainter painter(paintDevice);
      painter.setBrush(Wt::WBrush(Wt::WColor(Wt::StandardColor::DarkYellow)));
      painter.drawEllipse({0, 0, 32, 32});
   }
};

//---------------------------

void GraphView::reset()
{
   mImpl->clear();
   mImpl->hide();
}

void GraphView::assign(const StrategyModel::Graph& g)
{
   reset();
   
   mImpl->setStyleClass("GraphView");
   mImpl->show();

   mName = mImpl->addWidget(std::make_unique<Wt::WLabel>());
   mName->setStyleClass("GraphViewCaption");
   mName->clicked().connect([&](Wt::WMouseEvent e) { OnCaptionClicked(e); });

   layoutNodes(g.nodes);
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

void GraphView::layoutNodes(const std::vector<StrategyModel::Node>& nodes)
{
   for(auto x : nodes)
   {
      auto w = mImpl->addWidget(std::make_unique<NodeWidget>());
      w->mouseWentDown().connect([=](Wt::WMouseEvent e) { OnNodeClicked(x, e); });
   }
}