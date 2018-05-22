#ifndef STRATEGYVIEW_H
#define STRATEGYVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include "Models/strategydatamodel.h"
#include "iconmanager.h"

class QGridLayout;
class GoalWidget;
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
    void onGoalDetailsLoaded(const materia::Id id, const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives);

private:
    QHBoxLayout* mLayout;
    StrategyDataModel& mStrategyDataModel;
    std::map<materia::Id, QGridLayout*> mAffinityToGuiMap;
    std::map<materia::Id, int> mNumGoalsPerId;
    std::map<materia::Id, GoalWidget*> mGoalWidgetById;
    IconManager mIconManager;
};

#endif // STRATEGYVIEW_H
