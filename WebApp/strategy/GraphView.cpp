#include "GraphView.hpp"
#include <Wt/WContainerWidget.h>
#include <Wt/WLabel.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WPainter.h>
#include <Wt/WLineEdit.h>
#include <Wt/WTextArea.h>
#include <numeric>
#include "NodeEditDialog.hpp"
#include "../../Common/Utils.hpp"
#include "../dialog/CommonDialogManager.hpp"

//Duplicate
class GoalEditDialog : public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Goal&)> TOnOkCallback;
   GoalEditDialog(const StrategyModel::Goal& subject, TOnOkCallback cb)
   : BasicDialog("Goal Edit", true)
   {
      mTitle = new Wt::WLineEdit(subject.title);
      contents()->addWidget(std::unique_ptr<Wt::WLineEdit>(mTitle));

      mNotes = new Wt::WTextArea();
      mNotes->setHeight(500);
      mNotes->setText(subject.notes);
      mNotes->setMargin("5px", Wt::Side::Top);
      
      contents()->addWidget(std::unique_ptr<Wt::WTextArea>(mNotes));

      finished().connect(std::bind([=]() {
        if (result() == Wt::DialogCode::Accepted)
        {
           StrategyModel::Goal newGoal = subject;
           newGoal.title = mTitle->text().narrow();
           newGoal.notes = mNotes->text().narrow();
           cb(newGoal);
        }

        delete this;
      }));
   }

private:
   Wt::WLineEdit* mTitle;
   Wt::WTextArea* mNotes;
};

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
   virtual bool hitTest(const Wt::WPointF& pt) const = 0;
   virtual GraphElementType getType() const = 0;

   ~IGraphElement(){}
};

Wt::WFont createFont(const Wt::FontSize sz)
{
   Wt::WFont f;
   f.setFamily(Wt::FontFamily::Monospace, "'Courier New'");
   f.setSize(sz);
   return f;
}

class NodeGraphElement: public IGraphElement
{
public:
   NodeGraphElement(const unsigned int x, const unsigned int y, const StrategyModel::Node& n)
   : mSize(n.type == strategy::NodeType::GOAL ? 64 : 48)
   , mNode(n)
   , mBounds(x - mSize / 2, y - mSize / 2, mSize, mSize)
   , mImageRect(mBounds.x() + 8, mBounds.y() + 8, mSize - 16, mSize - 16)
   {

   }

   void draw(Wt::WPainter& painter) const override
   {
      painter.setBrush(Wt::WBrush(Wt::StandardColor::Black));

      auto pen = Wt::WPen(mNode.isDone ? Wt::StandardColor::Green : Wt::StandardColor::White);
      pen.setWidth(2);
      painter.setPen(pen);
      painter.drawEllipse(mBounds);

      if(mNode.type == strategy::NodeType::GOAL)
      {
         painter.drawImage(mImageRect, Wt::WPainter::Image("resources/achievement.png", 48, 48));
      }
      else if(mNode.type == strategy::NodeType::TASK)
      {
         painter.drawImage(mImageRect, Wt::WPainter::Image("resources/task.png", 258, 258));
      }
      else if(mNode.type == strategy::NodeType::REFERENCE)
      {
         painter.drawImage(mImageRect, Wt::WPainter::Image("resources/achievement.png", 48, 48));
      }
      else if(mNode.type == strategy::NodeType::WATCH)
      {
         painter.drawImage(mImageRect, Wt::WPainter::Image("resources/eye.png", 256, 256));
      }
      else if(mNode.type == strategy::NodeType::WAIT)
      {
         Wt::WRectF imageRect(mBounds.x() + 8 + 2, mBounds.y() + 8 + 2, 32, 32);
         painter.drawImage(imageRect, Wt::WPainter::Image("resources/hourglass.png", 256, 256));
      }
      else if(mNode.type == strategy::NodeType::COUNTER)
      {
         painter.setFont(createFont(Wt::FontSize::Small));
         painter.drawText(mImageRect, Wt::AlignmentFlag::Center | Wt::AlignmentFlag::Middle, 
            std::to_string(mNode.progress.first) + "/" + std::to_string(mNode.progress.second));
      }
      
      if(mNode.type != strategy::NodeType::GOAL)
      {
         painter.setFont(createFont(Wt::FontSize::XXSmall));

         auto b = mBounds;
         b.setY(b.y() + b.height() + 10);
         b.setHeight(10);
         painter.drawText(b, Wt::AlignmentFlag::Center, mNode.descriptiveTitle);
      }
   }

