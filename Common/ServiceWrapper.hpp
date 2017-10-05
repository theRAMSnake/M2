#pragma once
#include <exception>
#include <messages/common.pb.h>
#include <google/protobuf/service.h>

namespace materia
{
   
class CannotProvideServiceException : public std::exception{};
class MethodNotImplementedException : public std::exception{};
   
template<class TServiceProvider>
class ServiceWrapper : public google::protobuf::RpcController
{
public:
   ServiceWrapper(TServiceProvider& provider)
   : mProvider(provider)
   , mEmptyCb(google::protobuf::NewCallback(&google::protobuf::DoNothing))
   {
   }
   
   common::MateriaMessage sendMessage(const common::MateriaMessage& msg)
   {
      auto method = mProvider.descriptor()->FindMethodByName(msg.operationname());
      if(method != nullptr)
      {
         std::unique_ptr<google::protobuf::Message> request(mProvider.GetRequestPrototype(method).New());
         request->ParseFromString(msg.payload());
         
         std::unique_ptr<google::protobuf::Message> responce(mProvider.GetResponsePrototype(method).New());
         
         common::MateriaMessage result;
         result.set_from(mProvider.descriptor()->name());
         result.set_to(msg.from());
         result.set_operationname(msg.operationname());
         result.set_request_id(msg.request_id());
         
         try
         {
            mProvider.CallMethod(method, this, request.get(), responce.get(), mEmptyCb.get());
            responce->SerializeToString(result.mutable_payload());
         }
         catch(MethodNotImplementedException& ex)
         {
            result.set_error("Method not implemented");
         }
         
         return result;
      }
      else
      {
         throw CannotProvideServiceException();
      }
   }
   
   virtual void Reset()
   {
      //unused
   }

   virtual bool Failed() const
   {
      //unused
      return false;
   }

   virtual std::string ErrorText() const
   {
      //unused
      return "";
   }

   virtual void StartCancel()
   {
      //unused
   }

   virtual void SetFailed(const std::string& reason)
   {
      throw MethodNotImplementedException();
   }

   virtual bool IsCanceled() const
   {
      //unused
      return false;
   }

   virtual void NotifyOnCancel(google::protobuf::Closure* callback)
   {
      //unused
   }
   
private:
   TServiceProvider& mProvider;
   std::unique_ptr<google::protobuf::Closure> mEmptyCb;
};
   
}
