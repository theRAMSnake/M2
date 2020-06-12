#pragma once

#include "../dialog/CommonDialogManager.hpp"
#include "../materia/StrategyModel.hpp"

class INodeTypeSpecifics;

class IOperationProvider
{
public:
    virtual void modify(const StrategyModel::Node& n) = 0;
    virtual void clone(const StrategyModel::Node& n) = 0;
    virtual void split(const StrategyModel::Node& n) = 0;

    virtual ~IOperationProvider() {};
};

class NodeEditDialog: public BasicDialog
{
public:
   typedef std::function<void(const StrategyModel::Node&)> TCallback;

   NodeEditDialog(
       const StrategyModel::Node& node,
       const std::vector<StrategyModel::WatchItem>& watchItems, 
       const std::vector<StrategyModel::Goal>& goals, 
       const std::vector<ChallengeModel::Item>& chs,
       IOperationProvider& opProvider
       );

    bool verify() override;

private:

    std::unique_ptr<INodeTypeSpecifics> mNodeTypeSpecifics;
    IOperationProvider& mOpProvider;
};