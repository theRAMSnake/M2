#include "Expressions2.hpp"
#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>
#include <boost/spirit/include/qi_no_skip.hpp>
#include <boost/date_time/gregorian/greg_date.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::spirit;
namespace phx = boost::phoenix;


namespace materia
{
namespace v2
{

template<Type T>
class ConstantExpression : public Expression
{
public:
    ConstantExpression(const typename TypeTraits<T>::Class t)
    : mVal(t)
    {

    }

    Value evaluate(const InterpreterContext& context) const
    {
        return mVal;
    }

private:
    const typename TypeTraits<T>::Class mVal;
};

class FieldAccessExpression : public Expression
{
public:
    FieldAccessExpression( const std::vector<char>& t)
    : mVal(t.data(), t.size())
    {
    }

    Value evaluate(const InterpreterContext& context) const
    {
        const auto& obj = context.getObject();
        auto t = obj[mVal].getType();
        const auto& f = obj[mVal];

        switch(t)
        {
            case Type::Int: return f.get<Type::Int>();
            case Type::Money_v2: return f.get<Type::Money_v2>();
            case Type::Period: return f.get<Type::Period>();
            case Type::Timestamp: return f.get<Type::Timestamp>().value;
            case Type::Choice: return f.get<Type::Choice>();
            case Type::Double: return f.get<Type::Double>();
            case Type::Bool: return f.get<Type::Bool>();
            case Type::String: return f.get<Type::String>();
            case Type::Reference: return f.get<Type::Reference>();
            case Type::StringArray: throw std::runtime_error("Unsupported type StringArray");
        }

        throw std::runtime_error("Unknown type");
    }

private:
    std::string mVal;
};
class StringConstantExpression : public Expression
{
public:
    StringConstantExpression(const std::vector<char>& t)
    : mVal(t.data(), t.size())
    {

    }

    Value evaluate(const InterpreterContext& context) const
    {
        return mVal;
    }

private:
    const std::string mVal;
};
class SymbolConstantExpression : public Expression
{
public:
    SymbolConstantExpression(const std::vector<char>& t)
    : mVal(t.data(), t.size())
    {

    }

    Value evaluate(const InterpreterContext& context) const
    {
        return mVal;
    }

private:
    const std::string mVal;
};
class PeriodConstantExpression : public Expression
{
public:
    PeriodConstantExpression(const int value, const char symbol)
    : mVal((symbol == 'd' ? value : 0),
           ( symbol == 'm' ? value : 0),
           ( symbol == 'y' ? value : 0))
    {

    }

    Value evaluate(const InterpreterContext& context) const
    {
        return mVal;
    }

private:
    const Period mVal;
};
class CurrencyConstantExpression : public Expression
{
public:
    CurrencyConstantExpression(const double value, const std::vector<char> symbol)
    {
        if(symbol.size() != 3)
        {
            throw std::runtime_error("Cannot parse currency: " + std::string(symbol.data(), symbol.size()));
        }
        mVal.currency = std::string(symbol.data(), symbol.size());
        mVal.base = static_cast<int>(value);
        mVal.coins = static_cast<int>((value - static_cast<int>(value)) * 100);
    }

    Value evaluate(const InterpreterContext& context) const
    {
        return mVal;
    }

private:
    Money mVal;
};
template<class Func>
class BinaryExpression : public Expression
{
public:
    BinaryExpression(std::shared_ptr<Expression> arg1, std::shared_ptr<Expression> arg2)
    : mArg1(arg1)
    , mArg2(arg2)
    {
    }

    Value evaluate(const InterpreterContext& ctx) const
    {
        auto a1 = mArg1->evaluate(ctx);
        auto a2 = mArg2->evaluate(ctx);

        Func f;
        return f(a1, a2);
    }

private:
    std::shared_ptr<Expression> mArg1;
    std::shared_ptr<Expression> mArg2;
};

class NotExpression : public Expression
{
public:
    NotExpression(const std::shared_ptr<Expression> val)
    : mVal(val)
    {

    }

    Value evaluate(const InterpreterContext& ctx) const
    {
        auto eval = mVal->evaluate(ctx);
        if(std::holds_alternative<bool>(eval))
        {
            return !std::get<bool>(eval);
        }
        else
        {
            throw std::runtime_error("Unable to negate expression of non boolean type");
        }
    }

private:
    std::shared_ptr<Expression> mVal;
};

bool evaluateConnectionsFunctor(const Id& A, const Id& B, const ConnectionType& type, const Connections& cons)
{
    auto objCons = cons.get(A);
    auto pos = std::find_if(objCons.begin(), objCons.end(), [&](auto x){return x.b == B && x.type == type;});
    return pos != objCons.end();
}

static std::time_t to_time_t(const boost::gregorian::date& date )
{
	using namespace boost::posix_time;
	static ptime epoch(boost::gregorian::date(1970, 1, 1));
	time_duration::sec_type secs = (ptime(date,seconds(0)) - epoch).total_seconds();
	return std::time_t(secs);
}

class FunctionCall : public Expression
{
public:
    FunctionCall(const std::shared_ptr<Expression> name, const std::vector<std::shared_ptr<Expression>> params)
        : mName(name)
        , mParams(params)
    {

    }

