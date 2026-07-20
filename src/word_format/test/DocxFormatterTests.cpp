#define BOOST_TEST_MODULE MyTestSuite

#include <boost/test/unit_test.hpp>

#include <string>
#include "formatter.hpp"

BOOST_AUTO_TEST_CASE(DocxFormatterInitTest)
{
    Formatter _formatter;

    BOOST_TEST(_formatter.count() == 0);
}

BOOST_AUTO_TEST_CASE(DocxFormatterComboFillTest)
{
    Formatter _formatter("FTestFile_1.docx");

    _formatter.bindArg<std::string>("FIO", "Иванов Иван Иванович");
    _formatter.bindArg<std::string>("GROUP", "бИСТ-232");

    BOOST_TEST(_formatter.count() == 2);
}