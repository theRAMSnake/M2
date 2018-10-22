#pragma once

template<class TObject, class TConnection>
class ConnectedObject
{
public:
   ConnectedObject(const TObject& obj, TConnection con)
   : mObj(obj)
   , mCon(con)
   {

   }

   const TObject& get()
   {
      return mObj;
   }

   ~ConnectedObject()
   {
      mCon.disconnect();
   }

private:
   const TObject mObj;
   TConnection mCon;
};