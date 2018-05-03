#pragma once

class TestEventListener
{
public:
   TestEventListener(materia::Events events)
   : mEvents(events)
   {
      mStartTime = boost::posix_time::microsec_clock::local_time();
   }

   template<materia::EventType EV_TYPE>
   bool hasEvent(const typename EventProperties<EV_TYPE>::Value& props)
   {
      struct : public materia::IEventHandler
      {
         virtual void onGenericEvent(const materia::Event& event)
         {
            found = EV_TYPE == event.type;
         }
         virtual void onContainerUpdated(const materia::ContainerUpdatedEvent& event)
         {
            found = mContainerName == event.containerName && EV_TYPE == event.type;
         }
         virtual void onIdEvent(const materia::IdEvent& event)
         {
            found = mId == event.id && EV_TYPE == event.type;
         }

         materia::Id mId;
         std::string mContainerName;
         bool found = false;
      } evHdr;

      evHdr.mId = id;
      evHdr.mContainerName = containerName;
      mEvents.getEvents(mStartTime, evHdr);
      return evHdr.found;
   }

private:
   materia::Events& mEvents;
   boost::posix_time::ptime mStartTime;
};
