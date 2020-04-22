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
        mTable->store(newId, writeJson(value));

        return newId;
    }

    std::vector<Params> queryAll() override
    {
        std::vector<Params> result;

        mTable->foreach([&](std::string id, std::string json) 
        {
            result.push_back(readJson<boost::property_tree::ptree>(json));
        });

        return result;
    }

    std::vector<Params> query(const Filter& f) override
    {
        std::vector<Params> result;

        mTable->foreach([&](std::string id, std::string json) 
        {
            auto t = readJson<boost::property_tree::ptree>(json);
            //if(resolveExpression<bool>(f.evaluate(t)))
            {
                result.push_back(t);
            }
        });

        return result;
    }

    Params query(const Id& id)
    {
        auto obj = mTable->load(id);

        if(obj)
        {
            return readJson<Params>(*obj);
        }
        else
        {
            throw std::runtime_error(fmt::format("Object not found: {}", id.getGuid()));
        }
    }

    void destroy(const Id id) override
    {
        mTable->erase(id);
    }

    void modify(const Params& params) override
    {
        auto id = getOrThrow<Id>(params, "id", "Id is not specified");

        auto obj = mTable->load(id);

        if(obj)
        {
            mTable->store(id, writeJson(params));
        }
        else
        {
            throw std::runtime_error(fmt::format("Object not found: {}", id.getGuid()));
        }
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

    std::vector<Params> queryAll() override
    {
        std::vector<Params> result;

        for(auto & t : mTs.get())
        {
            result.push_back(toPropertyTree(t));
        }

        return result;
    }

    std::vector<Params> query(const Filter& f) override
    {
        std::vector<Params> result;

        for(auto & t : mTs.get())
        {
            auto pt = toPropertyTree(t);
            //if(resolveExpression<bool>(f.evaluate(t)))
            {
                result.push_back(pt);
            }
        }

        return result;
    }

    void destroy(const Id id) override
    {
        mTs.remove(id);
    }

    void modify(const Params& params) override
    {
        throw std::logic_error("Types cannot be modified");
    }

    Params query(const Id& id)
    {
        auto obj = mTs.get(id);
        if(obj)
        {
            return toPropertyTree(*obj);
        }
        else
        {
            throw std::runtime_error(fmt::format("Object not found: {}", id.getGuid()));
        }
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

std::vector<Params> ObjectManager::query(const TypeDef& type)
{
    auto& handler = *getOrThrow(mHandlers, {type.domain, type.name}, fmt::format("Type cannot be handled: {}.{}", type.domain, type.name));

    return handler.queryAll();
}

std::vector<Params> ObjectManager::query(const TypeDef& type, const Filter& filter)
{
    auto& handler = *getOrThrow(mHandlers, {type.domain, type.name}, fmt::format("Type cannot be handled: {}.{}", type.domain, type.name));

    return handler.query(filter);
}

void ObjectManager::destroy(const TypeDef& type, const Id id)
{
    auto& handler = *getOrThrow(mHandlers, {type.domain, type.name}, fmt::format("Type cannot be handled: {}.{}", type.domain, type.name));

    return handler.destroy(id);
}

Params ObjectManager::query(const TypeDef& type, const Id id)
{
    auto& handler = *getOrThrow(mHandlers, {type.domain, type.name}, fmt::format("Type cannot be handled: {}.{}", type.domain, type.name));

    return handler.query(id);
}

void ObjectManager::modify(const TypeDef& type, const Params& params)
{
    auto& handler = *getOrThrow(mHandlers, {type.domain, type.name}, fmt::format("Type cannot be handled: {}.{}", type.domain, type.name));

    return handler.modify(params);
}

}