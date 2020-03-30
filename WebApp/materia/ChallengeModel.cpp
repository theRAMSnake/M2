#include "ChallengeModel.hpp"

ChallengeModel::ChallengeModel(ZmqPbChannel& channel)
: mService(channel)
{

}

std::vector<ChallengeModel::Item> ChallengeModel::get()
{
    common::EmptyMessage e;
    challenge::ChallengeItems items;
    mService.getService().GetItems(nullptr, &e, &items, nullptr);

    std::vector<CalendarModel::Item> result;

    for(auto x : items.items())
    {
        CalendarModel::Item r;

        result.push_back(r);
    }

    return result;
}