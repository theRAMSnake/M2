#include "materiagateway.h"

MateriaGateway::MateriaGateway(const QString &ip, QObject *parent)
    : QObject(parent)
    , mClient("Desktop", ip.toStdString())
{

}

void MateriaGateway::loadAffinities()
{
    emit sigAffinitiesLoaded(mClient.getStrategy().getAffinities());
}

void MateriaGateway::configureAffinities(std::vector<materia::Affinity> affinities)
{
    mClient.getStrategy().configureAffinities(affinities);
}

void MateriaGateway::loadGoals()
{
    emit sigGoalsLoaded(mClient.getStrategy().getGoals());
}
