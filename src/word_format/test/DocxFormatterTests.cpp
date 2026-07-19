#define BOOST_TEST_MODULE MyTestSuite

#include <boost/test/included/unit_test.hpp>

#include "formatter.hpp"

BOOST_AUTO_TEST_CASE(DocxFormatterInitTest)
{
    Formatter _formatter;

    BOOST_TEST(_formatter.count() == 0);
}