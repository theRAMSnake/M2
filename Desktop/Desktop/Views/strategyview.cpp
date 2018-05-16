#include "strategyview.h"
#include "Models/strategydatamodel.h"
#include <QHBoxLayout>
#include <QLabel>

StrategyView::StrategyView(QWidget *parent, StrategyDataModel &strategyDataModel) : QWidget(parent)
{
    connect(&strategyDataModel, SIGNAL(onGoalUpdated(const materia::Id)), this, SLOT(onGoalUpdated(const materia::Id)));

    mLayout = new QHBoxLayout(this);
}

void StrategyView::onGoalUpdated(const materia::Goal g)
{
    mLayout->addWidget(new QLabel(g.name.c_str()));
}
