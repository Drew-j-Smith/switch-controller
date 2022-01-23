#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "InputEvent/DefaultInputEvent.h"

BOOST_AUTO_TEST_SUITE(DefaultInputEventTest);
BOOST_AUTO_TEST_CASE(TestConstructor) {
    DefaultInputEvent i, j;
    BOOST_TEST((i == j));
}
BOOST_AUTO_TEST_CASE(TestGetInputValue) {
    DefaultInputEvent i;
    BOOST_TEST(i.getInputValue() == 0);
}
BOOST_AUTO_TEST_CASE(TestIsDigital) {
    DefaultInputEvent i;
    BOOST_TEST(i.isDigital() == true);
}
BOOST_AUTO_TEST_CASE(TestMakeShared) {
    DefaultInputEvent i;
    auto factory = Factory<InputEvent>({});
    std::shared_ptr<InputEvent> j =
        i.makeShared(boost::property_tree::ptree(), factory);
    BOOST_TEST((i == *j));
}
BOOST_AUTO_TEST_CASE(TestGetTypeName) {
    DefaultInputEvent i;
    BOOST_TEST(i.getTypeName() == "DefaultInputEvent");
}
BOOST_AUTO_TEST_CASE(TestUpdate) {
    DefaultInputEvent i, j;
    i.update();
    BOOST_TEST((i == j));
}
BOOST_AUTO_TEST_CASE(TestToString) {
    DefaultInputEvent i;
    BOOST_TEST(i.getTypeName() == "DefaultInputEvent");
}
BOOST_AUTO_TEST_CASE(TestToPtree) {
    DefaultInputEvent i;
    boost::property_tree::ptree j = i.toPtree();
    BOOST_TEST(j.get<std::string>("type") == "DefaultInputEvent");
    BOOST_TEST(j.size() == 1);
}
BOOST_AUTO_TEST_SUITE(TestEquals);
BOOST_AUTO_TEST_CASE(TestEqualsDefaultInputEvent) {
    DefaultInputEvent i, j;
    BOOST_TEST((i == j));
}
BOOST_AUTO_TEST_CASE(TestNotEqualsDefaultInputEvent) {
    DefaultInputEvent i, j;
    BOOST_TEST(!(i != j));
}

BOOST_AUTO_TEST_CASE(TestEqualsActiveInputEvent) {
    DefaultInputEvent i;
    ActiveInputEvent j;
    BOOST_TEST(!(i == j));
}
BOOST_AUTO_TEST_CASE(TestNotEqualsActiveInputEvent) {
    DefaultInputEvent i;
    ActiveInputEvent j;
    BOOST_TEST((i != j));
}
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_SUITE(ActiveInputEventTest);
BOOST_AUTO_TEST_CASE(TestGetInputValue) {
    ActiveInputEvent i;
    BOOST_TEST(i.getInputValue() == 1);
}
BOOST_AUTO_TEST_CASE(TestIsDigital) {
    ActiveInputEvent i;
    BOOST_TEST(i.isDigital() == true);
}
BOOST_AUTO_TEST_CASE(TestMakeShared) {
    ActiveInputEvent i;
    auto factory = Factory<InputEvent>({});
    std::shared_ptr<InputEvent> j =
        i.makeShared(boost::property_tree::ptree(), factory);
    BOOST_TEST((i == *j));
}
BOOST_AUTO_TEST_CASE(TestGetTypeName) {
    ActiveInputEvent i;
    BOOST_TEST(i.getTypeName() == "ActiveInputEvent");
}
BOOST_AUTO_TEST_CASE(TestUpdate) {
    ActiveInputEvent i, j;
    i.update();
    BOOST_TEST((i == j));
}
BOOST_AUTO_TEST_CASE(TestToString) {
    ActiveInputEvent i;
    BOOST_TEST(i.getTypeName() == "ActiveInputEvent");
}
BOOST_AUTO_TEST_CASE(TestToPtree) {
    ActiveInputEvent i;
    boost::property_tree::ptree j = i.toPtree();
    BOOST_TEST(j.get<std::string>("type") == "ActiveInputEvent");
    BOOST_TEST(j.size() == 1);
}
BOOST_AUTO_TEST_SUITE_END();