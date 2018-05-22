#include "strategydatamodel.h"

StrategyDataModel::StrategyDataModel(MateriaGateway& materiaGateway)
    : mMateriaGateway(materiaGateway)
{
    connect(&materiaGateway, SIGNAL(onAffinitiesLoaded(const std::vector<materia::Affinity>)), this, SLOT(onAffinitiesLoaded(const std::vector<materia::Affinity>)));
    connect(&materiaGateway, SIGNAL(onGoalsLoaded(const std::vector<materia::Goal>)), this, SLOT(onGoalsLoaded(const std::vector<materia::Goal>)));
    connect(&materiaGateway, &MateriaGateway::onGoalDetailsLoaded, this, &StrategyDataModel::onGoalDetailsLoaded);
}

std::vector<materia::Affinity> createDefaultAffinities()
{
    std::vector<materia::Affinity> result;

    result.push_back({"aff1", "Family", "", "#31365b" /*Blue*/});
    result.push_back({"aff2", "Survival", "", "#31564b" /*Green*/});
    result.push_back({"aff3", "Creation", "", "#41365b" /*Pruple*/});
    result.push_back({"aff4", "Evolution", "", "#51564b" /*Yellow*/});

    return result;
}

void StrategyDataModel::onAffinitiesLoaded(const std::vector<materia::Affinity> affinities)
{
    if(affinities.empty())
    {
        auto defaultAffinities = createDefaultAffinities();

        mMateriaGateway.configureAffinities(defaultAffinities);

        mAffinities = defaultAffinities;
    }
    else
    {
        mAffinities = affinities;
    }

    emit onAffinitiesUpdated();

    mMateriaGateway.loadGoals();
}

void StrategyDataModel::onGoalsLoaded(const std::vector<materia::Goal> goals)
{
    for(auto g : goals)
    {
        emit onGoalUpdated(g);
    }

    for(auto g : goals)
    {
        mMateriaGateway.loadGoalDetails(g.id);
    }
}

void StrategyDataModel::init()
{
    mMateriaGateway.loadAffinities();
}

std::vector<materia::Affinity> StrategyDataModel::getAffinities()
{
    return mAffinities;
}

void StrategyDataModel::onGoalDetailsLoaded(const materia::Id id, const std::vector<materia::Task> tasks, const std::vector<materia::Objective> objectives)
{
    emit onGoalDetailsUpdated(id, tasks, objectives);
}
