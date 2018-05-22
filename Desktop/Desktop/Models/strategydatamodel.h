#ifndef STRATEGYDATAMODEL_H
#define STRATEGYDATAMODEL_H

#include <QObject>
#include "materiagateway.h"

class StrategyDataModel : public QObject
{
    Q_OBJECT
public:
    StrategyDataModel(MateriaGateway& materiaGateway);

    std::vector<materia::Affinity> getAffinities();

signals:
    void onGoalUpdated(const materia::Goal g);
    void onGoalDetailsUpdated(const materia::Id id, const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives);
    void onAffinitiesUpdated();

public slots:
    void onAffinitiesLoaded(const std::vector<materia::Affinity> affinities);
    void onGoalsLoaded(const std::vector<materia::Goal> goals);
    void init();
    void onGoalDetailsLoaded(const materia::Id id, const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives);

private:
    std::vector<materia::Affinity> mAffinities;
    MateriaGateway& mMateriaGateway;
};

#endif // STRATEGYDATAMODEL_H
