#include "FreeData.hpp"
#include "JsonSerializer.hpp"
#include <boost/lexical_cast.hpp>

BIND_JSON2(materia::DataBlock, name, value)

namespace materia
{

namespace freedata
{

FreeData::FreeData(Database& db)
: mStorage(db.getTable("freedata"))
{
    
}

std::vector<DataBlock> FreeData::get()
{
    std::vector<DataBlock> result;

    mStorage->foreach([&](std::string id, std::string json) 
    {
        result.push_back(readJson<materia::DataBlock>(json));
    });

    return result;
}

void FreeData::remove(const std::string& name)
{
    mStorage->erase(name);
}

void FreeData::set(const DataBlock& block)
{
    mStorage->store(block.name, writeJson(block));
}

void FreeData::increment(const std::string& name, const int value)
{
    auto blocks = get();
    auto pos = std::find_if(blocks.begin(), blocks.end(), [&](auto x)->bool {return x.name == name;});
    if(pos != blocks.end())
    {
        pos->value += value;
        set(*pos);
    }
}

bool is_number(const std::string& s)
{
   try 
   {
      boost::lexical_cast<int>(s);
      return true;
   }
   catch(...) 
   { 
      return false;
   }
}

std::string FreeData::bindField(const std::string& field)
{
    if(is_number(field))
    {
        return field;
    }
    else
    {
        auto blocks = get();
        auto pos = std::find_if(blocks.begin(), blocks.end(), [&](auto x)->bool {return x.name == field;});
        if(pos != blocks.end())
        {
            return std::to_string(pos->value);
        }
        else
        {
            throw std::invalid_argument("field");
        }
    }
}

std::optional<BinaryExpression> FreeData::bind(const BinaryExpression& expr)
{   
    try
    {
        BinaryExpression newExpr;

        newExpr.arg1 = bindField(expr.arg1);
        newExpr.arg2 = bindField(expr.arg2);
        newExpr.sign = expr.sign;

        return newExpr;
    }
    catch(const std::exception& e)
    {
        return std::optional<BinaryExpression>();
    }
}

bool FreeData::checkExpression(const std::string& expr)
{
    auto compiled = mInterpreter.compile(expr);
    if(compiled)
    {
        auto bound = bind(*compiled);
        return bound.has_value();
    }

    return false;
}

bool FreeData::evaluateExpression(const std::string& expr)
{
    auto compiled = mInterpreter.compile(expr);
    if(compiled)
    {
        auto bound = bind(*compiled);
        if(bound)
        {
            return mInterpreter.execBoolean(*bound);
        }
    }

    return false;
}

}
}