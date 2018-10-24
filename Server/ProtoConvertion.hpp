#pragma once

#include <messages/common.pb.h>
#include <Core/Id.hpp>

namespace materia
{

common::UniqueId toProto(const Id& id);
Id fromProto(const common::UniqueId& protoId);

}