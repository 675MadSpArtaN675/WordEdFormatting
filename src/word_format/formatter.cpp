#include <fstream>

#include "formatter.hpp"

#include <boost/regex/v5/match_flags.hpp>
#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_iterator.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lambda2.hpp>
#include <boost/system.hpp>

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
    init_formatter();
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

void Formatter::saveTo(std::string filename)
{
    LOG("Saving to file: " << filename);

    if (boost::filesystem::exists(filename))
    {
        boost::filesystem::remove(boost::filesystem::path(filename));
    }

    boost::system::error_code _error;
    if (boost::filesystem::copy_file(_name, filename, _error)) {
        duckx::Document _doc(filename);
        _doc.open();

        if (_doc.is_open()) {
            LOG("Start pattern file parsing...");

            unsigned int _index_of_run = 1;
            duckx::Paragraph& selected_paragraph = _doc.paragraphs();
            while (selected_paragraph.has_next())
            {
                duckx::Run& selected_run = selected_paragraph.runs();

                while (selected_run.has_next())
                {
                    std::string text_of_run = selected_run.get_text();
                    LOG("Selected run: '" << text_of_run << "' ");

                    getting_places(selected_run, text_of_run, _index_of_run);

                    LOG("Final text result: " << text_of_run);
                    selected_run.set_text(text_of_run);

                    selected_run = selected_run.next();
                }

                selected_paragraph = selected_paragraph.next();
            }

            _doc.save();
            LOG("Document is saved!");
        }
    }

    if (_error != boost::system::errc::success) {
        LOG("Error of parsing: " << _error);
    }
}

void Formatter::getting_places(duckx::Run& selected_run, std::string& text_of_run, unsigned int& _index_of_run)
{
    boost::regex_iterator<std::string::const_iterator> run_text_iterator = boost::make_regex_iterator(text_of_run, value_place_pattern, boost::regex_constants::match_partial);

    for (; run_text_iterator != boost::regex_iterator<std::string::const_iterator>(); run_text_iterator++)
    {
        boost::smatch _match = *run_text_iterator;

        if (_match[0].matched)
        {
            std::string found_match_str(_match[0].first, _match[0].second);
            LOG("Match found: " << found_match_str);

            if (_args[_index_of_run] && !_args[_index_of_run]->empty())
            {
                LOG("Value: '" << *_args[_index_of_run] << "'");
                boost::algorithm::replace_first(text_of_run, found_match_str, *_args[_index_of_run]);
            }

            _index_of_run++;
        }
        else if (!_match[0].matched && (_match[0].second - _match[0].first) > 0)
        {
            saving_partial_place(selected_run, text_of_run, _match, _index_of_run);
            _index_of_run++;
        }
    }
}

void Formatter::saving_partial_place(duckx::Run& selected_run, std::string& text_of_run, boost::smatch& _match, const unsigned int& _index_of_run)
{
    if (_args[_index_of_run] && !_args[_index_of_run]->empty())
    {
        LOG("Value: '" << *_args[_index_of_run] << "'");
        auto start_it = _match[0].first;
        std::string semi_text(start_it, text_of_run.cend());

        boost::algorithm::replace_first(text_of_run, semi_text, *_args[_index_of_run]);

        LOG("Start semi text: " << semi_text);
        while (selected_run.has_next())
        {
            selected_run = selected_run.next();
            std::string next_text_of_run = selected_run.get_text();
            LOG("Next run text: '" << next_text_of_run << "'");

            boost::smatch next_match;
            if (boost::regex_match(next_text_of_run, next_match, value_place_pattern, boost::regex_constants::match_partial)
                && !next_match[0].matched && (next_match[0].second - next_match[0].first) > 0)
            {
                next_text_of_run.erase(next_match[0].first, next_match[0].second);
            }

            LOG("New run text: '" << next_text_of_run << "'");
            selected_run.set_text(next_text_of_run);
        }
    }
}

void Formatter::init_formatter()
{
    clearArgs();

    try {
        _document->open();

        duckx::Paragraph& now_paragraph = _document->paragraphs();
        while (now_paragraph.has_next()) {
            std::vector<duckx::Run> _runs;

            duckx::Run& _now_run = now_paragraph.runs();
            while (_now_run.has_next())
            {
                _runs.push_back(_now_run);

                _now_run = _now_run.next();
            }

            int i = 0;
            while (i < _runs.size()) {
                std::string _text = _runs[i].get_text();
                LOG("Paragraph text: " << _text);

                if (!_text.empty()) {
                    get_places_for_values(_runs, _text, value_place_pattern, i);
                }

                i++;
            }

            now_paragraph = now_paragraph.next();
        }

        _document->save();
    }
    catch (const std::exception& error)
    {
        LOG("Error: " << error.what());
    }
}

void Formatter::get_places_for_values(const std::vector<duckx::Run>& _runs, std::string _text, boost::regex& value_place_pattern, int& run_counter)
{
    boost::regex_iterator<std::string::const_iterator> _iter = boost::make_regex_iterator(_text, value_place_pattern, boost::regex_constants::match_partial);

    for (; _iter != boost::regex_iterator<std::string::const_iterator>(); _iter++)
    {
        boost::smatch _match = *_iter;

        if (_match[0].matched && _match[0].first != _match[0].second)
        {
            LOG("Full scan: " << _match);
            add_arg(_match);
        }
        else if (!_match[0].matched && _match[0].first != _match[0].second && (_match[0].second - _match[0].first) > 0)
        {
            auto bracket_start = std::find_if(_text.begin(), _text.end(), boost::lambda2::_1 == '[' || boost::lambda2::_1 == '{');
            std::string full_partial_arg(bracket_start, _text.end());

            LOG("Partial scan: " << full_partial_arg);
            join_partial_text(_runs, full_partial_arg, value_place_pattern, run_counter);
        }
    }
}

void Formatter::add_arg(const boost::smatch& _match)
{
    std::string _name = _match["name"];
    std::string _options = _match["parameters"];
    LOG(boost::format("Name: '%s'. Options: '%s'") % _name % _options);

    if (_name.empty())
    {
        bindArg<std::string>(std::string());
    }
    else {
        bindArg<std::string>(_name, std::string());
    }
}

void Formatter::join_partial_text(const std::vector<duckx::Run>& _runs, std::string& full_partial_arg, boost::regex& value_place_pattern, int& run_counter)
{
    int start_index = run_counter + 1;
    while (start_index < _runs.size())
    {
        std::string _new_text = _runs[start_index].get_text();
        LOG("Now text: " << _new_text);
        boost::smatch next_match_partial;

        if (!boost::regex_match(_new_text, next_match_partial, value_place_pattern, boost::regex_constants::match_partial) && (next_match_partial[0].second - next_match_partial[0].first) > 0)
        {
            auto finded_element = std::find_if(_new_text.begin(), _new_text.end(), boost::lambda2::_1 == ']' || boost::lambda2::_1 == '}');

            if (finded_element != _new_text.end())
            {
                full_partial_arg += std::string(_new_text.begin(), ++finded_element);
                break;
            }
            else {
                full_partial_arg += _new_text;
            }
        }

        start_index++;
    }

    run_counter = start_index;

    LOG("Result arg: " << full_partial_arg);

    boost::smatch _line_match;
    if (boost::regex_match(full_partial_arg, _line_match, value_place_pattern))
    {
        add_arg(_line_match);
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