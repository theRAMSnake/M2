#include "Challenge.hpp"

BIND_JSON1(materia::StagesLayer, stages)

SERIALIZE_AS_INTEGER(materia::PointsLayerType)
BIND_JSON4(materia::PointsLayer, numPoints, pointsToNextLevel, advancementValue, type)

BIND_JSON2(materia::ChallengeLayer, id, parameters)
BIND_JSON5(materia::ChallengeItem, id, name, level, maxLevels, layers)

namespace materia
{

Challenge::Challenge(Database& db)
: mStorage(db.getTable("challenge"))
{
    
}

std::vector<ChallengeItem> Challenge::get() const
{
    std::vector<ChallengeItem> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {
        result.push_back(readJson<Challenge>(json));
    });

    return result;
}

void Challenge::removeChallenge(const Id& id)
{
    mStorage->erase(id);
}

Id Challenge::addChallenge(const std::string& name, const unsigned int maxLevels)
{
   auto newItem = ChallengeItem{Id::generate(), name, 1, maxLevels};

   mStorage->store(newItem.eventId, writeJson(newItem));

   return newItem.eventId;
}

Id Challenge::addLayer(const Id& id, const ChallengeLayer& layer)
{
    makeItemOperation(id, [=](auto& i)
    {
        auto newLayer = layer;
        newLayer.id = Id::generate();

        i.layers.push_back(newLayer);

        return true;
    });
}

void Challenge::removeLayer(const Id& challengeId, const Id& layerId)
{
    makeItemOperation(challengeId, [=](auto& i)
    {
        auto pos = find_by_id(i.layers, layerId);

        if(pos != i.layers.end())
        {
            i.layers.erase(pos);
            return true;
        }

        return false;
    });
}

void Challenge::toggleStage(const Id& challengeId, const Id& layerId, const unsigned int ordinalNumber)
{
    makeItemOperation(challengeId, [=](auto& i)
    {
        auto pos = find_by_id(i.layers, layerId);

        if(pos != i.layers.end() && std::holds_alternative<StageLayer>(*pos))
        {
            auto& layer = std::get<StageLayer>(*pos);

            if(ordinalNumber < layer.stages.size())
            {
                layer.stages[ordinalNumber] = !layer.stages[ordinalNumber];
            }

            return true;
        }

        return false;
    });
}

void Challenge::addPoints(const Id& challengeId, const Id& layerId, const unsigned int points)
{
    makeItemOperation(challengeId, [=](auto& i)
    {
        auto pos = find_by_id(i.layers, layerId);

        if(pos != i.layers.end() && std::holds_alternative<PointsLayer>(*pos))
        {
            auto& layer = std::get<PointsLayer>(*pos);

            layer.numPoints += points;

            return true;
        }

        return false;
    });
}

void Challenge::resetWeekly()
{
    makeItemOperation(challengeId, [=](auto& i)
    {
        bool hasChanges = false;
        for(auto& l : i.layers)
        {
            if(std::holds_alternative<PointsLayer>(l))
            {
                auto& layer = std::get<PointsLayer>(l);

                if(layer.type == PointsLayerType::Weekly)
                {
                    layer.numPoints = 0;
                    hasChanges = true;
                }
            }
        }

        return hasChanges;
    });
}

bool Challenge::isItemComplete(const ChallengeItem& item) const
{
    if(item.layers.empty())
    {
        return false;
    }

    for(auto& l : item.layers)
    {
        if(std::holds_alternative<PointsLayer>(l))
        {
            auto& layer = std::get<PointsLayer>(l);

            if(layer.numPoints < layer.pointsToNextLevel)
            {
                return false;
            }
        }
        else if(std::holds_alternative<StageLayer>(l))
        {
            auto& layer = std::get<StageLayer>(l);

            if(std::find(layer.stages.begin(), layer.stages.end(), false) != layer.stages.end())
            {
                return false;
            }
        }
        else
        {
            throw -1;
        }
    }

    return true;
}

void Challenge::advance(ChallengeItem& item) const
{
    if(item.level < item.maxLevel)
    {
        item.level++;
    }
    
    for(auto& l : item.layers)
    {
        if(std::holds_alternative<PointsLayer>(l))
        {
            auto& layer = std::get<PointsLayer>(l);

            layer.numPoints -= layer.pointsToNextLevel;
            layer.pointsToNextLevel += layer.advancementValue;
        }
        else if(std::holds_alternative<StageLayer>(l))
        {
            auto& layer = std::get<StageLayer>(l);

            for(auto& s : layer.stages)
            {
                s = false;
            }
        }
        else
        {
            throw -1;
        }
    }

    return true;
}

}