#include "paints_numberer.hpp"

#include <sstream>
#include <algorithm>
#include <queue>
#include <unordered_set>

#include <boost/lambda2.hpp>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>

#include <exprtk.hpp>


PaintsNumberer::PaintsNumberer() : PaintsNumberer("") { }

PaintsNumberer::PaintsNumberer(std::string filename, unsigned int min_numeration, unsigned int max_numeration) :
    _document(std::make_unique<duckx::Document>(filename)),
    _max_numeration(max_numeration),
    _min_numeration(min_numeration),
    _paint_patterns{ PatternTitle(STANDARD_PAINT_PATTERN, false)},
    _paints_count(0)
{ }


PaintsNumberer::PaintsNumberer(const PaintsNumberer& other) : _paint_patterns(other._paint_patterns) { }

PaintsNumberer::PaintsNumberer(PaintsNumberer&& other) :
    _document(std::move(other._document)),
    _min_numeration(std::move(other._min_numeration)),
    _max_numeration(std::move(other._max_numeration)),
    _paint_patterns(std::move(other._paint_patterns)),
    _paints_count(std::move(other._paints_count))
{
    other._paint_patterns.clear();
}

void PaintsNumberer::setFile(const std::string& file_path)
{
    LOG((boost::format("Setting file with name: '%s'") % file_path).str());

    if (!file_path.empty())
    {
        LOG("Intializing file...");
        _document.reset(new duckx::Document(file_path));

        return;
    }

    throw std::logic_error("Empty name of file...");
}

void PaintsNumberer::setFile(duckx::Document* file_doc)
{
    LOG("Setting docx file...");
    if (file_doc != nullptr)
    {
        LOG("Intializing file...");
        _document.reset(file_doc);

        return;
    }

    throw std::logic_error("Pointer to file object...");
}

duckx::Document* PaintsNumberer::releaseFile()
{
    LOG("Releasing file..." << ((_document) ? "File is ready..." : "File is empty"));

    return (_document) ? _document.release() : nullptr;
}
void PaintsNumberer::addPaintPattern(std::string pattern, bool is_title_clear) {
    LOG((boost::format("Input pattern: %s;\nIs title need clear: %d") % pattern % is_title_clear).str());

    PatternTitle title_(pattern, is_title_clear);
    if (!_paint_patterns.contains(title_)) {
        LOG("Inserting in list..");
        _paint_patterns.insert(title_);

        return;
    }

    throw std::logic_error("Pattern is already has...");
}

void PaintsNumberer::changePaintPattern(unsigned int index, std::string pattern, bool is_title_clear) {
    LOG((boost::format("Changing Paint pattern with: Index: %d; Pattern: %s; Is need to clear: %d") % index % pattern % is_title_clear));

    auto _begin = _paint_patterns.begin();
    std::advance(_begin, index);

    auto _title = _paint_patterns.extract(_begin);
    if (_begin != end(_paint_patterns))
    {
        if (!pattern.empty()) {
            LOG("Changing pattern...");
            _title.value().pattern = pattern;
        }

        LOG("Setting flag 'is need to clear'...");
        _title.value().is_title_clear = is_title_clear;

        _paint_patterns.insert(std::move(_title));
        return;
    }

    throw std::logic_error("Pattern not found...");
}

void PaintsNumberer::removePaintPattern(unsigned int index) {
    LOG((boost::format("Removing pattern with index: %d") % index));

    auto _begin = _paint_patterns.begin();
    std::advance(_begin, index);

    if (_begin != end(_paint_patterns))
    {
        LOG((boost::format("Removing: '%s'") % _begin->pattern).str());
        _paint_patterns.erase(_begin);

        return;
    }

    throw std::logic_error("Pattern not found...");
}

std::string PaintsNumberer::getPattern(unsigned int index) {
    LOG((boost::format("Getting pattern with index: %d") % index).str());
    auto _begin = _paint_patterns.begin();
    std::advance(_begin, index);

    if (_begin != end(_paint_patterns))
    {
        LOG("Pattern: " << _begin->pattern);
        return _begin->pattern;
    }

    throw std::logic_error("Pattern not found...");
}

