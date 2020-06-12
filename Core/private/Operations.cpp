#include "Operations.hpp"
#include "fmt/format.h"
#include <boost/date_time/date_duration_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace materia
{

Time advance(const Time src, const int recType)
{
    Time result = src;

    switch (recType)
    {
    case 1:
        result.value += 604800;
        break;

    case 2:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(1));
        break;

    case 3:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(3));
        break;

    case 4:
        result.value = boost::posix_time::to_time_t(boost::posix_time::from_time_t(result.value) + boost::gregorian::months(12));
        break;
    
    default:
        break;
    }

    return result;
}

void complete(const Id id, ObjectManager& om)
{
    auto objectPtr = om.get(id);
    auto object = *objectPtr;

    if(object.getTypeName() != "calendar_item")
    {
        throw std::runtime_error(fmt::format("Type {} is not completable", object.getTypeName()));
    }

    auto eType = static_cast<int>(object["entityType"]);
    auto recType = static_cast<int>(object["reccurencyType"]);
    if(eType == 1/*task*/)
    {
        om.LEGACY_getReward().addPoints(1);
    }

    if(recType != 0/*none*/)
    {
        object["timestamp"] = advance(static_cast<Time>(object["timestamp"]), recType);
        om.modify(object);
    }
    else
    {
        om.destroy(id);
    }
}

}