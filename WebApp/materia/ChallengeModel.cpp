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

    std::vector<ChallengeModel::Item> result;

    for(auto x : items.items())
    {
        ChallengeModel::Item r {x.id(), x.name(), x.level(), x.levelMax()};

        for(auto& l : x.layers())
        {
            if(l.has_stage_layer())
            {
                StagesLayer newLayer;

                for(auto s : l.stage_layer().stages())
                {
                    newLayer.push_back(s);
                }

                r.layers.push_back(newLayer);
            }
            else if(l.has_points_layer())
            {
                auto& src = l.pointsLayer;
                PointsLayer pointsLayer {src.points(), src.pointsNeeded(), src.pointsAdvance(), src.type()};
                r.layers.push_back(pointsLayer);
            }
        }

        result.push_back(r);
    }

    mItems = result;

    return result;
}

void ChallengeModel::eraseLayer(materia::Id chId, materia::Id lId)
{
    challenge::LayerId id;
    id.mutable_a()->set_guid(chId.getGuid());
    id.mutable_b()->set_guid(lId.getGuid());

    common::OperationResultMessage msg;
    mService.getService().DeleteLayer(nullptr, &id, &msg, nullptr);

    changed();
}

void ChallengeModel::apply(materia::Id chId, materia::Id lId, const PointsLayer& p)
{
    challenge::LayerId id;
    id.mutable_a()->set_guid(chId.getGuid());
    id.mutable_b()->set_guid(lId.getGuid());

    auto chPos = materia::find_by_id(mItems, chId);

    changed();
}

void ChallengeModel::apply(materia::Id chId, materia::Id lId, const StagesLayer& p)
{
    //SNAKE
}