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
        iter->second->addWidget(new QLabel(g.name.c_str()));
    }
}

void StrategyView::onAffinitiesUpdated()
{
    auto affs = mStrategyDataModel.getAffinities();
    for(auto a : affs)
    {
        auto f = new QFrame();
        f->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        f->setStyleSheet(QString ("QFrame { background-color : %1; color : #eff0f1; font-size : 20px; }").arg(a.colorName.c_str()));

        auto g = new QGridLayout(f);
        auto l = new QLabel();
        l->setText(a.name.c_str());
        g->addWidget(l);

        mLayout->addWidget(f);
        mAffinityToGuiMap.insert(std::make_pair(a.id, g));
    }
}
