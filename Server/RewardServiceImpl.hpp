#pragma once

#include <messages/reward.pb.h>
#include <Core/IReward.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

class RewardServiceImpl : public reward::RewardService
{
public:
   RewardServiceImpl(ICore& core)
   : mReward(core.getReward())
   {
   }

private:
   materia::IReward& mReward;
};

}