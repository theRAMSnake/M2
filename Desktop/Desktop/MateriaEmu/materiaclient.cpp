#include "materiaclient.h"
#include <chrono>
#include <thread>

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

    virtual std::vector<Goal> getGoals()
    {
        std::this_thread::sleep_for(DELAY);

        std::vector<Goal> result;

        Goal g;
        g.achieved = false;
        g.affinityId = {"aff1"};
        g.focused = false;
        g.id = {"g1"};
        g.name = "goal1";

        result.push_back(g);

        g.achieved = false;
        g.affinityId = {"aff2"};
        g.focused = true;
        g.id = {"g2"};
        g.name = "goal2";

        result.push_back(g);

        g.achieved = false;
        g.affinityId = {"aff3"};
        g.focused = true;
        g.id = {"g3"};
        g.name = "goal3";

        result.push_back(g);

        g.achieved = false;
        g.affinityId = {"aff4"};
        g.focused = false;
        g.id = {"g4"};
        g.name = "goal4";

        result.push_back(g);

        return result;
    }

private:
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

}
