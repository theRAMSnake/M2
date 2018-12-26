#pragma once

#include <messages/admin.pb.h>
#include <Core/IBackuper.hpp>
#include "ProtoConvertion.hpp"

namespace materia
{

class AdminServiceImpl : public admin::AdminService
{
public:
   AdminServiceImpl(ICore& core)
   : mBackuper(core.getBackuper())
   {
   }

    virtual void StartBackup(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::admin::BackupChunk* response,
                       ::google::protobuf::Closure* done)
    {
        mBackuper.start();

        std::vector<char> out;
        response->set_has_more(mBackuper.getNextChunk(out));
        response->set_bytes(&out.front(), out.size());
    }

    virtual void Next(::google::protobuf::RpcController* controller,
                       const ::common::EmptyMessage* request,
                       ::admin::BackupChunk* response,
                       ::google::protobuf::Closure* done)
    {
        std::vector<char> out;
        response->set_has_more(mBackuper.getNextChunk(out));
        response->set_bytes(&out.front(), out.size());
    }

private:
   materia::IBackuper& mBackuper;
};

}