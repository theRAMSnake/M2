#include "materiaclient.h"
#include <chrono>
#include <thread>
#include <QString>
#include <QtGlobal>
#include <QDateTime>

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
        std::this_thread::sleep_for(DELAY);
        return mGoalItems[id];
    }

    virtual std::vector<Goal> getGoals()
    {
        std::this_thread::sleep_for(DELAY);

        std::vector<Goal> result;

        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));

        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));
        result.push_back(createGoal({"aff1"}));

        result.push_back(createGoal({"aff2"}));
        result.push_back(createGoal({"aff2"}));

        result.push_back(createGoal({"aff3"}));
        result.push_back(createGoal({"aff3"}));

        result.push_back(createGoal({"aff4"}));
        result.push_back(createGoal({"aff4"}));
        result.push_back(createGoal({"aff4"}));

        return result;
    }

private:
    std::string genRandomName()
    {
        std::vector<std::string> names{
            "Einstein",
            "Newton",
            "Hawking",
            "Darwin",
            "Tesla",
            "Galilei",
            "Curie",
            "Faraday",
            "Aristotle",
            "Edison",
            "Pasteur",
            "Feynman",
            "Da vinci"
        };

        return names[qrand() % names.size()];
    }

    Goal createGoal(const Id affId)
    {
        static int id = 0;
        Goal g;
        g.achieved = qrand() % 2;
        g.affinityId = affId;
        g.focused = qrand() % 2;
        g.id = {QString::number(++id).toStdString()};
        g.name = genRandomName();
        g.iconId = {"eye"};

        std::tuple<std::vector<Task>, std::vector<Objective>> goalItems;

        std::vector<Task> tasks;
        int numTasks = qrand() % 6;
        for(int i = 0; i < numTasks; ++i)
        {
            Task t;
            t.done = qrand() % 2;
            t.id = {QString::number(++id).toStdString()};
            t.name = genRandomName();

            tasks.push_back(t);
        }

        std::vector<Objective> objectives;
        int numObjectives = qrand() % 6;
        for(int i = 0; i < numObjectives; ++i)
        {
            Objective o;
            o.id = {QString::number(++id).toStdString()};
            o.name = genRandomName();
            o.reached = qrand() % 2;

            objectives.push_back(o);
        }

        std::get<0>(goalItems) = tasks;
        std::get<1>(goalItems) = objectives;

        mGoalItems[g.id] = goalItems;

        return g;
    }

    std::map<Id, std::tuple<std::vector<Task>, std::vector<Objective>>> mGoalItems;
    std::vector<Affinity> mAffinities;
};

MateriaClient::MateriaClient(const std::string& clientName, const std::string& ip)
{
    clientName.c_str();
    ip.c_str();

    qsrand(QDateTime::currentMSecsSinceEpoch() / 1000);
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
