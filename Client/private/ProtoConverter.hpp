#pragma once

common::UniqueId toProto(const Id& id);
Id fromProto(const common::UniqueId& protoId);