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

class LinkGraphElement: public IGraphElement
{
public:
   LinkGraphElement(
      const std::pair<double, double> from, 
      const std::pair<double, double> to, 
      const materia::Id fromId,
      const materia::Id toId
      )
   : mFrom(from)
   , mTo(to)
   , mFromId(fromId)
   , mToId(toId)
   {

   }

   void draw(Wt::WPainter& painter) const override
   {
      painter.setBrush(Wt::WBrush(Wt::BrushStyle::None));
      painter.setPen(Wt::WPen(Wt::StandardColor::Green));
      painter.drawLine(mFrom.first, mFrom.second, mTo.first, mTo.second);
   }

   Wt::WRectF getBounds() const override
   {
      return Wt::WRectF(0, 0, 0, 0);
   }

   GraphElementType getType() const override
   {
      return GraphElementType::LINK;
   }

private:
   const std::pair<double, double> mFrom;
   const std::pair<double, double> mTo; 
   const materia::Id mFromId;
   const materia::Id mToId;
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
class GraphMatrix
{
public:
   void add(const StrategyModel::Node& n)
   {
      std::size_t h = height();

      for(auto& h_layer : nodes)
      {
         if(h_layer.size() < h)
         {
            h_layer.push_back(n);
            return;
         }
      }

      nodes.push_back({n});
   }

   std::size_t height() const
   {
      return std::max_element(nodes.begin(), nodes.end(), [](auto x){return x.size();})->size();
   }

   std::size_t width() const
   {
      return nodes.size();
   }

   void putLeftOf(const StrategyModel::Node& src, const StrategyModel::Node& n)
   {
      //assumes element exist
      auto pos = getHorizontalPosition(src);
      if(pos == 0)
      {
         nodes.insert(nodes.begin(), {n});
      }
      else
      {
         nodes[pos - 1].push_back(n);
      }
   }

   GraphMatrix operator+ (const GraphMatrix& other)
   {
      const std::size_t ESTETICAL_HEIGHT = 6;

      auto otherH = other.height();
      auto otherW = other.width();

      if(otherH < height() && otherW <= width()) //try to fit
      {

      }
      else if(otherH + height() < ESTETICAL_HEIGHT) //combine vertically
      {
         auto& widestNodes = nodes.size() > other.nodes.size() ? nodes : other.nodes;
         auto& shortestNodes = &widestNodes == &nodes ? other.nodes : nodes;

         auto resultNodes = widestNodes;
         std::size_t h_offset = widestNodes.size() - shortestNodes.size();
         for(std::size_t i = 0; i < shortestNodes.size(); ++i)
         {
            resultNodes[i + h_offset].insert(resultNodes[i + h_offset].begin(), shortestNodes[i].begin(), shortestNodes[i].end());
         }

         nodes = resultNodes;
      }
      else //combine horizontally
      {
         nodes.insert(nodes.begin(), other.nodes.begin(), other.nodes.end());
      }

      return *this;
   }

private:
   std::size_t getHorizontalPosition(const StrategyModel::Node& n) const
   {
      for(std::size_t i = 0; i < nodes.size(); ++i)
      {
         if(materia::find_by_id(nodes[i], n.id) != nodes[i].end())
         {
            return i;
         }
      }

      return -1;
   }

   std::vector<std::vector<StrategyModel::Node>> nodes; 
};

std::vector<StrategyModel::Node> getPredecessors(const StrategyModel::Graph& g, const materia::Id sucessorId)
{
   std::vector<StrategyModel::Node> result;

   for(auto l : g.links)
   {
      if(l.to == sucessorId)
      {
         auto srcNode = materia::find_by_id(g.nodes, l.from);
         if(srcNode != g.nodes.end())
         {
            result.push_back(*srcNode);
         }
      }
   }

   return result;
}

void putNodeRecursively(const StrategyModel::Graph& g, const StrategyModel::Node& parent, const StrategyModel::Node& n, GraphMatrix& matrix)
{
   matrix.putLeftOf(parent, n);

   auto predecessors = getPredecessors(g, n.id);
   for(auto p : predecessors)
   {
      putNodeRecursively(g, n, p, matrix);
   }
}

GraphMatrix layoutHorizontally(const StrategyModel::Graph& g, const StrategyModel::Node& n)
{
   GraphMatrix result;

   result.add(n);

   auto predecessors = getPredecessors(g, n.id);
   for(auto p : predecessors)
   {
      putNodeRecursively(g, n, p, result);
   }

   return result;
}

GraphMatrix layoutNodes(const StrategyModel::Graph& g)
{
   GraphMatrix result;

   //Find all endpoint nodes
   std::vector<StrategyModel::Node> endpointNodes;
   std::copy_if(g.nodes.begin(), g.nodes.end(), std::back_inserter(endpointNodes), [&](auto x)
   {
      return g.links.end() == std::find_if(g.links.begin(), g.links.end(), [&](auto l){return l.to == x.id;});
   });

   //Layout graph from them & Unite all graphs
   for(auto n : endpointNodes)
   {
      result = result + layoutHorizontally(g, n);
   }

   //Put non-linked nodes on free spots
   std::vector<StrategyModel::Node> unlinkedNodes;
   std::copy_if(g.nodes.begin(), g.nodes.end(), std::back_inserter(unlinkedNodes), [&](auto x)
   {
      return g.links.end() == std::find_if(g.links.begin(), g.links.end(), [&](auto l){return l.to == x.id || l.from == x.id;});
   });

   for(auto n : unlinkedNodes)
   {
      result.add(n);
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

   std::map<materia::Id, std::pair<double, double>> nodeToPositionMap;

   //build drawables from nodes
   int colSize = width / layout.size();

   for(std::size_t i = 0; i < layout.size(); ++i)
   {
      int rowSize = height / layout[i].size();
      for(std::size_t j = 0; j < layout[i].size(); ++j)
      {
         int x = width - colSize * i - colSize / 2;
         int y = rowSize * j + rowSize / 2;

         nodeToPositionMap[layout[i][j].id] = std::make_pair(x, y);
         result.push_back(std::make_shared<NodeGraphElement>(x, y, layout[i][j]));
      }
   }

   //build drawables from links
   for(auto l : g.links)
   {
      result.push_back(std::make_shared<LinkGraphElement>(nodeToPositionMap[l.from], nodeToPositionMap[l.to] ,l.from, l.to));
   }

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