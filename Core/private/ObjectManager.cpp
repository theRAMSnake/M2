#include "ObjectManager.hpp"
#include "JsonSerializer.hpp"
#include "ExceptionsUtil.hpp"

namespace materia
{

class GenericTypeHandler : public ITypeHandler
{
public:
    GenericTypeHandler(std::unique_ptr<DatabaseTable>&& table)
    : mTable(std::move(table))
    {

    }

    Id create(const Params& params) override
    {
        Id newId = Id::generate();

        auto value = params;
        value.put("id", newId);
        mTable->store(newId, writeJson(params));

        return newId;
    }

private:
    std::unique_ptr<DatabaseTable> mTable;
};

class TypeTypeHandler : public ITypeHandler
{
public:
    TypeTypeHandler(TypeSystem& ts)
    : mTs(ts)
    {

    }

    Id create(const Params& params) override
    {
        auto tpDomain = getOrThrow<std::string>(params, "type.domain", "Type domain is not specified");
        auto tpName = getOrThrow<std::string>(params, "type.name", "Type name is not specified");
        return mTs.add({tpDomain, tpName, false});
    }

private:
    TypeSystem& mTs;
};

//-------------------------------------------------------------------------------------------------
std::string typeToTableName(const TypeDef& td)
{
    return td.domain + "_" + td.name;
}

ObjectManager::ObjectManager(Database& db, TypeSystem& types)
: mDb(db)
, mTypes(types)
{
    mTypes.onAdded.connect(std::bind(&ObjectManager::onNewTypeAdded, this, std::placeholders::_1));

    for(auto t : mTypes.get())
    {
        if(t.domain != "core")
        {
            mHandlers[{t.domain, t.name}] = std::make_shared<GenericTypeHandler>(mDb.getTable(typeToTableName(t)));
        }
    }

    mHandlers[{"core", "type"}] = std::make_shared<TypeTypeHandler>(mTypes);
}

void ObjectManager::onNewTypeAdded(const TypeDef type)
{
    mHandlers[{type.domain, type.name}] = std::make_shared<GenericTypeHandler>(mDb.getTable(typeToTableName(type)));
}

Id ObjectManager::create(const TypeDef& type, const Params& params)
{
    auto& handler = *getOrThrow(mHandlers, {type.domain, type.name}, fmt::format("Type cannot be handled: {}.{}", type.domain, type.name));

    return handler.create(params);
}

}