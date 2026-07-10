#ifndef CONSTANTS_
#define CONSTANTS_

#include <string>

#include <boost/regex.hpp>

constexpr std::string PAINT_NUM_PLACE = "{[\\w_-]*}";
constexpr std::string PARTITIONER_PATTERN = "{NP}";
constexpr std::string IN_TEXT_PAINT_NUM_PLACE = "({([+-]?\\d+)?})";

struct PATTERNS {
    const boost::regex paint_num_pat;
    const boost::regex partitioner_paint_num_pat;
    const boost::regex in_text_paint_num;

    PATTERNS() :
        paint_num_pat(PAINT_NUM_PLACE),
        partitioner_paint_num_pat(PARTITIONER_PATTERN),
        in_text_paint_num(IN_TEXT_PAINT_NUM_PLACE) {}

};

#endif