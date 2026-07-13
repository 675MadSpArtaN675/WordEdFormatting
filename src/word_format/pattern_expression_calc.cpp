#include "pattern_expression_calc.hpp"

ExpressionCalc::ExpressionCalc()
{
}

ExpressionCalc::ExpressionCalc(const ExpressionCalc &_other)
{

}

ExpressionCalc::ExpressionCalc(ExpressionCalc &&_other)
{
}

void ExpressionCalc::parse_expression(std::string _expression)
{
    boost::algorithm::replace_all(_expression, " ", "");

    std::map<std::string, std::string> _brackets;
    std::string _expression_now = _expression;
    while (!_expression_now.empty())
    {
    }
}

void ExpressionCalc::calculate()
{
}
