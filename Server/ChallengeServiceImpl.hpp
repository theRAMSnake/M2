#pragma once

#include <messages/challenge.pb.h>
#include <Core/IChallenge.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

class ChallengeServiceImpl : public challenge::ChallengeService
{
public:
   ChallengeServiceImpl(ICore& core)
   : mCh(core.getChallenge())
   {
   }

   virtual void GetItems(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::challenge::ChallengeItems* response,
                       ::google::protobuf::Closure* done)
   {
      for(auto x : mCh.get())
      {
         auto item = response->add_items();
         *item = toProto(x);
      }
   }

   virtual void DeleteItem(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCh.removeChallenge(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddItem(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::AddChallengeParams* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      *response = toProto(mCh.addChallenge(request->name(), request->maxLevels()));
   }

   virtual void DeleteLayer(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::LayerId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCh.removeLayer(fromProto(request->a()), fromProto(request->b()));
      response->set_success(true);
   }

   virtual void AddLayer(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::AddLayerParams* request,
                       ::common::LayerId* response,
                       ::google::protobuf::Closure* done)
   {
      auto newLayerId = mCh.addLayer(fromProto(request->id()), fromProto(request->layer()));
      *(response->mutable_layer_id()->mutable_a()) = request->id();
      *(response->mutable_layer_id()->mutable_b()) = toProto(newLayerId);
   }

   virtual void ToggleStage(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::ToggleStageParams* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCg.toggleStage(fromProto(request->id().a()), fromProto(request->id().b()), request->ordinal_number());
      response->set_success(true);
   }

   virtual void AddPoints(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::AddPointsParams* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCg.addPoints(fromProto(request->id().a()), fromProto(request->id().b()), request->points());
      response->set_success(true);
   }

private:
   materia::IChallenge& mChallenge;
};