#ifndef MATERIAPROXY_H
#define MATERIAPROXY_H

#include <QObject>
#include <QThread>
#include <MateriaEmu/materiaclient.h>

class MateriaGateway;

struct GoalDetails
{

};

class MateriaProxy : public QObject
{
    Q_OBJECT
public:
    explicit MateriaProxy(const QString& ip, const QString& port, QObject *parent = nullptr);

signals:
    void sigLoadAffinities();
    void sigLoadGoals();
    void sigLoadGoalDetails(materia::Id);
    void sigConfigureAffinities(std::vector<materia::Affinity>);

private slots:
    void onAffinitiesLoaded(std::vector<materia::Affinity> affinities);
    void onGoalsLoaded(std::vector<materia::Goal> goals);
    void onGoalDetailsLoaded(GoalDetails g);

private:
    std::vector<materia::Affinity> mAffinities;
    std::vector<materia::Goal> mGoals;
    std::vector<GoalDetails> mGoalDetails;

    QThread* mMateriaGatewayThread;
    MateriaGateway* mMateriaGateway;
};

#endif // MATERIAPROXY_H
