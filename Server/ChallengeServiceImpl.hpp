#pragma once

#include <messages/challenge.pb.h>
#include <Core/IChallenge.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

class ChallengeServiceImpl : public challenge::ChallengeService
{
public:
   InboxServiceImpl(ICore& core)
   : mInbox(core.getInbox())
   {
   }

   virtual void GetItems(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::challenge::ChallengeItems* response,
                       ::google::protobuf::Closure* done);
  virtual void DeleteItem(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done);
  virtual void AddItem(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::AddChallengeParams* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done);
  virtual void DeleteLayer(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::common::IdSet* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done);
  virtual void AddLayer(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::LayerId* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done);
  virtual void ToggleStage(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::ToggleStageParams* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done);
  virtual void AddPoints(::PROTOBUF_NAMESPACE_ID::RpcController* controller,
                       const ::challenge::AddPointsParams* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done);
};