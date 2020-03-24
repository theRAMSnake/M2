#pragma once

#include <Common/Id.hpp>
#include <messages/challenge.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

class ChallengeModel
{
public:
   ChallengeModel(ZmqPbChannel& channel);

private:
   MateriaServiceProxy<reward::ChallengeService> mService;
};