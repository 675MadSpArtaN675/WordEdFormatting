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
};

#endif