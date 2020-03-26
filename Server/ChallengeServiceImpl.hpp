#pragma once

#include <messages/challenge.pb.h>
#include <Core/IChallenge.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

void morphProto(challenge::Layer& l, const PointsLayer& item)
{
   auto& result = (*l.mutable_points_layer());

   result.set_advancementvalue(item.advancementValue);
   result.set_numpoints(item.numPoints);
   result.set_type(static_cast<challenge::PointsLayerType>(item.type));
   result.set_pointstonextlevel(item.pointsToNextLevel);
}

void morphProto(challenge::Layer& l, const StagesLayer& item)
{
   for(auto s : item.stages)
   {
      l.mutable_stage_layer()->add_stages(s);
   }
}

challenge::Layer toProto(const Id& challengeId, const ChallengeLayer& item)
{
   challenge::Layer result;

   (*result.mutable_id()->mutable_a()) = toProto(challengeId);
   (*result.mutable_id()->mutable_b()) = toProto(item.id);

   std::visit([&](auto&& arg) {
         morphProto(result, arg);
      }, item.parameters);

   return result;
}

ChallengeLayer fromProto(const challenge::Layer& l)
{
   ChallengeLayer result;
   result.id = fromProto(l.id().b());

   if(l.has_points_layer())
   {
      auto& src = l.points_layer();
      PointsLayer pts {src.numpoints(), src.pointstonextlevel(), src.advancementvalue(), static_cast<PointsLayerType>(src.type())};

      result.parameters = pts;
   }
   else if(l.has_stage_layer())
   {
      StagesLayer st;
      for(auto s : l.stage_layer().stages())
      {
         st.stages.push_back(s);
      }

      result.parameters = st;
   }

   return result;
}

challenge::Item toProto(const ChallengeItem& item)
{
   challenge::Item result;

   (*result.mutable_id()) = toProto(item.id);
   result.set_level(item.level);
   result.set_maxlevels(item.maxLevels);
   result.set_name(item.name);

   for(auto& l : item.layers)
   {
      auto rl = result.add_layers();
      *rl = toProto(item.id, l);
   }

   return result;
}

class ChallengeServiceImpl : public challenge::ChallengeService
{
public:
   ChallengeServiceImpl(ICore& core)
   : mCh(core.getChallenge())
   {
   }

   virtual void GetItems(::google::protobuf::RpcController* controller,
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

   virtual void DeleteItem(::google::protobuf::RpcController* controller,
                       const ::common::UniqueId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCh.removeChallenge(fromProto(*request));
      response->set_success(true);
   }

   virtual void AddItem(::google::protobuf::RpcController* controller,
                       const ::challenge::AddChallengeParams* request,
                       ::common::UniqueId* response,
                       ::google::protobuf::Closure* done)
   {
      *response = toProto(mCh.addChallenge(request->name(), request->maxlevels()));
   }

   virtual void DeleteLayer(::google::protobuf::RpcController* controller,
                       const ::challenge::LayerId* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCh.removeLayer(fromProto(request->a()), fromProto(request->b()));
      response->set_success(true);
   }

   virtual void AddLayer(::google::protobuf::RpcController* controller,
                       const ::challenge::AddLayerParams* request,
                       ::challenge::LayerId* response,
                       ::google::protobuf::Closure* done)
   {
      auto newLayerId = mCh.addLayer(fromProto(request->parentid()), fromProto(request->layer()));
      *(response->mutable_layer_id()->mutable_a()) = request->id();
      *(response->mutable_layer_id()->mutable_b()) = toProto(newLayerId);
   }

   virtual void ToggleStage(::google::protobuf::RpcController* controller,
                       const ::challenge::ToggleStageParams* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCh.toggleStage(fromProto(request->id().a()), fromProto(request->id().b()), request->ordinal_number());
      response->set_success(true);
   }

   virtual void AddPoints(::google::protobuf::RpcController* controller,
                       const ::challenge::AddPointsParams* request,
                       ::common::OperationResultMessage* response,
                       ::google::protobuf::Closure* done)
   {
      mCh.addPoints(fromProto(request->id().a()), fromProto(request->id().b()), request->points());
      response->set_success(true);
   }

private:
   materia::IChallenge& mCh;
};