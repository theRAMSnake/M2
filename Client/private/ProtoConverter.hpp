#pragma once

#include "../IStrategy.hpp"
#include "../ICalendar.hpp"
#include "../IActions.hpp"
#include "messages/strategy.pb.h"
#include "messages/calendar.pb.h"
#include "messages/actions.pb.h"

namespace materia
{

common::UniqueId toProto(const Id& id);
Id fromProto(const common::UniqueId& protoId);

strategy::Goal toProto(const materia::Goal& x);
materia::Goal fromProto(const strategy::Goal& x);

strategy::Task toProto(const materia::Task& x);
materia::Task fromProto(const strategy::Task& x);

strategy::Objective toProto(const materia::Objective& x);
materia::Objective fromProto(const strategy::Objective& x);

strategy::Measurement toProto(const materia::Measurement& x);
materia::Measurement fromProto(const strategy::Measurement& x);

strategy::Affinity toProto(const materia::Affinity& x);
materia::Affinity fromProto(const strategy::Affinity& x);

calendar::CalendarItem toProto(const CalendarItem& in);
CalendarItem fromProto(const calendar::CalendarItem& in);

actions::ActionInfo toProto(const ActionItem& item);
materia::ActionItem fromProto(const actions::ActionInfo& x);

}
