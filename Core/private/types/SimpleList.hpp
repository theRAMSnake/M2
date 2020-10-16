#pragma once
#include "Common/Id.hpp"
#include "../Object.hpp"

namespace materia
{

class ObjectManager;

namespace types
{

class SimpleList
{
public:
    SimpleList(ObjectManager& om, const Id& id);
    ~SimpleList();
    void add(const std::string& item);
    std::string at(const std::size_t pos);
    std::size_t size(); 

private:
    bool mChanged = false;
    Object mObject;
    ObjectManager& mOm;
};

}
}