std::list<std::string> PaintsNumberer::patterns() {
    if (_paint_patterns.size() > 0) {
        auto _pattern_title_to_str = [](const PatternTitle& index) { return index.pattern; };

        std::list<std::string> _pattern_lines;
        std::transform(
                        _paint_patterns.begin(),
                        _paint_patterns.end(),
                        std::back_inserter(_pattern_lines),
                        _pattern_title_to_str
                    );

        std::stringstream _patterns_list;
        _patterns_list << "Founded patterns: " << ((_pattern_lines.size() > 0) ? "" : "None") << "\n\t";
        for (const std::string& _pattern : _pattern_lines)
        {
            _patterns_list << _pattern << ";\n\t";
        }
        LOG(_patterns_list.str());

        return _pattern_lines;
    }

    throw std::runtime_error("No patterns in list");
}

bool PaintsNumberer::empty()
{
    LOG((boost::format("Is paint empty: %d\nIs document has: %d") % _paint_patterns.empty() % _document).str());

    return _paint_patterns.empty() || !_document;
}

void PaintsNumberer::clear()
{
    _paint_patterns.clear();
    LOG("Paint patterns cleared!");
}

void PaintsNumberer::set_min_numeration(unsigned int index)
{
    LOG((boost::format("Set min numeration: %d") % index).str());
    _min_numeration = index;
}

void PaintsNumberer::set_max_numeration(unsigned int index)
{
    LOG((boost::format("Set max numeration: %d") % index).str());
    _max_numeration = index;
}

unsigned int PaintsNumberer::get_min_numeration()
{
    return _min_numeration;
}

unsigned int PaintsNumberer::get_max_numeration()
{
    return _max_numeration;
}

unsigned int PaintsNumberer::paints_count()
{
    return _paints_count;
}

unsigned long PaintsNumberer::init_paint_num()
{
    return (_min_numeration > 0) ? _min_numeration : 1;
}

void PaintsNumberer::numerate_in_text()
{
    LOG("Start in-text numeration;");
    _document->open();

    duckx::Paragraph& _par = _document->paragraphs();
    for (int i = 0; _par.has_next(); _par.next(), i++)
    {
        unsigned int count = 0;
        std::wstring paragraph_text = get_wstring_paragraph_text(_par, count);
        std::string str_paragraph_text = boost::locale::conv::utf_to_utf<char>(paragraph_text);

        LOG("Checking paragraph: " + str_paragraph_text);
        if (!is_pattern_has(paragraph_text) && !is_has_intext_pattern(paragraph_text))
        {
            duckx::Run& _run = _par.runs();

            int run_counter = 0;
            bool is_partted_bracket = false;
            std::string semi_brackets, run_text;
            while(_run.has_next() && run_counter < count * 2)
            {
                if (run_text.empty())
                {
                    run_text = _run.get_text();
                }
                LOG("Check line: '" << run_text << "'. is parted bracked: " << is_partted_bracket);

                if (run_text.empty() && _run.has_next())
                {
                    _run = _run.next();
                    run_counter++;
                    continue;
                }
                auto _bracket_start = std::find(run_text.begin(), run_text.end(), '{');

                auto _bracket_end = std::find(run_text.begin(), run_text.end(), '}');

                if (_bracket_start != run_text.end()
                    && _bracket_end != run_text.end()
                    && _bracket_end - _bracket_start > 0 && !is_partted_bracket) {

                    int position = _bracket_start - run_text.begin();
                    std::string brackets(_bracket_start, _bracket_end);
                    boost::algorithm::trim_if(brackets, boost::algorithm::is_any_of("{}"));

                    LOG("Find match: " + brackets);
                    if (run_text.end() - _bracket_end > 0) {
                        run_text.erase(_bracket_start, ++_bracket_end);
                    }

                    unsigned int value = get_paint_num(brackets, i, run_counter);
                    run_text.insert(position, std::to_string(value));

                    LOG("New: " + run_text);
                    _run.set_text(run_text);

                }
                else if (_bracket_start != run_text.end() &&
                        (_bracket_end - _bracket_start <= 0
                            || _bracket_end == run_text.end()) && !is_partted_bracket
                        )
                {
                    std::string part_brackets(_bracket_start, run_text.end());
                    semi_brackets = part_brackets;
                    LOG("Find match: " + part_brackets);

                    if (!part_brackets.empty()) {
                        boost::algorithm::trim_if(part_brackets, boost::algorithm::is_any_of("{} "));
                    }

                    run_text.erase(_bracket_start, run_text.end());
                    _run.set_text(run_text);

                    is_partted_bracket = true;
                }
                else if (is_partted_bracket)
                {
                    if (_bracket_end == run_text.end())
                    {
                        LOG("Message: " + run_text);
                        semi_brackets += run_text;

                        LOG("Semi now: " + semi_brackets);
                        _run.set_text("");
                    }
                    else {
                        LOG("Message semi of end: " + run_text);
                        std::string parted_part(run_text.begin(), ++_bracket_end);

                        semi_brackets += parted_part;

                        run_text.erase(run_text.begin(), _bracket_end);

                        LOG("Semi now: " << semi_brackets);
                        boost::algorithm::trim_if(semi_brackets, boost::algorithm::is_any_of("{}"));

                        unsigned int value = get_paint_num(semi_brackets, i, run_counter);
                        run_text.insert(0, std::to_string(value));

                        LOG("Run text: " << run_text);
                        _run.set_text(run_text);

                        is_partted_bracket = false;
                    }
                }

                if ((_bracket_start == run_text.end()
                        && _bracket_end == run_text.end())
                    || is_partted_bracket)
                {
                    _run = _run.next();
                    run_text.clear();

                    run_counter++;
                }
            }
        }

    }

    _document->save();
    LOG("Numering is ended...");
}

