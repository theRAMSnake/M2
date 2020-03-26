#include "Challenge.hpp"
#include "JsonSerializer.hpp"

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
        result.push_back(readJson<ChallengeItem>(json));
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

   mStorage->store(newItem.id, writeJson(newItem));

   return newItem.id;
}

Id Challenge::addLayer(const Id& id, const ChallengeLayer& layer)
{
    Id result;

    makeItemOperation(id, [&](auto& i)
    {
        auto newLayer = layer;
        newLayer.id = Id::generate();
        result = newLayer.id;

        i.layers.push_back(newLayer);

        return true;
    });

    return result;
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

        if(pos != i.layers.end() && std::holds_alternative<StagesLayer>(pos->parameters))
        {
            auto& layer = std::get<StagesLayer>(pos->parameters);

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

        if(pos != i.layers.end() && std::holds_alternative<PointsLayer>(pos->parameters))
        {
            auto& layer = std::get<PointsLayer>(pos->parameters);

            layer.numPoints += points;

            return true;
        }

        return false;
    });
}

void Challenge::resetWeekly()
{
    auto items = get();
    for(auto& i : items)
    {
        makeItemOperation(i.id, [=](auto& i)
        {
            bool hasChanges = false;
            for(auto& l : i.layers)
            {
                if(std::holds_alternative<PointsLayer>(l.parameters))
                {
                    auto& layer = std::get<PointsLayer>(l.parameters);

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
}

bool Challenge::isItemComplete(const ChallengeItem& item) const
{
    if(item.layers.empty())
    {
        return false;
    }

    for(auto& l : item.layers)
    {
        if(std::holds_alternative<PointsLayer>(l.parameters))
        {
            auto& layer = std::get<PointsLayer>(l.parameters);

            if(layer.numPoints < layer.pointsToNextLevel)
            {
                return false;
            }
        }
        else if(std::holds_alternative<StagesLayer>(l.parameters))
        {
            auto& layer = std::get<StagesLayer>(l.parameters);

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
    if(item.level < item.maxLevels)
    {
        item.level++;
    }
    
    for(auto& l : item.layers)
    {
        if(std::holds_alternative<PointsLayer>(l.parameters))
        {
            auto& layer = std::get<PointsLayer>(l.parameters);

            layer.numPoints -= layer.pointsToNextLevel;
            layer.pointsToNextLevel += layer.advancementValue;
        }
        else if(std::holds_alternative<StagesLayer>(l.parameters))
        {
            auto& layer = std::get<StagesLayer>(l.parameters);

            for(std::size_t i = 0; i < layer.stages.size(); ++i)
            {
                layer.stages[i] = false;
            }
        }
        else
        {
            throw -1;
        }
    }
}

ChallengeItem Challenge::readItem(const std::string& json) const
{
    return readJson<ChallengeItem>(json);
}

std::string Challenge::writeItem(const ChallengeItem& item) const
{
    return writeJson(item);
}

}