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

template<class T>
class RemoteCollectionItemTraits
{
public:
   static Id getId(const T& item)
   {
      return item.id;
   }
};

template<class T, class Serializer = ToJsonSerializer<T>>
class RemoteCollection
{
public:
   class Iterator
   {
   public:
      typedef T value_type;
      typedef int difference_type;
      typedef std::random_access_iterator_tag iterator_category;
      typedef T* pointer;
      typedef T& reference;

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

      const T& operator* () const
      {
         return *mImpl;
      }

      const T* operator-> () const
      {
         return mImpl.operator->();
      }

      difference_type operator- (const Iterator& other)
      {
         return  mImpl - other.mImpl;
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
         mLocalToRemoteIdMap.insert(std::make_pair(RemoteCollectionItemTraits<T>::getId(item), x.id));
         mLocalCache.push_back(item);
      }
   }

   void insert(const T& item)
   {
      auto insertedIds = mContainer.insertItems(mName, {{ materia::Id::Invalid, Serializer::serialize(item) }});
      if(insertedIds.size() == 1)
      {
         mLocalToRemoteIdMap.insert(std::make_pair(RemoteCollectionItemTraits<T>::getId(item), insertedIds[0]));
         mLocalCache.push_back(item);
      }
   }

   template<class TIterator>
   void insert(const TIterator& begin, const TIterator& end)
   {
      //it might be broken
      std::vector<ContainerItem> items(std::distance(begin, end));
      std::transform(begin, end, items.begin(), [](auto x)->auto {return ContainerItem{ materia::Id::Invalid, Serializer::serialize(x) }; });

      auto insertedIds = mContainer.insertItems(mName, items);
      if(insertedIds.size() == items.size())
      {
         auto curId = insertedIds.begin();
         auto itemIter = begin;
         while(itemIter != end)
         {
            mLocalToRemoteIdMap.insert(std::make_pair(itemIter->id, *curId));
            curId++;
            itemIter++;
         }

         mLocalCache.insert(mLocalCache.end(), begin, end);
      }
   }

   void update(const T& item)
   {
      mContainer.replaceItems(mName, {{ mLocalToRemoteIdMap[RemoteCollectionItemTraits<T>::getId(item)], Serializer::serialize(item) }});
      *std::find_if(mLocalCache.begin(), mLocalCache.end(), [&](auto x)->bool{return RemoteCollectionItemTraits<T>::getId(item) == 
         RemoteCollectionItemTraits<T>::getId(x);}) = item;
   }

   void erase(const Iterator& pos)
   {
      if(mContainer.deleteItems(mName, {{ mLocalToRemoteIdMap[RemoteCollectionItemTraits<T>::getId(*pos)] }}))
      {
         mLocalToRemoteIdMap.erase(mLocalToRemoteIdMap.find(RemoteCollectionItemTraits<T>::getId(*pos)));
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
      return std::find_if(begin(), end(), [&](auto x)->auto{return RemoteCollectionItemTraits<T>::getId(x) == id;});
   }

private:

   std::vector<T> mLocalCache;
   std::map<Id, Id> mLocalToRemoteIdMap;
   std::string mName;
   IContainer& mContainer;
};

}