#include "materiagateway.h"
#include "logger.h"

MateriaGateway::MateriaGateway(const QString &ip, QObject *parent)
    : QObject(parent)
    , mClient("Desktop", ip.toStdString())
{

}

void MateriaGateway::loadAffinities()
{
    Logger::instance().log("Loading affinities...");
    auto affs = mClient.getStrategy().getAffinities();
    Logger::instance().log("Done");
    emit onAffinitiesLoaded(affs);
}

void MateriaGateway::configureAffinities(const std::vector<materia::Affinity> affinities)
{
    Logger::instance().log("Configuring default affinities...");
    mClient.getStrategy().configureAffinities(affinities);
    Logger::instance().log("Done");
}

void MateriaGateway::loadGoals()
{
    Logger::instance().log("Loading goals...");
    auto goals = mClient.getStrategy().getGoals();
    Logger::instance().log("Done");
    emit onGoalsLoaded(goals);
}

void MateriaGateway::loadGoalDetails(const materia::Id id)
{
    Logger::instance().log(QString("Loading goal details for: {%1}").arg(id.guid.c_str()));
    auto goalDetails = mClient.getStrategy().getGoalItems(id);
    Logger::instance().log("Done");
    emit onGoalDetailsLoaded(std::get<0>(goalDetails), std::get<1>(goalDetails));
}
