#ifndef MATERIAGATEWAY_H
#define MATERIAGATEWAY_H

#include <QObject>
#include "materiaclient.h"

class MateriaGateway : public QObject
{
    Q_OBJECT
public:
    MateriaGateway(const QString &ip, QObject *parent = nullptr);

signals:
    void onAffinitiesLoaded(const std::vector<materia::Affinity> affinities);
    void onGoalsLoaded(const std::vector<materia::Goal> goals);
    void onGoalDetailsLoaded(const materia::Id id, const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives);

public slots:
    void loadAffinities();
    void loadGoals();
    void loadGoalDetails(const materia::Id id);
    void configureAffinities(const std::vector<materia::Affinity> affinities);

private:
    materia::MateriaClient mClient;
};

#endif // MATERIAGATEWAY_H
