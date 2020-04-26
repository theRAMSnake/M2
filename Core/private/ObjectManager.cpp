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

    Id create(const Id id, const std::vector<std::string>& traits, const Params& params) override
    {
        Id newId = Id::generate();

        auto value = params;
        value.put("id", newId);

        Params subParams;
        for(auto& t: traits)
        {
            Params p;
            p.put("", t);
            subParams.push_back({"", p});
        }

        value.add_child("traits", subParams);

        mTable->store(newId, writeJson(value));

        return newId;
    }

    std::vector<Params> query(const std::vector<Id>& ids) override
    {
        std::vector<Params> result;

        for(auto id : ids)
        {
            auto o = get(id);
            if(o)
            {
                result.push_back(*o);
            }
        }

        return result;
    }

    std::vector<Params> query(const Filter& f) override
    {
        std::vector<Params> result;

        mTable->foreach([&](std::string id, std::string json) 
        {
            auto t = readJson<boost::property_tree::ptree>(json);
            if(std::get<bool>(f.evaluate(t)))
            {
                result.push_back(t);
            }
        });

        return result;
    }

    std::optional<Params> get(const Id& id) override
    {
        auto obj = mTable->load(id);

        if(obj)
        {
            return readJson<Params>(*obj);
        }
        else
        {
            return std::optional<Params>();
        }
    }

    bool destroy(const Id id) override
    {
        if(get(id))
        {
            mTable->erase(id);
            return true;
        }
        return false;
    }

    void modify(const Id id, const Params& params) override
    {
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

class TraitTypeHandler : public ITypeHandler
{
public:
    TraitTypeHandler(TraitSystem& ts)
    : mTs(ts)
    {

    }

    Id create(const Id id, const std::vector<std::string>& traits, const Params& params) override
    {
        auto trait = getOrThrow<std::string>(params, "name", "Trait name is not specified");
        return mTs.add({trait, false});
    }

    std::vector<Params> query(const std::vector<Id>& ids) override
    {
        std::vector<Params> result;

        for(auto & id : ids)
        {
            auto o = mTs.get(id);

            if(o)
            {
                result.push_back(toPropertyTree(*o));
            }
        }

        return result;
    }

    std::vector<Params> query(const Filter& f) override
    {
        std::vector<Params> result;

        for(auto & t : mTs.get())
        {
            auto pt = toPropertyTree(t);
            if(std::get<bool>(f.evaluate(pt)))
            {
                result.push_back(pt);
            }
        }

        return result;
    }

    bool destroy(const Id id) override
    {
        return mTs.remove(id);
    }

    void modify(const Id id, const Params& params) override
    {
        throw std::logic_error("Types cannot be modified");
    }

    std::optional<Params> get(const Id& id) override
    {
        auto obj = mTs.get(id);
        if(obj)
        {
            return toPropertyTree(*obj);
        }
        else
        {
            return {};
        }
    }

private:
    TraitSystem& mTs;
};

//-------------------------------------------------------------------------------------------------

ObjectManager::ObjectManager(Database& db, TraitSystem& types)
: mDb(db)
, mTypes(types)
{
    mHandlers["trait"] = std::make_shared<TraitTypeHandler>(mTypes);

    mDefaultHandler = std::make_shared<GenericTypeHandler>(mDb.getTable("objects"));
    mHandlers[""] = mDefaultHandler;
}

Id ObjectManager::create(const std::vector<std::string>& traits, const Id id, const Params& params)
{
    if(traits.empty())
    {
        throw std::runtime_error("Cannot create object without traits");
    }

    auto& handler = getHandler(traits[0]);

    if(id != Id::Invalid && lookup(id))
    {
        throw std::runtime_error(fmt::format("Object with id {} already exist", id.getGuid()));
    }

    return handler.create((id == Id::Invalid ? Id::generate() : id), traits, params);
}

std::vector<Params> ObjectManager::query(const std::vector<Id>& ids)
{
    std::vector<Params> objects;

    for(auto& h : mHandlers)
    {
        auto newObjects = h.second->query(ids);
        objects.insert(objects.end(), newObjects.begin(), newObjects.end());
    }

    return objects;
}

std::vector<Params> ObjectManager::query(const Filter& filter)
{
    std::vector<Params> objects;

    for(auto& h : mHandlers)
    {
        auto newObjects = h.second->query(filter);
        objects.insert(objects.end(), newObjects.begin(), newObjects.end());
    }

    return objects;
}

void ObjectManager::destroy(const Id id)
{
    std::vector<Params> objects;

    for(auto& h : mHandlers)
    {
        if(h.second->destroy(id))
        {
            break;
        }
    }
}

void ObjectManager::modify(const Id id, const Params& params)
{
    auto o = lookup(id);
    if(o)
    {
        auto [handler, object] = *o;
        handler.modify(id, params);
    }
}

ITypeHandler& ObjectManager::getHandler(const std::string& traitName)
{
    if(mHandlers.find(traitName) != mHandlers.end())
    {
        return *mHandlers[traitName];
    }

    return *mDefaultHandler;
}

std::optional<std::tuple<ITypeHandler&, Params>> ObjectManager::lookup(const Id id)
{
    for(auto& h : mHandlers)
    {
        auto o = h.second->get(id);
        if(o)
        {
            return std::tuple<ITypeHandler&, Params>(*h.second, *o);
        }
    }

    return std::optional<std::tuple<ITypeHandler&, Params>>();
}

}