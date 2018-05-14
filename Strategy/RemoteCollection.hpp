#pragma once

#include <Client/IContainer.hpp>

namespace materia
{

template<class T>
T fromJson(const std::string& content);

template<class T>
std::string toJson(const T& t);

template<class T>
class ToJsonSerializer
{
public:
   static T deserialize(const std::string& content)
   {
      return fromJson<T>(content);
   }

   static std::string serialize(const T& src)
   {
      return toJson<T>(src);
   }
};

template<class T, class Serializer = ToJsonSerializer<T>>
class RemoteCollection
{
public:
   class Iterator
   {
   public:
      friend class RemoteCollection<T>;

      bool operator == (const Iterator& other) const
      {
         return other.mImpl == mImpl;
      }

      bool operator != (const Iterator& other) const
      {
         return !operator==(other);
      }

      Iterator& operator ++ ()
      {
         mImpl++;
         return *this;
      }

      const T& operator* ()
      {
         return *mImpl;
      }

      const T* operator-> ()
      {
         return mImpl.operator->();
      }

   protected:
      Iterator(typename std::vector<T>::iterator impl)
      : mImpl(impl)
      {
         
      }

      typename std::vector<T>::iterator mImpl;
   };

   RemoteCollection(const std::string& name, IContainer& container, bool isPrivate = true)
   : mName(name)
   , mContainer(container)
   {
      auto items = mContainer.getItems(mName);

      if(items.empty())
      {
         mContainer.addContainer({name, !isPrivate});
      }

      mLocalCache.reserve(items.size());

      for(auto x : items)
      {
         auto item = Serializer::deserialize(x.content);
         mLocalToRemoteIdMap.insert(std::make_pair(item.id, x.id));
         mLocalCache.push_back(item);
      }
   }

   void insert(const T& item)
   {
      auto insertedIds = mContainer.insertItems(mName, {{ materia::Id::Invalid, Serializer::serialize(item) }});
      if(insertedIds.size() == 1)
      {
         mLocalToRemoteIdMap.insert(std::make_pair(item.id, insertedIds[0]));
         mLocalCache.push_back(item);
      }
   }

   template<class TIterator>
   void insert(const TIterator& begin, const TIterator& end)
   {
      std::vector<ContainerItem> items(std::distance(begin, end));
      std::transform(begin, end, items.begin(), [](auto x)->auto {return ContainerItem{ materia::Id::Invalid, Serializer::serialize(x) }; });

      auto insertedIds = mContainer.insertItems(mName, items);
      if(insertedIds.size() == items.size())
      {
         auto curId = insertedIds.begin();
         for(auto x : items)
         {
            mLocalToRemoteIdMap.insert(std::make_pair(x.id, *curId++));
         }

         mLocalCache.insert(mLocalCache.end(), begin, end);
      }
   }

   void update(const T& item)
   {
      mContainer.replaceItems(mName, {{ mLocalToRemoteIdMap[item.id], Serializer::serialize(item) }});
      *find(mLocalCache, item.id) = item;
   }

   void erase(const Iterator& pos)
   {
      if(mContainer.deleteItems(mName, {{ mLocalToRemoteIdMap[pos->id] }}))
      {
         mLocalToRemoteIdMap.erase(mLocalToRemoteIdMap.find(pos->id));
         mLocalCache.erase(pos.mImpl);
      }
   }

   void clear()
   {
      mContainer.clearContainer(mName);
      mLocalCache.clear();
      mLocalToRemoteIdMap.clear();
   }

   Iterator begin()
   {
      return Iterator(mLocalCache.begin());
   }

   Iterator end()
   {
      return Iterator(mLocalCache.end());
   }

   Iterator find(const Id& id)
   {
      return std::find_if(begin(), end(), [&](auto x)->auto{return x.id == id;});
   }

private:

   std::vector<T> mLocalCache;
   std::map<Id, Id> mLocalToRemoteIdMap;
   std::string mName;
   IContainer& mContainer;
};

}