    Value evaluate(const InterpreterContext& ctx) const
    {
        auto evaledName = mName->evaluate(ctx);
        if(!std::holds_alternative<std::string>(evaledName))
        {
            throw std::runtime_error("Function call should have a string name");
        }

        auto name = std::get<std::string>(evaledName);
        auto numParamsExpected = name == "RootElement" ? 0 : 1;
        if(numParamsExpected != mParams.size())
        {
            throw std::runtime_error("Invalid function call for " + name + ": expected " + std::to_string(numParamsExpected) + " params, but got " + std::to_string(mParams.size()));
        }

        std::string firstParam;
        if(numParamsExpected == 1)
        {
            auto eval = mParams[0]->evaluate(ctx);
            if(!std::holds_alternative<std::string>(eval))
            {
                throw std::runtime_error("Expected string for function call for " + name);
            }

            firstParam = std::get<std::string>(eval);
        }

        const auto& cons = ctx.getConnections();
        const auto& object = ctx.getObject();

        if(name == "IS")
        {
            return ctx.getObject().getType().name == firstParam;
        }
        else if(name == "DATE")
        {
            boost::gregorian::date d = boost::gregorian::from_string(firstParam);
            return to_time_t(d);
        }
        else if(name == "RootElement")
        {
            auto objCons = cons.get(object.getId());
            for(auto& c : objCons)
            {
                if(c.b == object.getId() && c.type == ConnectionType::Hierarchy)
                {
                    return false;
                }
            }
            return true;
        }
        else if(name == "ParentOf")
        {
            return evaluateConnectionsFunctor(object.getId(), firstParam, ConnectionType::Hierarchy, cons);
        }
        else if(name == "ChildOf")
        {
            return evaluateConnectionsFunctor(firstParam, object.getId(), ConnectionType::Hierarchy, cons);
        }
        else if(name == "Refers")
        {
            return evaluateConnectionsFunctor(object.getId(), firstParam, ConnectionType::Reference, cons);
        }
        else if(name == "ReferedBy")
        {
            return evaluateConnectionsFunctor(firstParam, object.getId(), ConnectionType::Reference, cons);
        }
        else if(name == "ExtendedBy")
        {
            return evaluateConnectionsFunctor(object.getId(), firstParam, ConnectionType::Extension, cons);
        }
        else if(name == "Extends")
        {
            return evaluateConnectionsFunctor(firstParam, object.getId(), ConnectionType::Extension, cons);
        }
        else if(name == "Enables")
        {
            return evaluateConnectionsFunctor(object.getId(), firstParam, ConnectionType::Requirement, cons);
        }
        else if(name == "Requires")
        {
            return evaluateConnectionsFunctor(firstParam, object.getId(), ConnectionType::Requirement, cons);
        }
        else
        {
            throw std::runtime_error("Unknown function: " + name);
        }
    }

private:
    std::shared_ptr<Expression> mName;
    std::vector<std::shared_ptr<Expression>> mParams;
};
struct binaryOr
{
    bool operator()(const Value& a, const Value& b) const
    {
        if(std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b))
        {
            return std::get<bool>(a) || std::get<bool>(b);
        }
        else
        {
            throw std::runtime_error("Unable to logical or for expressions of non boolean type");
        }
    }
};
struct binaryAnd
{
    bool operator()(const Value& a, const Value& b) const
    {
        if(std::holds_alternative<bool>(a) && std::holds_alternative<bool>(b))
        {
            return std::get<bool>(a) &&  std::get<bool>(b);
        }
        else
        {
            throw std::runtime_error("Unable to logical or for expressions of non boolean type");
        }
    }
};
struct contains
{
    bool operator()(const Value& a, const Value& b) const
    {
        if(std::holds_alternative<std::string>(a) && std::holds_alternative<std::string>(b))
        {
            return std::get<std::string>(a).find(std::get<std::string>(b)) != std::string::npos;
        }
        else
        {
            throw std::runtime_error("Unable to apply contains for non string arguments");
        }
    }
};
template<typename ConcreteExpr, typename... T>
std::shared_ptr<Expression> createExpression(T&... a)
{
    return std::make_shared<ConcreteExpr>(std::forward<T>(a)...);
}

class Grammar : public qi::grammar<std::string::const_iterator, std::shared_ptr<Expression>(), ascii::space_type>
{
public:
    Grammar() : Grammar::base_type(anyExpr)
    {
        qi::real_parser<double, qi::strict_real_policies<double>> strict_double;
        constantBool = qi::bool_[qi::_val = phx::bind(&createExpression<ConstantExpression<Type::Bool>, bool>, qi::_1)];
        constantInt = qi::long_long[qi::_val = phx::bind(&createExpression<ConstantExpression<Type::Int>, std::int64_t>, qi::_1)];
        constantDouble = strict_double[qi::_val = phx::bind(&createExpression<ConstantExpression<Type::Double>, double>, qi::_1)];
        constantCurrency = (qi::double_ >> +qi::char_("A-Z"))[qi::_val = phx::bind(&createExpression<CurrencyConstantExpression, double, std::vector<char>>, qi::_1, qi::_2)];
        constantPeriod = (qi::int_ >> qi::char_("mdy"))[qi::_val = phx::bind(&createExpression<PeriodConstantExpression, int, char>, qi::_1, qi::_2)];
        constantStr = ('"' >> +~qi::char_('"') >> '"')[qi::_val = phx::bind(&createExpression<StringConstantExpression, std::vector<char>>, qi::_1)];
        constantSymbol = (+qi::char_("a-zA-Z0-9_-"))[qi::_val = phx::bind(&createExpression<SymbolConstantExpression, std::vector<char>>, qi::_1)];
        fieldExpr = ('.' >> qi::no_skip[+qi::char_("a-zA-Z0-9_-")])[qi::_val = phx::bind(&createExpression<FieldAccessExpression, std::vector<char>>, qi::_1)];
        operatorEq = (primaryExpr >> '=' >> anyExpr)[qi::_val = phx::bind(&createExpression<BinaryExpression<std::equal_to<Value>>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>, qi::_1, qi::_2)];
        operatorGt = (primaryExpr >> '>' >> anyExpr)[qi::_val = phx::bind(&createExpression<BinaryExpression<std::greater<Value>>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>, qi::_1, qi::_2)];
        operatorLe = (primaryExpr >> '<' >> anyExpr)[qi::_val = phx::bind(&createExpression<BinaryExpression<std::less<Value>>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>, qi::_1, qi::_2)];
        operatorOr = (primaryExpr >> "OR" >> anyExpr)[qi::_val = phx::bind(&createExpression<BinaryExpression<binaryOr>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>, qi::_1, qi::_2)];
        operatorAnd = (primaryExpr >> "AND" >> anyExpr)[qi::_val = phx::bind(&createExpression<BinaryExpression<binaryAnd>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>, qi::_1, qi::_2)];
        operatorContains = (primaryExpr >> "contains" >> anyExpr)[qi::_val = phx::bind(&createExpression<BinaryExpression<contains>, std::shared_ptr<Expression>, std::shared_ptr<Expression>>, qi::_1, qi::_2)];
        operatorNot = '!' >> anyExpr[qi::_val = phx::bind(&createExpression<NotExpression, std::shared_ptr<Expression>>, qi::_1)];
        groupExpr = ('(' >> anyExpr >> ')')[qi::_val = qi::_1];
        functionCall = (constantSymbol >> '(' >> *primaryExpr >> ')')[qi::_val = phx::bind(&createExpression<FunctionCall, std::shared_ptr<Expression>, std::vector<std::shared_ptr<Expression>>>, qi::_1, qi::_2)];

        primaryExpr = functionCall | groupExpr | constantStr | constantPeriod | constantBool | constantCurrency | constantDouble | constantInt | fieldExpr | constantSymbol;
        binaryExpr = operatorOr | operatorAnd | operatorEq | operatorGt | operatorLe | operatorContains;
        anyExpr = binaryExpr | operatorNot | primaryExpr;
    }

private:
    using Rule = qi::rule<std::string::const_iterator, std::shared_ptr<Expression>(), ascii::space_type>;
    Rule operatorEq;
    Rule operatorGt;
    Rule operatorLe;
    Rule operatorOr;
    Rule operatorContains;
    Rule operatorAnd;
    Rule operatorNot;
    Rule constantBool;
    Rule constantInt;
    Rule constantStr;
    Rule constantCurrency;
    Rule constantDouble;
    Rule constantPeriod;
    Rule constantSymbol;
    Rule primaryExpr;
    Rule binaryExpr;
    Rule groupExpr;
    Rule fieldExpr;
    Rule functionCall;
    Rule anyExpr;
};

std::shared_ptr<Expression> parseExpression(const std::string& src)
{
    std::shared_ptr<Expression> result;
    Grammar grammar;
    auto begin = src.begin();
    auto end = src.end();
    boost::spirit::qi::phrase_parse(
        begin, end, grammar, ascii::space, result);

    if (begin != end) {
        std::cout << "Unparseable: "
                  << std::quoted(std::string(begin, end)) << std::endl;
        throw std::runtime_error("Parse error: " + src);
    }

    return result;
}

void InterpreterContext::setObject(const Object& o)
{
    mObject = &o;
}

const Object& InterpreterContext::getObject() const
{
    if(!mObject)
    {
        throw std::runtime_error("Object is not attached");
    }

    return *mObject;
}

InterpreterContext::InterpreterContext(const Connections& cons)
    : mConns(cons)
{
}

const Connections& InterpreterContext::getConnections() const
{
    return mConns;
}
}
}
