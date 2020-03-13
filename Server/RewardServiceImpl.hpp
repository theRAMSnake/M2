#pragma once

#include <messages/reward.pb.h>
#include <Core/IReward.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

reward::RewardPoolItem toProto(const RewardPoolItem& src)
{
   reward::RewardPoolItem result;

   (*result.mutable_id()) = toProto(src.id);
   result.set_name(src.name);
   result.set_amount(src.amount);
   result.set_amountmax(src.amountMax);

   return result;
}

RewardPoolItem fromProto(const reward::RewardPoolItem& src)
{
   return {fromProto(src.id()), src.name(), src.amount(), src.amountmax()};
}

class RewardServiceImpl : public reward::RewardService
{
public:
   RewardServiceImpl(ICore& core)
   : mReward(core.getReward())
   {
   }

   virtual void GetPools(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::reward::RewardPoolItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mReward.getPools())
      {
         auto item = response->add_items();
         *item = toProto(x);
      }
   }

   virtual void DeletePool(::google::protobuf::RpcController* controller,
                        const ::common::UniqueId* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mReward.removePool(fromProto(*request));
      response->set_success(true);
   }

   virtual void EditPool(::google::protobuf::RpcController* controller,
                        const ::reward::RewardPoolItem* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mReward.modifyPool(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddPool(::google::protobuf::RpcController* controller,
                        const ::reward::RewardPoolItem* request,
                        ::common::UniqueId* response,
                        ::google::protobuf::Closure* done)
   {
      *response = toProto(mReward.addPool(fromProto(*request)));
   }

   virtual void AddPoints(::google::protobuf::RpcController* controller,
                        const ::common::IntMessage* request,
                        ::common::OperationResultMessage* response,
                        ::google::protobuf::Closure* done)
   {
      mReward.addPoints(request->content());
      response->set_success(true);
   }

private:
   materia::IReward& mReward;
};

}