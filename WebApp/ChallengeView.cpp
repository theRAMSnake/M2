#include "ChallengeView.hpp"
#include "WidgetFactory.hpp"
#include "dialog/CommonDialogManager.hpp"

#include <Wt/WPainter.h>
#include <Wt/WPaintedWidget.h>
#include <Wt/WGroupBox.h>
#include <fmt/format.h>

#include <variant>

void fillLayerDetails(CustomDialog<StagesLayer>& dlg)
{
    dlg.addCheckboxSet("Stages", &StagesLayer::stages);
}

void fillLayerDetails(CustomDialog<PointsLayer>& dlg)
{
    dlg.addNumberEdit("Points", &PointsLayer::newPoints);
}

template<class LayerT>
CustomDialog<LayerT>* createDialog(const LayerT item)
{
    auto d = CommonDialogManager::createCustomDialog("Layer view", item);
    
    fillLayerDetails(*d);

    return d;
}

//-----------------------------------------------------------------------------------------------------

void drawLayer(Wt::WPainter& p, const double w, const double h, const PointsLayer& l)
{
    if(l.pointsNeeded == 0)
    {
        return;
    }
    double amountFilled = w * std::min(1.0, double(l.points) / double(l.pointsNeeded));

    auto greenBrush = Wt::WBrush(Wt::StandardColor::DarkGreen);
    auto grayBrush = Wt::WBrush(Wt::StandardColor::Gray);

    p.fillRect({2, 4, amountFilled, (double)h - 8}, greenBrush);
    p.fillRect({2 + amountFilled, 4, 1.0 - amountFilled, (double)h - 8}, grayBrush);

    p.drawText({0, 0, (double)w, (double)h}, 
        Wt::AlignmentFlag::Center | Wt::AlignmentFlag::Middle, 
        fmt::format("{}: {}/{}", l.type, l.points, l.pointsNeeded)
        );
}

void drawLayer(Wt::WPainter& p, const double w, const double h, const StagesLayer& l)
{
    if(l.stages.empty())
    {
        return;
    }

    double sizePerBlock = (w * 0.8) / (l.stages.size());

    auto greenBrush = Wt::WBrush(Wt::StandardColor::DarkGreen);
    auto grayBrush = Wt::WBrush(Wt::StandardColor::Gray);

    for(unsigned int i = 0; i < l.stages.size(); ++i)
    {
        p.fillRect({2 + w / 5 + i * (sizePerBlock), 4, sizePerBlock - 4, h - 8},
            l.stages[i] ? greenBrush : grayBrush);
    }
}

template<class LayerT>
class LayerView : public Wt::WPaintedWidget
{
public:
    LayerView(const materia::Id chId, const materia::Id layerId, const LayerT& l, ChallengeModel& model)
    : mChId(chId)
    , mLayerId(layerId)
    , mItem(l)
    , mModel(model)
    {
        setWidth(1850);
        setHeight(32);
        setMargin(20, Wt::Side::Left);

        clicked().connect(std::bind(&LayerView<LayerT>::onClicked, this, std::placeholders::_1));
        doubleClicked().connect(std::bind(&LayerView<LayerT>::onDoubleClicked, this));
    }

protected:
    void paintEvent(Wt::WPaintDevice *paintDevice) 
    {
        Wt::WPainter painter(paintDevice);

        drawLayer(painter, paintDevice->width().toPixels(), paintDevice->height().toPixels(), mItem);
    }

private:
    void onClicked(Wt::WMouseEvent ev)
    {
        if(ev.modifiers().test(Wt::KeyboardModifier::Control))
        {
            std::function<void()> elementDeletedFunc = [=, this] () {
                mModel.eraseLayer(mChId, mLayerId);
                };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
        }
    }

    void onDoubleClicked()
    {
        auto dlg = createDialog<LayerT>(mItem);
        dlg->onResult.connect([this](auto e)
        {
            mModel.apply(mChId, mLayerId, e);
        });

        dlg->show();
    }

    const materia::Id mChId;
    const materia::Id mLayerId;
    const LayerT mItem;
    ChallengeModel& mModel;
};

