#pragma once
#include <map>
#include <boost/lexical_cast.hpp>
#include "../Common/Id.hpp"

namespace materia
{

enum class NodeAttributeType
{
   //keep consistency
   IS_DONE = 1,
   BRIEF = 2,
   PROGRESS_TOTAL = 3,
   PROGRESS_CURRENT = 4,
   WATCH_ITEM_REFERENCE = 5,
   REQUIRED_TIMESTAMP = 6,
   GOAL_REFERENCE = 7,
   CONDITION_EXPRESSION = 8,
   CHALLENGE_REFERENCE = 9
};

template<NodeAttributeType ATR_TYPE>
struct NodeAttributeTraits {};

template<>
struct NodeAttributeTraits<NodeAttributeType::IS_DONE> { using Type = bool; };

template<>
struct NodeAttributeTraits<NodeAttributeType::BRIEF> { using Type = std::string; };

template<>
struct NodeAttributeTraits<NodeAttributeType::PROGRESS_TOTAL> { using Type = unsigned int; };

template<>
struct NodeAttributeTraits<NodeAttributeType::PROGRESS_CURRENT> { using Type = unsigned int; };

template<>
struct NodeAttributeTraits<NodeAttributeType::WATCH_ITEM_REFERENCE> { using Type = Id; };

template<>
struct NodeAttributeTraits<NodeAttributeType::REQUIRED_TIMESTAMP> { using Type = std::time_t; };

template<>
struct NodeAttributeTraits<NodeAttributeType::GOAL_REFERENCE> { using Type = Id; };

template<>
struct NodeAttributeTraits<NodeAttributeType::CONDITION_EXPRESSION> { using Type = std::string; };

template<>
struct NodeAttributeTraits<NodeAttributeType::CHALLENGE_REFERENCE> { using Type = Id; };

class NodeAttributes
{
public:
   NodeAttributes();
	explicit NodeAttributes(const std::map<NodeAttributeType, std::string>& init);

	NodeAttributes& operator= (const NodeAttributes& other);

	std::map<NodeAttributeType, std::string> toMap() const;
   bool contains(const NodeAttributeType attrType) const;

   template<NodeAttributeType ATTR_TYPE>
   void set(typename NodeAttributeTraits<ATTR_TYPE>::Type attrVal)
   {
      mStorage[ATTR_TYPE] = boost::lexical_cast<std::string>(attrVal);
   }

   template<NodeAttributeType ATTR_TYPE>
   typename NodeAttributeTraits<ATTR_TYPE>::Type get() const
   {
      return boost::lexical_cast<typename NodeAttributeTraits<ATTR_TYPE>::Type>(mStorage.find(ATTR_TYPE)->second);
   }

private:
   std::map<NodeAttributeType, std::string> mStorage;
};

}

