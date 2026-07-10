#include "pattern_title.hpp"

PatternTitle::PatternTitle() : PatternTitle("", false)
{ }

PatternTitle::PatternTitle(std::string _pattern, bool _is_title_clear) : pattern(_pattern), is_title_clear(_is_title_clear)
{ }

boost::regex PatternTitle::pattern_to_regex() const
{
    return boost::regex(pattern);
}

bool PatternTitle::operator==(const PatternTitle& _pt) const noexcept
{
    return _pt.pattern == _pt.pattern;
}

std::size_t std::hash<PatternTitle>::operator()(const PatternTitle& pt) const noexcept
{
    std::hash<std::string> _pattern_hash{};

    return _pattern_hash(pt.pattern);
}