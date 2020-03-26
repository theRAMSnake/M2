#include "ChallengeView.hpp"

void drawLayer(Wt::WPainter& p, const PointsLayer& l)
{
    if(l.pointsNeeded = 0)
    {
        return;
    }
    double amountFilled = paintDevice->width().toPixels() * std::min(1.0, double(l.points) / double(l.pointsNeeded);

    auto greenBrush = Wt::WBrush(Wt::StandardColor::DarkGreen);
    auto grayBrush = Wt::WBrush(Wt::StandardColor::Gray);

    painter.fillRect({2, 4, amountFilled, paintDevice->height().toPixels() - 8}, greenBrush);
    painter.fillRect({2 + amountFilled, 4, 1.0 - amountFilled, paintDevice->height().toPixels() - 8}, grayBrush);

    painter.drawText({0, 0, paintDevice->width().toPixels(), paintDevice->height().toPixels()}, 
        Wt::AlignmentFlag::Center | Wt::AlignmentFlag::Middle, 
        std::format("{}: {}/{}", item.type, item.points, item.pointsNeeded)
        );
}

void drawLayer(Wt::WPainter& p, const StagesLayer& l)
{
    /*SNAKE*/
}

template<class LayerT>
class LayerView : public Wt::WPaintedWidget
{
public:
    LayerView(const materia::Id id, const LayerT& l, ChallengeModel& model)
    : mId(id)
    , mItem(item)
    , mModel(model)
    {
        setWidth(1850);
        setHeight(32);
        setMargin(20, Wt::Side::Left);

        clicked().connect(&LayerView<LayerT>::onClicked, this, std::placeholders::_1);
        doubleclicked().connect(&LayerView<LayerT>::onDoubleClicked, this);
    }

protected:
    void paintEvent(Wt::WPaintDevice *paintDevice) 
    {
        Wt::WPainter painter(paintDevice);

        drawLayer(painter, l);
    }

private:
    void onClicked(Wt::WMouseEvent ev)
    {
        /*SNAKE*/
    }

    void onDoubleClicked()
    {
        /*SNAKE*/
    }

    const materia::Id mId;
    const LayerT mItem;
    ChallengeModel& mModel;
};

//-------------------------------------------------------------------------------------------------------

class ChallengeItemView : public Wt::WContainerWidget
{
public:
    ChallengeItemView(const ChallengeModel::Item& item, ChallengeModel& model)
    : mItem(item)
    {
        setMargin(20, Wt::Side::Left);

        auto vs = addWidget(createSplit(Split::Vertical), 0.2);

        vs->first.addWidget(createLabel(item.name, FontSize::Big));
        vs->first.addWidget(createLabel(std::format("Level {}/{}", item.level, item.maxLevel), FontSize::Small));
        vs->first.addWidget(createButton("X", std::bind(&ChallengeModel::onDeleteClick, this))));
        vs->first.addWidget(createButton("+", std::bind(&ChallengeModel::onAddClick, this))));

        auto& t = *vs->second.addWidget(createTable());
        for(auto& l : item.layers)
        {
            std::visit([&](auto&& arg) {
                t.addWidget(std::make_unique<LayerView<std::decay_t<decltype(arg)>::value>>(l.id, arg, mModel));
            }, l.params);
        }
    }

private:
    void onAddClick()
    {
        /*SNAKE*/
    }

    void onDeleteClick()
    {
        /*SNAKE*/
    }

    const ChallengeModel::Item mItem;
};

//----------------------------------------------------------------------------------------------------

ChallengeView::ChallengeView(ChallengeModel& model)
: mModel(model)
{
   addWidget(createButton("Add", std::bind(&ChallengeModel::onAddClick, this)));

   auto gb = addWidget(std::make_unique<Wt::WGroupBox>());
   mTable = gb->addWidget(createList());

   refreshList();

   model.changed.connect(std::bind(&ChallengeView::refreshList, this));
}

void ChallengeView::refreshList()
{
    mTable->clear();

    auto items = mModel.get();
    for(std::size_t i = 0; i < items.size(); ++i)
    {
       auto cell = mTable->elementAt(i, 0);
       
       cell->addWidget(std::make_unique<ChallengeItemView>(items[i], mModel));
    }
}

void ChallengeView::onAddClick()
{
    /*SNAKE*/
}