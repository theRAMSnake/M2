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
    void sigAffinitiesLoaded(std::vector<materia::Affinity> affinities);
    void sigGoalsLoaded(std::vector<materia::Goal> goals);
    void sigGoalDetailsLoaded(std::vector<materia::Task> tasks, std::vector<materia::Objective> objectives);

public slots:
    void loadAffinities();
    void loadGoals();
    void loadGoalDetails(materia::Id id);
    void configureAffinities(std::vector<materia::Affinity>);

private:
    materia::MateriaClient mClient;
};

#endif // MATERIAGATEWAY_H
