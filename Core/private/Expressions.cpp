#include "Expressions.hpp"

namespace materia
{

enum class TokenType
{
    Identifier,
    String,
    Int,
    Double,
    Bool,
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
}

//SNAKE:
bool isValue(const Token& t);
std::shared_ptr<Expression> createValueExpression(const Token& t);
std::shared_ptr<Expression> createBinaryExpression(const Token& t);

std::unique_ptr<Expression> parseExpression(const std::string& src)
{
    auto tokens = tokenize(src);

    std::shared_ptr<Expression> currentExp;
    std::optional<TokenType> currentOperator;

    //Maybe state based is better if I want to make it more complex
    for(auto t : tokens)
    {
        if(isValue(t))
        {
            auto exp = createValueExpression(t);

            if(currentOperator)
            {
                currentExp = createBinaryExpression(currentExp, exp, *currentOperator);
                currentOperator.reset();
            }
            else if(currentExp == nullptr)
            {
                currentExp = exp;
            }
            else
            {
                throw std::runtime_error("failed to parse expression");
            }
        }
        else
        {
            if(!currentOperator && currentExp != nullptr)
            {
                currentOperator.reset(t.type);
            }
            else
            {
                throw std::runtime_error("failed to parse expression");
            }
        }
    }

    return currentExp;
}

}