//-------------------------------------------------------------------------------------------------------
struct WrappedUnsignedInt
{
    unsigned int val;
};
CustomDialog<WrappedUnsignedInt>* createNewStagesDialog()
{
    WrappedUnsignedInt v;
    auto d = CommonDialogManager::createCustomDialog("Create stages layer", v);
    
    d->addNumberEdit("Num stages", &WrappedUnsignedInt::val);

    return d;
}

CustomDialog<PointsLayer>* createNewPointsDialog()
{
    auto d = CommonDialogManager::createCustomDialog("Create points layer", {});
    
    d->addNumberEdit("Points needed", &PointsLayer::pointsNeeded);
    d->addNumberEdit("Points advance", &PointsLayer::pointsAdvance);

    std::vector<challenge::PointsLayerType> types = {challenge::PointsLayerType::WEEKLY, challenge::PointsLayerType::TOTAL};
      d->addComboBox("Type", 
         types, 
         types.begin() + ev.eventType, 
         [](auto x){return to_string(x);}, 
         [](PointsLayer& obj, const challenge::PointsLayerType& selected){obj.type = selected;}
         );

    return d;
}

class ChallengeItemView : public Wt::WContainerWidget
{
public:
    ChallengeItemView(const ChallengeModel::Item& item, ChallengeModel& model)
    : mItem(item)
    , mModel(model)
    {
        setMargin(20, Wt::Side::Left);

        auto vs = addWidget(createSplit(SplitType::Vertical, 0.2));

        vs->first.addWidget(createLabel(item.name, Wt::FontSize::Large));
        vs->first.addWidget(createLabel(fmt::format("Level {}/{}", item.level, item.levelMax), Wt::FontSize::Small));
        vs->first.addWidget(createButton("X", std::bind(&ChallengeItemView::onDeleteClick, this)));
        vs->first.addWidget(createButton("+", std::bind(&ChallengeItemView::onAddClick, this)));

        auto& t = *vs->second.addWidget(createList());
        for(std::size_t i = 0; i < item.layers.size(); ++i)
        {
            auto cell = t.elementAt(i, 0);

            std::visit([&](auto&& arg) {
                cell->addWidget(std::make_unique<LayerView<std::decay_t<decltype(arg)>>>(mItem.id, item.layers[i].id, arg, model));
            }, item.layers[i].params);
        }
    }

private:
    void onAddClick()
    {
        std::vector<std::string> choise = {"Stages", "Points"};
        CommonDialogManager::showChoiseDialog(choise, [=](auto selected) {
            const bool isStages = selected == 0;

            if(isStages)
            {
                auto d = createNewStagesDialog();
                d->onResult.connect([=](auto e)
                {
                    mModel.createStagesLayer(mItem.id, e.val);
                });

                dlg->show();
            }
            else
            {
                auto d = createNewPointsDialog();
                d->onResult.connect([=](auto e)
                {
                    mModel.createPointsLayer(mItem.id, e);
                });

                dlg->show();
            }
        });
    }

    void onDeleteClick()
    {
        if(ev.modifiers().test(Wt::KeyboardModifier::Control))
        {
            std::function<void()> elementDeletedFunc = [=] () {
                mModel.eraseChallenge(mItem.id);
                };

            CommonDialogManager::showConfirmationDialog("Delete it?", elementDeletedFunc);
        }
    }

    ChallengeModel& mModel;
    const ChallengeModel::Item mItem;
};

//----------------------------------------------------------------------------------------------------

ChallengeView::ChallengeView(ChallengeModel& model)
: mModel(model)
{
   addWidget(createButton("Add", std::bind(&ChallengeView::onAddClick, this)));

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

CustomDialog<std::pair<std::string, unsigned int>>* createNewChallengeDialog()
{
    using ItemType = std::pair<std::string, unsigned int> ;
    ItemType result {"", 1};
    auto d = CommonDialogManager::createCustomDialog("Create challenge", result);
    
    d->addLineEdit("Name", &ItemType::first);
    d->addNumberEdit("Max level", &ItemType::second);

    return d;
}

void ChallengeView::onAddClick()
{
    auto dlg = createNewChallengeDialog();
    d->onResult.connect([=](auto e)
    {
        mModel.createChallenge(e.first, e.second);
    });

    dlg->show();
}