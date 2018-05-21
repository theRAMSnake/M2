#include "strategyview.h"
#include "Models/strategydatamodel.h"
#include <QHBoxLayout>
#include <QLabel>

StrategyView::StrategyView(QWidget *parent, StrategyDataModel &strategyDataModel)
    : QWidget(parent)
    , mStrategyDataModel(strategyDataModel)
{
    connect(&strategyDataModel, &StrategyDataModel::onGoalUpdated, this, &StrategyView::onGoalUpdated);
    connect(&strategyDataModel, &StrategyDataModel::onAffinitiesUpdated, this, &StrategyView::onAffinitiesUpdated);

    mLayout = new QHBoxLayout(this);
}

StrategyView::~StrategyView()
{

}

void StrategyView::onGoalUpdated(const materia::Goal g)
{
    auto iter = mAffinityToGuiMap.find(g.affinityId);
    if(iter != mAffinityToGuiMap.end())
    {
        auto gc = new QLabel();
        gc->setStyleSheet(QString ("border: 1px solid black"));
        gc->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        gc->setMinimumSize(75, 75);
        gc->setMaximumSize(75, 75);

        auto im = mIconManager.get(g.iconId);
        gc->setPixmap(im->scaled(75, 75, Qt::IgnoreAspectRatio, Qt::FastTransformation));

        iter->second->addWidget(gc, mNumGoalsPerId[g.affinityId] / 5, mNumGoalsPerId[g.affinityId] % 5);
        mNumGoalsPerId[g.affinityId]++;
    }
}

void StrategyView::onAffinitiesUpdated()
{
    auto affs = mStrategyDataModel.getAffinities();
    for(auto a : affs)
    {
        auto affinityFrame = new QFrame(this);
        affinityFrame->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        affinityFrame->setStyleSheet(QString ("QFrame { background-color : %1; color : #eff0f1; font-size : 20px; }").arg(a.colorName.c_str()));
        auto vb = new QVBoxLayout(affinityFrame);

        auto l = new QLabel();
        l->setText(a.name.c_str());
        l->setAlignment(Qt::AlignCenter);
        l->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        vb->addWidget(l);

        auto goalsFrame = new QFrame();
        vb->addWidget(goalsFrame);

        auto g = new QGridLayout(goalsFrame);

        mLayout->addWidget(affinityFrame);
        mAffinityToGuiMap.insert(std::make_pair(a.id, g));
    }
}
