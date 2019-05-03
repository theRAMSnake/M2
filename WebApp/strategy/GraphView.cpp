#include "GraphView.hpp"
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>

enum class GraphElementType
{
   TEXT,
   NODE,
   LINK
};

class IGraphElement
{
public:
   virtual void draw(Wt::WPainter& painter) const = 0;
   virtual Wt::WRectF getBounds() const = 0;
   virtual GraphElementType getType() const = 0;

   ~IGraphElement(){}
};

class NodeGraphElement: public IGraphElement
{
public:
   NodeGraphElement(const unsigned int x, const unsigned int y, const StrategyModel::Node& n)
   : mSize(n.type == strategy::NodeType::GOAL ? 64 : 48)
   , mNode(n)
   , mBounds(x - mSize / 2, y - mSize / 2, mSize, mSize)
   {

   }

   void draw(Wt::WPainter& painter) const override
   {
      painter.setBrush(Wt::WBrush(Wt::BrushStyle::None));
      painter.setPen(Wt::WPen(Wt::StandardColor::White));
      painter.drawEllipse(mBounds);

      if(mNode.type == strategy::NodeType::GOAL)
      {
         Wt::WRectF imageRect(mBounds.x() + 8, mBounds.y() + 8, 48, 48);
         painter.drawImage(imageRect, Wt::WPainter::Image("resources/achievement.png", 48, 48));
      }
   }

   Wt::WRectF getBounds() const override
   {
      return mBounds;
   }

   GraphElementType getType() const override
   {
      return GraphElementType::NODE;
   }

   StrategyModel::Node getNode() const
   {
      return mNode;
   }

private:
   const double mSize = 48;
   const StrategyModel::Node mNode;
   const Wt::WRectF mBounds;
};

class TextGraphElement: public IGraphElement
{
public:
   TextGraphElement(const Wt::WRectF& bounds, const std::string& s)
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

   Wt::WRectF getBounds() const override
   {
      return mBounds;
   }

   GraphElementType getType() const override
   {
      return GraphElementType::TEXT;
   }

private:
   const Wt::WRectF mBounds;
   const std::string mText;
};

//---------------------------

class CompositeGraphView : public Wt::WPaintedWidget
{
public:
   boost::signals2::signal<void(Wt::WMouseEvent, const IGraphElement&)> OnElementClicked;

   CompositeGraphView(const unsigned int width, const unsigned int height, const std::vector<std::shared_ptr<IGraphElement>>& items)
   : mItems(items)
   {
      resize(width, height);
      setInline(true);

      clicked().connect(this, &CompositeGraphView::OnClicked);
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
   void OnClicked(Wt::WMouseEvent ev)
   {
      for(auto x : mItems)
      {
         if(x->getBounds().contains(static_cast<Wt::WPointF>(ev.widget())))
         {
            OnElementClicked(ev, *x);
            break;
         }
      }
   }

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

   //split big layers
   const int maxLayerSize = 5;
   auto pos = std::find_if(result.begin(), result.end(), [](auto x) {return x.size() > maxLayerSize;});
   while(pos != result.end())
   {
      int numToSplit = pos->size() / maxLayerSize + 1;

      std::vector<std::vector<StrategyModel::Node>> newLayers;
      newLayers.resize(numToSplit);

      for(std::size_t i = 0; i < pos->size(); ++i)
      {
         newLayers[i % numToSplit].push_back((*pos)[i]);
      }

      {
         *pos = newLayers[0];
         result.insert(pos, newLayers.begin() + 1, newLayers.end());
      }
      
      pos = std::find_if(result.begin(), result.end(), [](auto x) {return x.size() > maxLayerSize;});
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

std::vector<std::shared_ptr<IGraphElement>> buildCompositeGraphView(
   const unsigned int width, 
   const unsigned int height, 
   const StrategyModel::Graph& g, 
   const std::string& caption
   )
{
   auto layout = layoutNodes(g);

   std::vector<std::shared_ptr<IGraphElement>> result;

   assert(g.nodes.size() == count(layout));

   result.push_back(std::make_shared<TextGraphElement>(Wt::WRectF(0, 10, width, 50), caption));

   //build drawables from nodes
   int colSize = width / layout.size();

   for(std::size_t i = 0; i < layout.size(); ++i)
   {
      int rowSize = height / layout[i].size();
      for(std::size_t j = 0; j < layout[i].size(); ++j)
      {
         int x = width - colSize * i - colSize / 2;
         int y = rowSize * j + rowSize / 2;

         result.push_back(std::make_shared<NodeGraphElement>(x, y, layout[i][j]));
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

   auto cgv = mImpl->addWidget(std::make_unique<CompositeGraphView>(900, 230, buildCompositeGraphView(900u, 230u, g, caption)));
   cgv->OnElementClicked.connect(std::bind(&GraphView::OnElementClicked, this, std::placeholders::_1, std::placeholders::_2));
}

GraphView::GraphView()
{
   mImpl = setImplementation(std::make_unique<Wt::WContainerWidget>());
   mImpl->hide();
}

void GraphView::OnElementClicked(Wt::WMouseEvent ev, const IGraphElement& element)
{
   switch(element.getType())
   {
      case GraphElementType::TEXT:
         OnCaptionClicked(ev);
         break;

      case GraphElementType::NODE:
         OnNodeClicked(static_cast<const NodeGraphElement&>(element).getNode(), ev);
         break;

      default:
         break;
   }
}