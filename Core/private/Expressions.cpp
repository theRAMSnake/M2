#include "Expressions.hpp"
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <fmt/format.h>
#include <iostream>

namespace materia
{

enum class TokenType
{
    Identifier,
    String,
    Int,
    Double,
    Bool,
    Is,
    Operator_Less,
    Operator_Greater,
    Operator_Equals,
    Operator_Contains,
    Operator_And,
    Operator_Or
};

struct Token
{
    TokenType type;
    std::string symbol;
};

bool isInteger(const std::string& src)
{
    try
    {
        boost::lexical_cast<int>(src);
        return true;
    }
    catch(boost::bad_lexical_cast& e)
    {
        return false;
    }
}

Token parseToken(const std::string& src)
{
    if(src == ">")
    {
        return {TokenType::Operator_Greater};
    }
    else if(src == "<")
    {
        return {TokenType::Operator_Less};
    }
    else if(src == "=")
    {
        return {TokenType::Operator_Equals};
    }
    else if(src == "contains")
    {
        return {TokenType::Operator_Contains};
    }
    else if(src == "AND")
    {
        return {TokenType::Operator_And};
    }
    else if(src == "OR")
    {
        return {TokenType::Operator_Or};
    }
    else if(src == "true" || src == "false")
    {
        return {TokenType::Bool, src};
    }
    else if(src.front() == '\"' && src.back() == '\"')
    {
        return {TokenType::String, src};
    }
    else if(src.front() == '.')
    {
        return {TokenType::Identifier, src};
    }
    else if(src.find('.') != std::string::npos)
    {
        return {TokenType::Double, src};
    }
    else if(isInteger(src))
    {
        return {TokenType::Int, src};
    }
    else if(src.size() > 3 && src[0] == 'I' && src[1] == 'S' && src[2] == '(' && src.find(')') != std::string::npos)
    {
        return {TokenType::Is, src.substr(3, src.find(')') - 3)};
    }
    else
    {
        throw std::runtime_error(fmt::format("Unrecognisable token {}", src));
    }
}

std::vector<Token> tokenize(const std::string& src)
{
    std::vector<std::string> rawTokens;
    boost::split(rawTokens, src, boost::is_any_of("\t "));

    std::vector<Token> result;
    for(auto & r : rawTokens)
    {
        result.push_back(parseToken(r));
    }

    return result;
}

bool isValue(const Token& t)
{
    return t.type == TokenType::Identifier ||
        t.type == TokenType::String ||
        t.type == TokenType::Int ||
        t.type == TokenType::Double ||
        t.type == TokenType::Bool ||
        t.type == TokenType::Is;
}

bool isComparison(const Token& t)
{
    return t.type == TokenType::Operator_Equals ||
        t.type == TokenType::Operator_Contains ||
        t.type == TokenType::Operator_Greater ||
        t.type == TokenType::Operator_Less;
}

bool isLogical(const Token& t)
{
    return t.type == TokenType::Operator_And ||
        t.type == TokenType::Operator_Or;
}

class IdentifierExpression : public Expression
{
public:
    IdentifierExpression(const Token t)
    : mIdentifier(t.symbol.substr(1))
    {

    }

    Value evaluate(const Object& object) const
    {
        try
        {
            auto t = object[mIdentifier].getType();

            switch(t)
            {
                case Type::Int: return object[mIdentifier].get<Type::Int>();
                case Type::Money: return object[mIdentifier].get<Type::Money>();
                case Type::Timestamp: return object[mIdentifier].get<Type::Timestamp>().value;
                case Type::Option: return object[mIdentifier].get<Type::Option>();
                case Type::Double: return object[mIdentifier].get<Type::Double>();
                case Type::Bool: return object[mIdentifier].get<Type::Bool>();
                case Type::String: return object[mIdentifier].get<Type::String>();
                case Type::Reference: return object[mIdentifier].get<Type::Reference>();
                case Type::StringArray: return Value("");
            }

            throw std::runtime_error("Unknown type");
        }
        catch(...)
        {
            return false;
        }
    }

private:
    std::string mIdentifier;
};

class IsExpression : public Expression
{
public:
    IsExpression(const Token t)
    : mTypeName(t.symbol)
    {

    }

    Value evaluate(const Object& object) const
    {
        return object.getType().name == mTypeName;
    }

private:
    std::string mTypeName;
};

class StringExpression : public Expression
{
public:
    StringExpression(const Token t)
    : mStr(t.symbol.substr(1, t.symbol.size() - 2))
    {

    }

    Value evaluate(const Object& object) const
    {
        return mStr;
    }

private:
    std::string mStr;
};

class IntExpression : public Expression
{
public:
    IntExpression(const Token t)
    : mVal(boost::lexical_cast<int>(t.symbol))
    {

    }

