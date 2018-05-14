#include "materiaproxy.h"
#include "materiagatewaythread.h"
#include "materiagateway.h"

MateriaProxy::MateriaProxy(const QString &ip, const QString &port, QObject *parent)
: QObject(parent)
, mMateriaGatewayThread(new MateriaGatewayThread(ip, port))
{
    mMateriaGateway = new MateriaGateway(ip);

    connect(mMateriaGateway, SIGNAL(sigAffinitiesLoaded(std::vector<materia::Affinity>)), this, SLOT(onAffinitiesLoaded(std::vector<materia::Affinity>)));
    connect(mMateriaGateway, SIGNAL(sigGoalsLoaded(std::vector<materia::Goal>)), this, SLOT(onGoalsLoaded(std::vector<materia::Goal>)));

    connect(this, SIGNAL(sigLoadAffinities()), mMateriaGateway, SLOT(loadAffinities()));
    connect(this, SIGNAL(sigConfigureAffinities(std::vector<materia::Affinity>)), mMateriaGateway, SLOT(configureAffinities(std::vector<materia::Affinity>)));
    connect(this, SIGNAL(sigLoadGoals(std::vector<materia::Goal>)), mMateriaGateway, SLOT(loadGoal(std::vector<materia::Goal>)));

    mMateriaGatewayThread->start();
    mMateriaGateway->moveToThread(mMateriaGatewayThread);

    emit sigLoadAffinities();
}

std::vector<materia::Affinity> createDefaultAffinities()
{
    std::vector<materia::Affinity> result;

    result.push_back({"aff1", "Family", "", "Blue"});
    result.push_back({"aff2", "Survival", "", "Green"});
    result.push_back({"aff3", "Creation", "", "Purple"});
    result.push_back({"aff4", "Development", "", "Orange"});

    return result;
}

void MateriaProxy::onAffinitiesLoaded(std::vector<materia::Affinity> affinities)
{
    if(affinities.empty())
    {
        auto defaultAffinities = createDefaultAffinities();

        emit sigConfigureAffinities(defaultAffinities);

        mAffinities = defaultAffinities;
    }
    else
    {
        mAffinities = affinities;
    }
}

void MateriaProxy::onGoalsLoaded(std::vector<materia::Goal> goals)
{

}

