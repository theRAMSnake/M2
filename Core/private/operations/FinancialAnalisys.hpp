#pragma once

namespace materia
{

class ObjectManager;
class IReward;

namespace types
{

class SimpleList;

}

void performFinancialAnalisys(ObjectManager& objMan, IReward& reward, types::SimpleList& inbox);

}