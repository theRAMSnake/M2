#include "strategydatamodel.h"

StrategyDataModel::StrategyDataModel(MateriaGateway& materiaGateway)
    : mMateriaGateway(materiaGateway)
{
    connect(&materiaGateway, SIGNAL(onAffinitiesLoaded(const std::vector<materia::Affinity>)), this, SLOT(onAffinitiesLoaded(const std::vector<materia::Affinity>)));
    connect(&materiaGateway, SIGNAL(onGoalsLoaded(const std::vector<materia::Goal>)), this, SLOT(onGoalsLoaded(const std::vector<materia::Goal>)));
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

    mMateriaGateway.loadGoals();
}

void StrategyDataModel::onGoalsLoaded(const std::vector<materia::Goal> goals)
{
    for(auto g : goals)
    {
        if(g.focused)
        {
            mMateriaGateway.loadGoalDetails(g.id);
            emit onGoalUpdated(g);
        }
    }

    for(auto g : goals)
    {
        if(!g.focused)
        {
            mMateriaGateway.loadGoalDetails(g.id);
            emit onGoalUpdated(g);
        }
    }
}

void StrategyDataModel::init()
{
    mMateriaGateway.loadAffinities();
}
