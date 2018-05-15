#include "materiagateway.h"

MateriaGateway::MateriaGateway(const QString &ip, QObject *parent)
    : QObject(parent)
    , mClient("Desktop", ip.toStdString())
{

}

void MateriaGateway::loadAffinities()
{
    emit onAffinitiesLoaded(mClient.getStrategy().getAffinities());
}

void MateriaGateway::configureAffinities(const std::vector<materia::Affinity> affinities)
{
    mClient.getStrategy().configureAffinities(affinities);
}

void MateriaGateway::loadGoals()
{
    emit onGoalsLoaded(mClient.getStrategy().getGoals());
}