long PaintsNumberer::get_paint_num(std::string bracket_expression, const unsigned int& paragraph_num, const int& run_num)
{
    bool is_before = false;
    std::string::iterator sym_iter = std::find_if(
                bracket_expression.begin(),
                bracket_expression.end(),
                boost::lambda2::_1 == 'P' || boost::lambda2::_1 == 'N'
            );

    if (*sym_iter == 'P') {
        is_before = true;
    }

    LOG(boost::format("Expression: '%s' - Paragraph: #%d; Run: #%d; Before or next: %d") % bracket_expression % paragraph_num % run_num % is_before);
    auto _compare = (is_before) ? [](const unsigned int& _first, const unsigned int& second){ return _first <= second; }
        : [](const unsigned int& _first, const unsigned int& second){ return _first >= second; };

    std::vector<unsigned int> ready_paints;
    for (const std::pair<unsigned int, std::vector<PaintPoint>>& _paint_data : _setted_paints)
    {
        LOG(boost::format("Comparing: %d and %d") % _paint_data.first % paragraph_num);
        if (_compare(_paint_data.first, paragraph_num))
        {
            LOG("Nice result...");
            for (const PaintPoint& _paint_data_n : _paint_data.second)
            {
                if (_compare(_paint_data.first, run_num))
                {
                    LOG("Inserting a paint with num: " << _paint_data_n.paint_num);
                    ready_paints.insert(_paint_data_n.paint_num);
                }
            }
        }
    }
    boost::algorithm::trim_if(bracket_expression, boost::algorithm::is_any_of("{}"));

    unsigned int _min_element = (is_before) ? *ready_paints.rbegin() : *ready_paints.begin();
    LOG("Found min: " << _min_element);
    std::string standart_variable;
    standart_variable += *sym_iter;

    exprtk::symbol_table<double> _symbol_table;
    exprtk::expression<double> _expression;
    exprtk::parser<double> _parser;

    _symbol_table.add_constant(standart_variable, _min_element);
    _symbol_table.add_constants();

    _expression.register_symbol_table(_symbol_table);
    _parser.compile(bracket_expression, _expression);
    double _dx = std::ceil(_expression.value() - 1);
    LOG("Step: " << _dx);

    return _min_element + static_cast<unsigned int>(_dx);
}

bool PaintsNumberer::is_pattern_has(std::wstring _text)
{
    bool _is_compared = false;
    for (const PatternTitle& pattern : _paint_patterns)
    {
        boost::wregex _pat(boost::locale::conv::utf_to_utf<wchar_t>(pattern.pattern));

        _is_compared = boost::regex_match(_text, _pat);
        LOG((boost::format("Checking pattern: '%s'. Is compared: %d...") % pattern.pattern % _is_compared).str());

        if (_is_compared)
        {
            break;
        }
    }

    return _is_compared;
}

