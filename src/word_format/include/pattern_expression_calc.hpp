#pragma once

#include <map>

#include <boost/spirit/home/qi.hpp>
#include <boost/algorithm/string.hpp>

#include "exporter.hpp"

namespace qi = boost::spirit::qi;

struct AriphmeticAction {
    int first_num;
    int second_num;
    char action;
};

template<typename Iterator>
struct AriphmeticGrammar : public qi::grammar<Iterator, AriphmeticAction> {
    AriphmeticGrammar(char ariphm_action) : AriphmeticGrammar::base_type(_start)
    {
        _ariphm_action = ariphm_action;

        _start %= qi::int_ << lit(ariphm_action) << qi::int_;
    }

    qi::rule<Iterator, AriphmeticAction> _start;

private:
    char _ariphm_action;
};

template<typename Iterator>
struct AriphmeticInBracketsGrammar : public qi::grammar<Iterator, std::string> {
    AriphmeticGrammar(char ariphm_action) : AriphmeticGrammar::base_type(_start)
    {
        _ariphm_action = ariphm_action;

        _start %= '(' << qi::string << ')';
    }

    qi::rule<Iterator, AriphmeticAction> _start;

private:
    char _ariphm_action;
};

class ExpressionCalc
{
public:
    ExpressionCalc();
    ExpressionCalc(const ExpressionCalc& _other);
    ExpressionCalc(ExpressionCalc&& _other);

    void parse_expression(std::string _expression);
    void calculate();

    ExpressionCalc& operator=(const ExpressionCalc& _other) = default;
    ExpressionCalc& operator=(ExpressionCalc&& _other) = default;
};