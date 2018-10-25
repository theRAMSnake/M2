#pragma once
#include <string>
#include <vector>
#include <Common/Id.hpp>

namespace materia
{

struct InboxItem
{
    Id id;
    std::string text;
};

class IInbox
{
public:
    virtual std::vector<InboxItem> get() const = 0;

    virtual void remove(const Id& id) = 0;
    virtual Id add(const InboxItem& item) = 0;
    virtual void replace(const InboxItem& item) = 0;

    virtual ~IInbox(){}
};

}