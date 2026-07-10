#include "formatter.hpp"

Formatter::Formatter() : Formatter("")
{ }

Formatter::Formatter(std::string file_pattern_name)
{
    setFilePattern(file_pattern_name);
}

Formatter::Formatter(const Formatter& other) {
    _aliases = other._aliases;
    _args = other._args;

    setFilePattern(other._name);
}
Formatter::Formatter(Formatter&& other) {
    Formatter _other_ready(other);

    std::swap(*this, _other_ready);
}

void Formatter::setFilePattern(std::string file_pattern_name)
{
    if (file_pattern_name.empty())
    {
        _document = nullptr;
        return;
    }

    _name = file_pattern_name;
    _document = std::make_unique<duckx::Document>(duckx::Document(file_pattern_name));
}

template<StringConvertable T>
void Formatter::bindArg(T _arg_value)
{
    bindArg(0, _arg_value);
}

template<StringConvertable T>
void Formatter::bindArg(int arg_num, T _arg_value)
{
    if (arg_num < 1)
    {
        arg_num = get_free_index();
    }

    if (_args.contains(arg_num)) {
        _args[arg_num].reset(_arg_value.str());
    }
    else {
        _args[arg_num] = std::make_shared<T>(_arg_value.str());
    }
}

template<StringConvertable T>
void Formatter::bindArg(std::string arg_name, T _arg_value)
{
    if (_aliases.left.find(arg_name) != end(_aliases.left))
    {
        unsigned int _index = _aliases.left[arg_name];
    
        bindArg(_index, _arg_value);
    }
}

void Formatter::removeArg(int arg_num)
{
    if (_args.contains(arg_num)) {
        _args.erase(arg_num);
    }
}

void Formatter::removeArg(std::string arg_name)
{
    auto index = _aliases.left.find(arg_name);
    if (index != end(_aliases.left)) {
        unsigned int arg_num = index->second;

        _aliases.left.erase(arg_name);
        removeArg(arg_num);
    }
}

bool Formatter::isArgEmpty(int arg_num)
{
    if (!_args.contains(arg_num) || _args.empty())
    {
        return true;
    }

    return !_args[arg_num] || _args[arg_num]->empty();
}

bool Formatter::isArgEmpty(std::string arg_name)
{
    if (_aliases.left.find(arg_name) != end(_aliases.left)) {
        unsigned int _index = _aliases.left.at(arg_name);

        return isArgEmpty(_index);
    }

    return true;
}

int Formatter::getArgNum(std::string arg_name)
{
    if (!isArgEmpty(arg_name))
    {
        return _aliases.left.find(arg_name)->get_right();
    }

    return 0;
}

std::string Formatter::getArgName(unsigned int arg_num)
{
    return _aliases.right.at(arg_num);
}

std::string Formatter::getArgValue(unsigned int arg_num)
{
    return *_args[arg_num];
}

std::string Formatter::getArgValue(std::string name)
{
    unsigned int index = _aliases.left.find(name)->second;

    return *_args[index];
}

unsigned int Formatter::count()
{
    return _args.size();
}

void Formatter::clearArgs()
{
    _aliases.clear();
    _args.clear();
}

std::string Formatter::toStr()
{
    return std::string();
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

void Formatter::init_formatter()
{
    boost::regex value_place_pattern("\\{(?<name>[\\w_]*)(?<parameters>\\:[\\w#_-\\d]*)?\\}");

    try {
        _document->open();
        
        duckx::Paragraph& now_paragraph = _document->paragraphs();
        while (now_paragraph.has_next()) {
            duckx::Run& now_run = now_paragraph.runs();

            while (now_run.has_next()) {
                std::string _text = now_run.get_text();

                now_run = now_run.next();
            }

            now_paragraph = now_paragraph.next();
        }

        _document->save();
    }
    catch (const std::exception& error)
    {
        
    }
}

unsigned int Formatter::get_free_index()
{
    for (int i = 1; i < _args.size() + 1; i++)
    {
        if (!_args.contains(i))
        {
            return i;
        }
    }

    return _args.size() + 1;
}