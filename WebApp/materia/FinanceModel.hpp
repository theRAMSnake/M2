#pragma once

#include <Common/Id.hpp>
#include <messages/finance.pb.h>
#include "ZmqPbChannel.hpp"
#include "MateriaServiceProxy.hpp"

class FinanceModel
{
public:
   FinanceModel(ZmqPbChannel& channel);

private:
   MateriaServiceProxy<finance::FinanceService> mService;
};