bool PaintsNumberer::is_has_intext_pattern(std::wstring _text)
{
    std::string _text_in_str = boost::locale::conv::utf_to_utf<char>(_text);
    std::wstring _pattern = boost::locale::conv::utf_to_utf<wchar_t>(patterns_of_elements.in_text_paint_num.str());

    boost::wregex _pat(_pattern);
    bool _is_compared = boost::regex_match(_text, _pat);;

    LOG((boost::format("Checking line: '%s'. Is compared: %d...") % _text_in_str % _is_compared).str());

    return _is_compared;
}

void PaintsNumberer::numerate()
{
    _document->open();

    unsigned long paint_num = init_paint_num();
    duckx::Paragraph& _par = _document->paragraphs();
    for (int paragraph_num = 0; _par.has_next(); _par = _par.next(), paragraph_num++)
    {
        LOG("Getting texts...");

        std::wstring _text = get_wstring_paragraph_text(_par);

        LOG("Converting them...");
        LOG("Get paragraph's text: '" << boost::locale::conv::utf_to_utf<char>(_text) << "'. Paint num: '" << std::to_string(paint_num) << "'...");

        std::vector<duckx::Run> _runs = get_runs(_par);
        if (is_pattern_has(_text))
        {
            LOG("Pattern access...");
            performing_runs(_runs, paint_num, paragraph_num);
        }

        align_runs(_runs, _par.runs());
    }

    LOG("Saving doc...");
    _document->save();
}

void PaintsNumberer::performing_runs(std::vector<duckx::Run>& _runs, unsigned long& paint_num, const unsigned long& par_num) {
    LOG("Paint num: " << std::to_string(paint_num) << " Runs count: " << _runs.size());

    for (int i = 0; i < _runs.size(); i++) {
        duckx::Run& _run_of_par = _runs[i];
        std::string run_text = _run_of_par.get_text();
        LOG("Run text: " << run_text);

        boost::regex_iterator<std::string::const_iterator> _start =
            boost::make_regex_iterator(run_text, patterns_of_elements.paint_num_pat, boost::regex_constants::match_default | boost::regex_constants::match_partial);
        boost::regex_iterator<std::string::const_iterator> _end;

        LOG("Performing signs...");
        for (; _start != _end; _start++) {
            boost::smatch _match = *_start;

            if (_match[0].matched) {
                perform_full_text(run_text, _start, paint_num, par_num, i);

                _paints_count++;
            }
            else if (!_match[0].matched && _match[0].first != _match[0].second && _match[0].second - _match[0].first > 0) {
                LOG("Performing partial... Len: " << (_match[0].second - run_text.begin()));
                int start_index = i + 1;
                duckx::Run& _other_lines = _runs[start_index];
                LOG("Old: " << run_text << " Next: " << _other_lines.get_text());

                bool is_end = false;
                while (_other_lines.has_next() && !is_end)
                {
                    std::string _line = _other_lines.get_text();
                    LOG("Performing line: " << ((_line.empty()) ? "<Empty>" : _line));

                    if (!_line.empty())
                    {
                        _line = process_line(_line, paint_num, is_end);

                        LOG("New: " << ((_line.empty()) ? "<Empty>" : _line));
                        _other_lines.set_text(_line);
                    }

                    if (_other_lines.has_next() && start_index < _runs.size()) {
                        _other_lines = _runs[(!is_end) ? ++start_index : start_index];
                        LOG("Next: " << _other_lines.get_text() << ". Is has next: " << _other_lines.has_next() << ". Is ended: " << is_end);
                    }
                    else {
                        LOG("End of paragraph");
                        is_end = true;
                    }
                }

                add_paint_num_to_table(par_num, i, paint_num);
                std::string num = std::to_string(paint_num++);
                boost::replace_first(run_text, "{", num);

                i = start_index;
                _paints_count++;
            }
        }

        LOG("Ready run: " << run_text << " Old: " << _run_of_par.get_text());
        _run_of_par.set_text(run_text);
    }
}

