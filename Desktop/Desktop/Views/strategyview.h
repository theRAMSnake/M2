#ifndef STRATEGYVIEW_H
#define STRATEGYVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include "Models/strategydatamodel.h"

class QGridLayout;
class StrategyView : public QWidget
{
    Q_OBJECT
public:
    StrategyView(QWidget *parent, StrategyDataModel &strategyDataModel);
    ~StrategyView();

signals:

public slots:
    void onGoalUpdated(const materia::Goal g);
    void onAffinitiesUpdated();

private:
    QHBoxLayout* mLayout;
    StrategyDataModel& mStrategyDataModel;
    std::map<materia::Id, QGridLayout*> mAffinityToGuiMap;
};

#endif // STRATEGYVIEW_H
