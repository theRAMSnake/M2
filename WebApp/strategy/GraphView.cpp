#include "GraphView.hpp"
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>

class IGraphElement
{
public:
   virtual void draw(Wt::WPainter& painter) const = 0;

   ~IGraphElement(){}
};

class NodeDrawable: public IGraphElement
{
public:
   NodeDrawable(const unsigned int x, const unsigned int y, const StrategyModel::Node& n)
   : mX(x)
   , mY(y)
   , mNode(n)
   {

   }

   void draw(Wt::WPainter& painter) const override
   {
      const double size = 48;
      painter.setBrush(Wt::WBrush(Wt::WColor(Wt::StandardColor::Yellow)));
      painter.drawEllipse(mX + size / 2, mY + size / 2, size, size);
   }

private:
   const unsigned int mX;
   const unsigned int mY;
   const StrategyModel::Node mNode;
};

class TextDrawable: public IGraphElement
{
public:
   TextDrawable(const Wt::WRectF& bounds, const std::string& s)
   : mBounds(bounds)
   , mText(s)
   {

   }

   void draw(Wt::WPainter& painter) const override
   {
      painter.setPen(Wt::WPen(Wt::StandardColor::Yellow));

      Wt::WFont f;
      f.setFamily(Wt::FontFamily::Monospace, "'Courier New'");
      f.setSize(Wt::FontSize::Large);
      painter.setFont(f);
      painter.drawText(mBounds, Wt::AlignmentFlag::Center, mText);
   }

private:
   const Wt::WRectF mBounds;
   const std::string mText;
};

class LinkDrawable: public IGraphElement
{
public:
   void draw(Wt::WPainter& painter) const override
   {
      
   }
};

//---------------------------

class DrawList : public Wt::WPaintedWidget
{
public:
   DrawList(const unsigned int width, const unsigned int height, const std::vector<std::shared_ptr<IGraphElement>>& items)
   : mItems(items)
   {
      resize(width, height);
      setInline(true);
   }

protected:
   void paintEvent(Wt::WPaintDevice *paintDevice) 
   {
      Wt::WPainter painter(paintDevice);

      for(auto x : mItems)
      {
         x->draw(painter);
      }
   }

private:
   const std::vector<std::shared_ptr<IGraphElement>> mItems;
};

//---------------------------

std::vector<std::vector<StrategyModel::Node>> layoutNodes(const StrategyModel::Graph& g)
{
   std::vector<std::vector<StrategyModel::Node>> result;

   auto goalNodePos = std::find_if(g.nodes.begin(), g.nodes.end(), [](auto n){return n.type == strategy::NodeType::GOAL;});

   std::vector<StrategyModel::Node> curLayerNodes;

   if(goalNodePos != g.nodes.end())
   {
      curLayerNodes.push_back(*goalNodePos);
      while(!curLayerNodes.empty())
      {
         result.push_back(curLayerNodes);

         std::vector<StrategyModel::Node> nextLayerNodes;
         for(auto x : curLayerNodes)
         {
            for(auto l : g.links)
            {
               if(l.to == x.id)
               {
                  auto srcNode = materia::find_by_id(g.nodes, l.from);
                  if(srcNode != g.nodes.end())
                  {
                     nextLayerNodes.push_back(*srcNode);
                  }
               }
            }
         }

         curLayerNodes = nextLayerNodes;
      }

      //fill last layer nodes (nodes which are unlinked)
      std::set<materia::Id> allLinkedNodesIds;
      for(auto l : g.links)
      {
         allLinkedNodesIds.insert(l.from);
         allLinkedNodesIds.insert(l.to);
      }

      for(auto n : g.nodes)
      {
         if(allLinkedNodesIds.find(n.id) == allLinkedNodesIds.end() && n.type != strategy::NodeType::GOAL)
         {
            curLayerNodes.push_back(n);
         }
      }

      if(!curLayerNodes.empty())
      {
         result.push_back(curLayerNodes);
      }
   }

   return result;
}

template<class T>
std::size_t count(const std::vector<std::vector<T>>& items)
{
   std::size_t result = 0;

   for(auto x : items)
   {
      result += x.size();
   }

   return result;
}

std::vector<std::shared_ptr<IGraphElement>> buildDrawList(
   const unsigned int width, 
   const unsigned int height, 
   const StrategyModel::Graph& g, 
   const std::string& caption
   )
{
   auto layout = layoutNodes(g);

   std::vector<std::shared_ptr<IGraphElement>> result;

   assert(g.nodes.size() == count(layout));

   result.push_back(std::make_shared<TextDrawable>(Wt::WRectF(0, 10, width, 50), caption));

   //build drawables from nodes
   int colSize = width / layout.size();

   for(std::size_t i = 0; i < layout.size(); ++i)
   {
      int rowSize = height / layout[i].size();
      for(std::size_t j = 0; j < layout[i].size(); ++j)
      {
         int x = width - colSize * i - colSize / 2;
         int y = rowSize * j + rowSize / 2;

         result.push_back(std::make_shared<NodeDrawable>(x, y, layout[i][j]));
      }
   }

   //build drawables from links

   return result;
}

void GraphView::reset()
{
   mImpl->clear();
   mImpl->hide();
}

void GraphView::assign(const StrategyModel::Graph& g, const std::string& caption)
{
   reset();
   
   mImpl->setStyleClass("GraphView");
   mImpl->show();

   //mName = mImpl->addWidget(std::make_unique<Wt::WLabel>());
   //mName->setStyleClass("GraphViewCaption");
   //mName->clicked().connect([&](Wt::WMouseEvent e) { OnCaptionClicked(e); });

   mImpl->addWidget(std::make_unique<DrawList>(900, 150, buildDrawList(900u, 150u, g, caption)));
}

GraphView::GraphView()
{
   mImpl = setImplementation(std::make_unique<Wt::WContainerWidget>());
   mImpl->hide();
}