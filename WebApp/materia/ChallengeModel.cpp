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
        ChallengeModel::Item r {x.id().guid(), x.name(), x.level(), x.maxlevels()};

        for(auto& l : x.layers())
        {
            if(l.has_stage_layer())
            {
                StagesLayer newLayer;

                for(auto s : l.stage_layer().stages())
                {
                    newLayer.stages.push_back(s);
                }

                r.layers.push_back({l.id().b().guid(), newLayer});
            }
            else if(l.has_points_layer())
            {
                auto& src = l.points_layer();
                PointsLayer pointsLayer {src.numpoints(), src.pointstonextlevel(), src.advancementvalue(), src.type()};
                r.layers.push_back({l.id().b().guid(), pointsLayer});
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
    if(p.newPoints != 0)
    {
        challenge::AddPointsParams pts;
        pts.mutable_id()->mutable_a()->set_guid(chId.getGuid());
        pts.mutable_id()->mutable_b()->set_guid(lId.getGuid());

        pts.set_points(p.newPoints);

        common::OperationResultMessage msg;
        mService.getService().AddPoints(nullptr, &pts, &msg, nullptr);

        changed();
    }
}

void ChallengeModel::apply(materia::Id chId, materia::Id lId, const StagesLayer& l)
{
    challenge::ToggleStageParams p;
    p.mutable_id()->mutable_a()->set_guid(chId.getGuid());
    p.mutable_id()->mutable_b()->set_guid(lId.getGuid());

    auto& oldLayer = std::get<StagesLayer>(materia::find_by_id(materia::find_by_id(mItems, chId)->layers, lId)->params);

    for(std::size_t i = 0; i < oldLayer.stages.size(); ++i)
    {
        if(oldLayer.stages[i] != l.stages[i])
        {
            p.set_ordinal_number(i);
            common::OperationResultMessage msg;
            mService.getService().ToggleStage(nullptr, &p, &msg, nullptr);
        }
    }

    changed();
}

void ChallengeModel::createChallenge(const std::string& name, const unsigned int maxLevel)
{
    challenge::AddChallengeParams p;
    p.set_name(name);
    p.set_maxlevels(maxLevel);

    common::UniqueId msg;
    mService.getService().AddItem(nullptr, &p, &msg, nullptr);

    changed();
}

void ChallengeModel::eraseChallenge(materia::Id chId)
{
    common::UniqueId id;
    id.set_guid(chId.getGuid());

    common::OperationResultMessage msg;
    mService.getService().DeleteItem(nullptr, &id, &msg, nullptr);

    changed();
}

void ChallengeModel::createPointsLayer(materia::Id chId, const PointsLayer& p)
{
    challenge::AddLayerParams prms;
    prms.mutable_parentid()->set_guid(chId);
    auto& dst = *prms.mutable_layer()->mutable_points_layer();

    dst.set_pointstonextlevel(p.pointsNeeded);
    dst.set_advancementvalue(p.pointsAdvance);
    dst.set_type(p.type);

    challenge::LayerId result;
    mService.getService().AddLayer(nullptr, &prms, &result, nullptr);
}

void ChallengeModel::createStagesLayer(materia::Id chId, const unsigned int& numStages)
{
    challenge::AddLayerParams prms;
    prms.mutable_parentid()->set_guid(chId);
    auto& dst = *prms.mutable_layer()->mutable_stage_layer();

    for(unsigned int i = 0; i < numStages; ++i)
    {
        dst.add_stages(false);
    }

    challenge::LayerId result;
    mService.getService().AddLayer(nullptr, &prms, &result, nullptr);
}