   bool hitTest(const Wt::WPointF& pt) const override
   {
      return mBounds.contains(pt);
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
   const Wt::WRectF mImageRect;
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

   bool hitTest(const Wt::WPointF& pt) const override
   {
      return mBounds.contains(pt);
   }

   GraphElementType getType() const override
   {
      return GraphElementType::TEXT;
   }

private:
   const Wt::WRectF mBounds;
   const std::string mText;
};

double getDistanceBetweenPoints(const std::pair<double, double>& a, const std::pair<double, double>& b)
{
   return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

bool fcmp(const double a, const double b)
{
   return fabs(a-b) < a * .001;
}

class LinkGraphElement: public IGraphElement
{
public:
   LinkGraphElement(
      const std::pair<double, double> from, 
      const std::pair<double, double> to, 
      const materia::Id fromId,
      const materia::Id toId,
      const Wt::StandardColor color
      )
   : mFrom(from)
   , mTo(to)
   , mFromId(fromId)
   , mToId(toId)
   , mColor(color)
   {

   }

   void draw(Wt::WPainter& painter) const override
   {
      painter.setBrush(Wt::WBrush(Wt::BrushStyle::None));
      painter.setPen(Wt::WPen(mColor));
      painter.drawLine(mFrom.first, mFrom.second, mTo.first, mTo.second);
   }

   bool hitTest(const Wt::WPointF& pt) const override
   {
      auto testPoint = std::make_pair(pt.x(), pt.y());
      return fcmp(getDistanceBetweenPoints(mFrom, testPoint) + getDistanceBetweenPoints(testPoint, mTo), getDistanceBetweenPoints(mFrom, mTo));
   }

   GraphElementType getType() const override
   {
      return GraphElementType::LINK;
   }

   StrategyModel::Link getLink() const
   {
      return StrategyModel::Link {mFromId, mToId};
   }

private:
   const std::pair<double, double> mFrom;
   const std::pair<double, double> mTo; 
   const materia::Id mFromId;
   const materia::Id mToId;
   const Wt::StandardColor mColor;
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
         if(x->hitTest(static_cast<Wt::WPointF>(ev.widget())))
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
      if(nodes.empty())
      {
         return 0;
      }
      return std::max_element(nodes.begin(), nodes.end(), [](auto x, auto y){return x.size() < y.size();})->size();
   }

   std::size_t width() const
   {
      return nodes.size();
   }

   void putLeftOf(const StrategyModel::Node& src, const StrategyModel::Node& n)
   {
      auto srcPos = getHorizontalPosition(src);
      auto pos = getHorizontalPosition(n);

      if(pos != std::size_t(-1))
      {
         nodes[pos].erase(materia::find_by_id(nodes[pos], n.id));
      }

      const std::size_t MAXIMUM_COL_HEIGHT = 4;
      auto curPos = srcPos == 0 ? 0 : srcPos - 1;
      while(curPos != 0 && nodes[curPos].size() >= MAXIMUM_COL_HEIGHT)
      {
         curPos--;
      }

      if(curPos == 0)
      {
         nodes.insert(nodes.begin(), {n});
      }
      else
      {
         nodes[curPos].push_back(n);
      }
   }

   GraphMatrix operator+ (const GraphMatrix& other)
   {
      const std::size_t ESTETICAL_HEIGHT = 4;

      auto otherH = other.height();

      //remove duplicates (other option is to layout accordingly)
      auto allOtherNodes = std::accumulate(
         other.nodes.begin(), 
         other.nodes.end(), 
         std::vector<StrategyModel::Node>(), 
         [](auto x, auto y){x.insert(x.begin(), y.begin(), y.end()); return x;});

      std::cout << "all other nodes size: " << allOtherNodes.size() << "\n";

      for(auto& x: nodes)
      {
         erase_if(x, [&](auto n){return allOtherNodes.end() != materia::find_by_id(allOtherNodes, n.id);});
      }

      if(width() == 0)
      {
         nodes = other.nodes;
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

   std::size_t nodesCount() const
   {
      return std::accumulate(nodes.begin(), nodes.end(), 0, [](auto a, auto b){return a + b.size();});
   }

   const std::vector<StrategyModel::Node> getLayer(const std::size_t pos) const
   {
      return nodes[pos];
   }

   void debugPrint()
   {
      for(std::size_t i = 0; i < nodes.size(); ++i)
      {
         for(std::size_t j = 0; j < nodes[i].size(); ++j)
         {
            std::cout << i << ", " << j << ": node\n";
         }
      }

      std::cout << "end\n";

      std::cout.flush();
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

      return std::size_t(-1);
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

   for(auto x : g.links)
   {
      std::cout << x.from << "--->" << x.to << "\n";
   }

   //Find all endpoint nodes
   std::vector<StrategyModel::Node> endpointNodes;
   std::copy_if(g.nodes.begin(), g.nodes.end(), std::back_inserter(endpointNodes), [&](auto x)
   {
      //something leads to it
      //nothing lead from it
      return g.links.end() != std::find_if(g.links.begin(), g.links.end(), [&](auto l){return l.to == x.id;}) &&
         g.links.end() == std::find_if(g.links.begin(), g.links.end(), [&](auto l){return l.from == x.id;});
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

   result.debugPrint();

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
   std::cout << "starting layout\n";
   std::cout.flush();

   auto layout = layoutNodes(g);

   std::vector<std::shared_ptr<IGraphElement>> result;

   std::cout << g.nodes.size() << ":" << layout.nodesCount();
   std::cout.flush();
   assert(g.nodes.size() == layout.nodesCount());

   result.push_back(std::make_shared<TextGraphElement>(Wt::WRectF(0, 10, width, 50), caption));

   std::map<materia::Id, std::pair<double, double>> nodeToPositionMap;

   //build drawables from nodes
   int colSize = width / layout.width();

   for(std::size_t i = 0; i < layout.width(); ++i)
   {
      auto curLayer = layout.getLayer(i);
      if(curLayer.empty())
      {
         continue;
      }
      int rowSize = height / curLayer.size();
      for(std::size_t j = 0; j < curLayer.size(); ++j)
      {
         int x = colSize * i + colSize / 2;
         int y = rowSize * j + rowSize / 2;

         nodeToPositionMap[curLayer[j].id] = std::make_pair(x, y);
         result.push_back(std::make_shared<NodeGraphElement>(x, y, curLayer[j]));
      }
   }

   //build drawables from links
   for(auto l : g.links)
   {
      result.insert(result.begin(), std::make_shared<LinkGraphElement>(
         nodeToPositionMap[l.from], 
         nodeToPositionMap[l.to], 
         l.from, 
         l.to,
         materia::find_by_id(g.nodes, l.from)->isDone ? Wt::StandardColor::Green : Wt::StandardColor::White
         ));
   }

   return result;
}

void GraphView::reset()
{
   mImpl->clear();
   mImpl->hide();
}

void GraphView::assign(const materia::Id& id, const StrategyModel::Graph& g, const std::string& caption)
{
   reset();

   mId = id;
   
   mImpl->setStyleClass("GraphView");
   mImpl->show();

   auto cgv = mImpl->addWidget(std::make_unique<CompositeGraphView>(900, 350, buildCompositeGraphView(900u, 350u, g, caption)));
   cgv->OnElementClicked.connect(std::bind(&GraphView::OnElementClicked, this, std::placeholders::_1, std::placeholders::_2));
}

GraphView::GraphView(StrategyModel& model)
: mModel(model)
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

      case GraphElementType::LINK:
      {
         auto& linkNode = static_cast<const LinkGraphElement&>(element);
         OnLinkClicked(linkNode.getLink(), ev);
         break;
      }

      default:
         break;
   }
}

void GraphView::refreshGraph()   
{
   assign(mId, *mModel.getGraph(mId), "");
}

void GraphView::OnNodeClicked(const StrategyModel::Node& node, const Wt::WMouseEvent ev)
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
         std::function<void(const StrategyModel::Node)> doneCallback = [=] (const StrategyModel::Node outNode) {
            mModel.updateNode(mId, outNode);
            refreshGraph();
         };

         std::function<void(const StrategyModel::Node)> cloneCallback = [=] (const StrategyModel::Node outNode) {
            mModel.cloneNode(mId, outNode);
            refreshGraph();
         };

         std::function<void(const StrategyModel::Node)> focusCallback = [=] (const StrategyModel::Node outNode) {
            mModel.focusNode(mId, outNode);
            refreshGraph();
         };

         NodeEditDialog* dlg = new NodeEditDialog(node, mModel.getWatchItems(), mModel.getGoals(), doneCallback, cloneCallback, focusCallback);
         dlg->show();
      }
      else if(node.type == strategy::NodeType::GOAL)
      {
         std::function<void(const StrategyModel::Goal)> callback = [=] (const StrategyModel::Goal out) {
            mModel.modifyGoal(out);
            refreshGraph();
         };

         GoalEditDialog* dlg = new GoalEditDialog(*mModel.getGoal(mId), callback);
         dlg->show();
      }
   }
}

void GraphView::OnLinkClicked(const StrategyModel::Link& link, const Wt::WMouseEvent ev)
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