#define BOOST_TEST_MODULE MyTestSuite

#include <boost/test/included/unit_test.hpp>

#include "paints_numberer.hpp"

BOOST_AUTO_TEST_CASE(NumbererInitializationTest)
{
    PaintsNumberer _numberer;

    BOOST_TEST_MESSAGE("Testing numberer initialization...");
    BOOST_TEST(_numberer.empty());

    _numberer.setFile("TestFile_1.docx");

    BOOST_TEST_MESSAGE("Testing numberer setting file...");
    BOOST_TEST(_numberer.empty());
}

BOOST_AUTO_TEST_CASE(NumbererNumeratingTest)
{
    PaintsNumberer _numberer;

    _numberer.setFile("TestFile_1.docx");
    _numberer.addPaintPattern("Рисунок\\s*\\{.*\\}\\s*[‒–—−―]?[\\sа-яА-Я\\d]*", false);
    _numberer.numerate();

    BOOST_TEST_MESSAGE("Testing numeration...");
    BOOST_TEST(!_numberer.empty());
}

BOOST_AUTO_TEST_CASE(NumbererNumeratingWithPartitionsTest)
{
    try {
        PaintsNumberer _numberer;

        _numberer.setFile("TestFile_2.docx");
        _numberer.addPaintPattern("Рисунок\\s*\\{.*\\}\\s*[‒–—−―]?[\\sа-яА-Я\\w]*", false);
        _numberer.numerate();

        BOOST_TEST_MESSAGE("Testing numeration with partition...");
        BOOST_TEST(!_numberer.empty());
    }
    catch (const std::exception& _error){
        BOOST_TEST_MESSAGE(std::string("Error: ") + _error.what());
    }
}

BOOST_AUTO_TEST_CASE(NumbererInTextNumerationTest)
{
    try {
        PaintsNumberer _numberer;

        _numberer.setFile("TestFile_4.docx");
        _numberer.numerate();
        _numberer.numerate_in_text();

        BOOST_TEST_MESSAGE("Testing numeration with partition...");
        BOOST_TEST(!_numberer.empty());
    }
    catch (const std::exception& _error){
        BOOST_TEST_MESSAGE(std::string("Error: ") + _error.what());
    }

}

BOOST_AUTO_TEST_CASE(NumbererInTextNumerationWithPartitionTest)
{
    try {
        PaintsNumberer _numberer;

        _numberer.setFile("TestFile_3.docx");
        _numberer.numerate();
        _numberer.numerate_in_text();

        BOOST_TEST_MESSAGE("Testing numeration with partition...");
        BOOST_TEST(!_numberer.empty());
    }
    catch (const std::exception& _error){
        BOOST_TEST_MESSAGE(std::string("Error: ") + _error.what());
    }
}
