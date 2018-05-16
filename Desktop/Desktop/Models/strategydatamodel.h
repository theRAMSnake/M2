#ifndef STRATEGYDATAMODEL_H
#define STRATEGYDATAMODEL_H

#include <QObject>
#include "materiagateway.h"

class StrategyDataModel : public QObject
{
    Q_OBJECT
public:
    StrategyDataModel(MateriaGateway& materiaGateway);

signals:
    void onGoalUpdated(const materia::Goal g);

public slots:
    void onAffinitiesLoaded(const std::vector<materia::Affinity> affinities);
    void onGoalsLoaded(const std::vector<materia::Goal> goals);
    void init();

private:
    std::vector<materia::Affinity> mAffinities;
    MateriaGateway& mMateriaGateway;
};

#endif // STRATEGYDATAMODEL_H
