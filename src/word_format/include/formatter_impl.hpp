#pragma once

#include <concepts>
#include "exporter.hpp"

template<StringConvertable T>
void Formatter::bindArg(int arg_num, T _arg_value)
{
    if (arg_num < 1)
    {
        arg_num = get_free_index();
    }

    std::string value;
    if constexpr (std::same_as<T, std::string>)
    {
        value = _arg_value;
    }
    else {
        value = std::to_string(_arg_value);
    }

    if (_args.contains(arg_num)) {
        _args[arg_num].reset(new std::string(value));
    }
    else {
        _args[arg_num] = std::make_shared<std::string>(value);
    }
}

template<StringConvertable T>
void Formatter::bindArg(std::string arg_name, T _arg_value)
{
    if (_aliases.left.find(arg_name) != end(_aliases.left))
    {
        unsigned int _index = _aliases.left.at(arg_name);
    
        bindArg(_index, _arg_value);
    }
}

template<StringConvertable T>
void Formatter::bindArg(T _arg_value)
{
    bindArg(0, _arg_value);
}   

template<StringConvertable T>
Formatter Formatter::operator%(T value)
{
    Formatter formatter(*this);
    formatter.bindArg(value);

    return formatter;
}

template<StringConvertable T>
Formatter& Formatter::operator%=(T value)
{
    bindArg(value);

    return *this; 
}