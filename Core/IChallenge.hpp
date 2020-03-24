#pragma once

#include <vector>
#include <variant>
#include "Common/Id.hpp"

namespace materia
{

struct StagesLayer
{
    std::vector<bool> stages;
};

enum class PointsLayerType
{
    Total,
    Weekly
};

struct PointsLayer
{
    unsigned int numPoints;
    unsigned int pointsToNextLevel;
    unsigned int advancementValue;

    PointsLayerType type;
};

struct ChallengeLayer
{
    Id id;
    std::variant<StagesLayer, PointsLayer> parameters;
};

struct ChallengeItem
{
    Id id;
    std::string name;
    unsigned int level;
    unsigned int maxLevels;

    std::vector<ChallengeLayer> layers;
};

class IChallenge
{
public:
    virtual std::vector<ChallengeItem> get() const = 0;

    virtual void removeChallenge(const Id& id) = 0;
    virtual Id addChallenge(const std::string& name, const unsigned int maxLevels) = 0;

    virtual Id addLayer(const Id& id, const ChallengeLayer& item) = 0;
    virtual void removeLayer(const Id& challengeId, const Id& layerId) = 0;

    virtual void toggleStage(const Id& challengeId, const Id& layerId, const unsigned int ordinalNumber) = 0;
    virtual void addPoints(const Id& challengeId, const Id& layerId, const unsigned int points) = 0;

    virtual ~IChallenge(){}
};

}