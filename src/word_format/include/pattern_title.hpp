#ifndef PATTERN_TITLES
#define PATTERN_TITLES

#include <string>

#include <boost/regex.hpp>

#include "exporter.hpp"

struct EXPORT PatternTitle {
    std::string pattern;
    bool is_title_clear;

    PatternTitle();
    PatternTitle(std::string _pattern, bool _is_title_clear);

    boost::regex pattern_to_regex() const;

    bool operator==(const PatternTitle& _pt) const noexcept;
};

template<>
struct std::hash<PatternTitle> {
    std::size_t operator()(const PatternTitle& pt) const noexcept;
};

#endif