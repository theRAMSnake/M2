#pragma once

namespace materia
{

template<class T>
class ToJsonSerializer
{
public:
   static T deserialize(const std::string& content)
   {
      return fromJson(content);
   }

   static std::string serialize(const T& src)
   {
      return toJson(src);
   }
}

template<class T, class Serializer = ToJsonSerializer>
class RemoteCollection
{
public:
   class Iterator
   {
      friend class RemoteCollection<T>;

      bool operator == (const Iterator& other) const
      {
         return other.mImpl == mImpl;
      }

      bool operator != (const Iterator& other) const
      {
         return !operator==(other);
      }

   protected:
      Iterator(std::vector<T>::iterator& impl)
      : mImpl(impl)
      {
         
      }

      std::vector<T>::iterator mImpl;
   };

   RemoteCollection(const std::string& name, Container& container)
   : mName(name)
   , mContainer(container)
   {
      auto items = mContainer.getItems(mName);

      mLocalCache.reserve(items.size());

      for(auto x : items)
      {
         auto item = Serializer::deserialize(x.content);
         mLocalToRemoteIdMap.insert(std::make_pair(item.id, x.id));
         mLocalCache.push_back(item);
         result.push_back(item);
      }

      return result;
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

   template<TIterator>
   void insert(const TIterator& begin, const TIterator& end)
   {
      std::vector<ContainerItem> items(std::distance(begin, end));
      std::transform(begin, end, items, [](auto x)->auto {return { materia::Id::Invalid, Serializer::serialize(x) }; });

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
      mContainer.deleteContainer(mName);
      mContainer.createContainer(mName);

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

private:

   std::vector<T> mLocalCache;
   std::map<Id, Id> mLocalToRemoteIdMap;
   std::string mName;
   Container& mContainer;
};

}