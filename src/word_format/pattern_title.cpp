#include "pattern_title.hpp"

PatternTitle::PatternTitle() : PatternTitle("", false)
{ }

PatternTitle::PatternTitle(std::string _pattern, bool _is_title_clear) : pattern(_pattern), is_title_clear(_is_title_clear)
{ }

boost::regex PatternTitle::pattern_to_regex() const
{
    return boost::regex(pattern);
}
