#include "Core.hpp"

namespace materia
{

IInbox& Core::getInbox() const
{
   return mInbox;
}

ICalendar& Core::getCalendar() const
{
   return mCalendar;
}

IStrategy& Core::getStrategy() const
{
   return mStrategy;
}

IJournal& Core::getJournal() const
{
   return mJournal;
}

std::shared_ptr<ICore> createCore()
{
   return new Core();
}

}