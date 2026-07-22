#pragma once

#include <concepts>
#include <boost/format.hpp>

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
        LOG(boost::format("Bind arg with number %d his text: '%s'") % arg_num % value);
        _args[arg_num].reset(new std::string(value));
    }
    else {
        LOG(boost::format("Added new arg with number %d his text: '%s'") % arg_num % value);
        _args[arg_num] = std::make_shared<std::string>(value);
    }
}

template<StringConvertable T>
void Formatter::bindArg(std::string arg_name, T _arg_value)
{
    if (_aliases.left.find(arg_name) != _aliases.left.end())
    {
        LOG(boost::format("Adding named argument: '%s'") % arg_name);
        unsigned int _index = _aliases.left.at(arg_name);

        bindArg(_index, _arg_value);
    }
    else {
        unsigned int _index = get_free_index();
        _aliases.insert(aliases_collection_type::value_type(arg_name, _index));

        LOG(boost::format("Adding named argument: '%s'") % arg_name);
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