std::vector<duckx::Run> PaintsNumberer::get_runs(duckx::Paragraph& _par)
{
    duckx::Run _run_of_par_ = _par.runs();
    std::vector<duckx::Run> _runs;

    std::wstringstream _text_of_paragraph;
    for (; _run_of_par_.has_next(); _run_of_par_ = _run_of_par_.next()) {
        _runs.push_back(_run_of_par_);
    }

    return _runs;
}

std::wstring PaintsNumberer::get_wstring_paragraph_text(duckx::Paragraph& _par)
{
    unsigned int runs = 0;
    return get_wstring_paragraph_text(_par, runs);
}

std::wstring PaintsNumberer::get_wstring_paragraph_text(duckx::Paragraph& _par, unsigned int& runs_count)
{
    runs_count = 0;
    duckx::Run _run_of_par_ = _par.runs();

    std::wstringstream _text_of_paragraph;
    for (; _run_of_par_.has_next(); _run_of_par_ = _run_of_par_.next()) {
        std::string text = _run_of_par_.get_text();

        _text_of_paragraph << boost::locale::conv::utf_to_utf<wchar_t>(text);
        runs_count++;
    }

    std::wstring result_text = _text_of_paragraph.str();
    return result_text;
}

void PaintsNumberer::perform_full_text(std::string& run_text, boost::sregex_iterator& _start, unsigned long& paint_num, const unsigned long& par_num, const int& run_num){
    std::string part = boost::algorithm::trim_copy(_start->str());

    LOG("Found: " << part << " Num: " << paint_num);
    if (boost::regex_match(part, patterns_of_elements.partitioner_paint_num_pat)){
        LOG("Update num...");
        paint_num = init_paint_num();
    }

    LOG("Replacing...");
    boost::algorithm::replace_first(run_text, part, std::to_string(paint_num));
    add_paint_num_to_table(par_num, run_num, paint_num);

    if (paint_num < _max_numeration || _max_numeration == 0) {
        paint_num++;
    }
    else {
        paint_num = init_paint_num();
    }
}

std::string PaintsNumberer::process_line(const std::string& input_line, unsigned long& paint_num, bool& is_end) {
    std::string _line = input_line;
    LOG("Processing line: " << input_line << " Paint num: " << std::to_string(paint_num));

    boost::smatch _match;
    if (!boost::regex_match(_line, _match, patterns_of_elements.partitioner_paint_num_pat, boost::regex_constants::match_partial)
        && _match[0].first != _match[0].second
        && _match[0].second - _match[0].first > 1)
    {
        LOG("Update num...");
        paint_num = init_paint_num();
    }

    LOG((boost::format("First symbol: '%d'; Second symbol: '%d'") % (_match[0].first - _line.begin()) % (_match[0].second - _line.begin())).str());
    std::string::iterator find_close_bracket = std::find(_line.begin(), _line.end(), '}');

    if (find_close_bracket != _line.end())
    {
        LOG("Clearing to closed bracked...");
        _line.erase(_line.begin(), ++find_close_bracket);
        is_end = true;
    }
    else {
        LOG("Clearing to closed bracked... No bracked in line...");
        _line.clear();
    }

    return _line;
}

void PaintsNumberer::align_runs(std::vector<duckx::Run>& _prepared_runs, duckx::Run& runs_old) {
    std::queue<duckx::Run> _queue_of_runs(_prepared_runs.begin(), _prepared_runs.end());

    LOG("Aligning runs... ");
    for (; runs_old.has_next(); runs_old.next()) {
        std::string new_run_text;

        if (!_queue_of_runs.empty()) {
            new_run_text = _queue_of_runs.front().get_text();
            _queue_of_runs.pop();
        }

        LOG("Old run: " << runs_old.get_text() << " New run text: " << new_run_text);
        runs_old.set_text(new_run_text);
    }
}
void PaintsNumberer::add_paint_num_to_table(const unsigned int& paragraph_num, const unsigned int& run_num, const unsigned int& paint_num)
{
    if (!_setted_paints.contains(paragraph_num))
    {
        _setted_paints[paragraph_num] = std::vector<PaintPoint>();
    }

    LOG(boost::format("Paragraph num: %d;\nRun num: %d;\nPaint num: %d") % paragraph_num % run_num % paint_num);
    _setted_paints[paragraph_num].push_back(PaintPoint(paragraph_num, run_num, paint_num));
}