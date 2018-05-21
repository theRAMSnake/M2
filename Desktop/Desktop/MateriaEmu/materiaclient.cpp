#include "materiaclient.h"
#include <chrono>
#include <thread>
#include <QString>

const std::chrono::milliseconds DELAY(300);

namespace materia
{

class FakeStrategy : public IStrategy
{
public:
    virtual void configureAffinities(const std::vector<Affinity>& affinites)
    {
        std::this_thread::sleep_for(DELAY);

        mAffinities = affinites;
    }

    virtual std::vector<Affinity> getAffinities()
    {
        std::this_thread::sleep_for(DELAY);
        return mAffinities;
    }

    virtual std::tuple<std::vector<Task>, std::vector<Objective>> getGoalItems(const Id& id)
    {
        decltype(getGoalItems(id)) result;

        return result;
    }

    virtual std::vector<Goal> getGoals()
    {
        std::this_thread::sleep_for(DELAY);

        std::vector<Goal> result;

        result.push_back(createGoal({"aff1"}, "g1"));
        result.push_back(createGoal({"aff1"}, "g2"));
        result.push_back(createGoal({"aff1"}, "g3"));

        auto id = result.back().id;
        result.push_back(createGoal({"aff1"}, "g31", id));
        result.push_back(createGoal({"aff1"}, "g32", id));
        result.push_back(createGoal({"aff1"}, "g33", id));
        result.push_back(createGoal({"aff1"}, "g34", id));
        result.push_back(createGoal({"aff1"}, "g34", id));
        result.push_back(createGoal({"aff1"}, "g34", id));
        result.push_back(createGoal({"aff1"}, "g34", id));
        result.push_back(createGoal({"aff1"}, "g34", id));

        result.push_back(createGoal({"aff2"}, "g1"));
        result.push_back(createGoal({"aff2"}, "g2"));

        result.push_back(createGoal({"aff3"}, "g1"));
        result.push_back(createGoal({"aff3"}, "g2"));

        result.push_back(createGoal({"aff4"}, "g1"));
        result.push_back(createGoal({"aff4"}, "g2"));
        result.push_back(createGoal({"aff4"}, "g3"));

        return result;
    }

private:
    Goal createGoal(const Id affId, const std::string& name, const Id& parentId = Id{})
    {
        static int id = 0;
        Goal g;
        g.achieved = false;
        g.affinityId = affId;
        g.focused = false;
        g.id = {QString::number(++id).toStdString()};
        g.name = name;
        g.parentGoalId = parentId;
        g.iconId = {"eye"};

        return g;
    }

    std::vector<Affinity> mAffinities;
};

MateriaClient::MateriaClient(const std::string& clientName, const std::string& ip)
{
    clientName.c_str();
    ip.c_str();
}

IStrategy& MateriaClient::getStrategy()
{
    static FakeStrategy fakeStrategy;
    return fakeStrategy;
}

bool Id::operator <(const Id &other) const
{
    return guid < other.guid;
}

}
