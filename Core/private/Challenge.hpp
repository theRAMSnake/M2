#pragma once

#include "../IChallenge.hpp"
#include "Database.hpp"

namespace materia
{

class Database;
class Challenge : public IChallenge
{
public:
    Challenge(Database& db);

    std::vector<ChallengeItem> get() const override;

    void removeChallenge(const Id& id) override;
    Id addChallenge(const std::string& name, const unsigned int maxLevels) override;

    Id addLayer(const Id& id, const ChallengeLayer& item) override;
    void removeLayer(const Id& challengeId, const Id& layerId) override;

    void toggleStage(const Id& challengeId, const Id& layerId, const unsigned int ordinalNumber) override;
    void addPoints(const Id& challengeId, const Id& layerId, const unsigned int points) override;

    void resetWeekly();

private:
    template<class F>
    void makeItemOperation(const Id& itemId, F f)
    {
        auto ch = mStorage->load(id);
        if(ch)
        {
            auto item = readJson<ChallengeItem>(*ch);
            if(f(item))
            {
                if(isItemComplete(item))
                {
                    advance(item);
                }

                mStorage->store(id, writeJson(item));
            }
        }
    }

    bool isItemComplete(const ChallengeItem& item) const;
    void advance(ChallengeItem& item) const;

    std::unique_ptr<DatabaseTable> mStorage;
};

}