    Value evaluate(const Object& object) const
    {
        return mVal;
    }

private:
    std::int64_t mVal;
};

class DoubleExpression : public Expression
{
public:
    DoubleExpression(const Token t)
    : mVal(boost::lexical_cast<double>(t.symbol))
    {

    }

    Value evaluate(const Object& object) const
    {
        return mVal;
    }

private:
    double mVal;
};

class BoolExpression : public Expression
{
public:
    BoolExpression(const Token t)
    : mVal(t.symbol == "true")
    {

    }

    Value evaluate(const Object& object) const
    {
        return mVal;
    }

private:
    bool mVal;
};

std::shared_ptr<Expression> createValueExpression(const Token t)
{
    switch(t.type)
    {
        case TokenType::Identifier:
            return std::make_shared<IdentifierExpression>(t);

        case TokenType::String:
            return std::make_shared<StringExpression>(t);

        case TokenType::Int:
            return std::make_shared<IntExpression>(t);

        case TokenType::Double:
            return std::make_shared<DoubleExpression>(t);

        case TokenType::Bool:
            return std::make_shared<BoolExpression>(t);

        case TokenType::Is:
            return std::make_shared<IsExpression>(t);

        default:
            throw std::runtime_error(fmt::format("Expression of type {} is not supported", t.type));
    }
}

class BinaryExpression : public Expression
{
public:
    BinaryExpression(std::shared_ptr<Expression> arg1, std::shared_ptr<Expression> arg2, std::function<Value(Value, Value)> vFunc)
    : mArg1(arg1)
    , mArg2(arg2)
    , mVfunc(vFunc)
    {

    }

    Value evaluate(const Object& object) const
    {
        auto a1 = mArg1->evaluate(object);
        auto a2 = mArg2->evaluate(object);

        return mVfunc(a1, a2);
    }

private:
    std::shared_ptr<Expression> mArg1;
    std::shared_ptr<Expression> mArg2;
    std::function<Value(Value, Value)> mVfunc;
};

std::shared_ptr<Expression> createBinaryExpression(std::shared_ptr<Expression> arg1, std::shared_ptr<Expression> arg2, const TokenType t)
{
    switch(t)
    {
        case TokenType::Operator_Less:
            return std::make_shared<BinaryExpression>(arg1, arg2, [](auto a1, auto a2) { return a1 < a2;} );

        case TokenType::Operator_Greater:
            return std::make_shared<BinaryExpression>(arg1, arg2, [](auto a1, auto a2) { return a1 > a2;});

        case TokenType::Operator_Equals:
            return std::make_shared<BinaryExpression>(arg1, arg2, [](auto a1, auto a2) { return a1 == a2;});

        case TokenType::Operator_Contains:
            return std::make_shared<BinaryExpression>(arg1, arg2, [](auto a1, auto a2) { return std::get<std::string>(a1).find(std::get<std::string>(a2)) != std::string::npos;});

        case TokenType::Operator_And:
            return std::make_shared<BinaryExpression>(arg1, arg2, [](auto a1, auto a2) { return std::get<bool>(a1) && std::get<bool>(a2);});

        case TokenType::Operator_Or:
            return std::make_shared<BinaryExpression>(arg1, arg2, [](auto a1, auto a2) { return std::get<bool>(a1) || std::get<bool>(a2);});

        default:
            throw std::runtime_error(fmt::format("Binary expression of type {} is not supported", t));
    }
}

std::shared_ptr<Expression> parseExpression(const std::vector<Token> tokens)
{
    //Expression evaluation order: 1.Value, 2.Comparison, 3.Logical
    if(tokens.size() == 1)
    {
        if(!isValue(tokens[0]))
        {
            throw std::runtime_error(fmt::format("Unexpected token: {}", tokens[0].symbol));
        }

        return createValueExpression(tokens[0]);
    }
    else if(tokens.size() == 0)
    {
        return std::shared_ptr<Expression>();
    }
    else
    {
        //Find expr of highest order and split tokens
        auto pos = std::find_if(tokens.begin(), tokens.end(), [](auto x){return isLogical(x);});

        if(pos == tokens.end())
        {
            pos = std::find_if(tokens.begin(), tokens.end(), [](auto x){return isComparison(x);});

            if(pos == tokens.end())
            {
                throw std::runtime_error(fmt::format("Unexpected token: {}", tokens[1].symbol));
            }
        }

        auto t = *pos;
        std::vector<Token> left(tokens.begin(), pos);
        std::vector<Token> right(pos + 1, tokens.end());

        return createBinaryExpression(parseExpression(left), parseExpression(right), t.type);
    }
}

std::shared_ptr<Expression> parseExpression(const std::string& src)
{
    auto tokens = tokenize(src);
    return parseExpression(tokens);
}

}