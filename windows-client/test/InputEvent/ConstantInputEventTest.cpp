#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "InputEvent/ConcreteClasses/ConstantInputEvent.h"
#include <boost/property_tree/json_parser.hpp>

BOOST_AUTO_TEST_SUITE(ConstantInputEventTest);
BOOST_AUTO_TEST_CASE(TestDefaultConstructor) {
    ConstantInputEvent i;
    BOOST_TEST(i.getInputValue() == 0);
    BOOST_TEST(i.isDigital() == true);
}
BOOST_AUTO_TEST_CASE(TestFillConstructor) {
    ConstantInputEvent i(1, false);
    BOOST_TEST(i.getInputValue() == 1);
    BOOST_TEST(i.isDigital() == false);
}
BOOST_AUTO_TEST_CASE(TestPtreeConstructor1) {
    InputEventFactory factory({});
    ConstantInputEvent i(boost::property_tree::ptree(), factory);
}
BOOST_AUTO_TEST_CASE(TestPtreeConstructor2) {
    InputEventFactory factory({});
    std::stringstream ss;
    ss << R"({
        "type": "ConstantInputEvent",
        "inputValue": 1,
        "isDigital": 1
    })";
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(ss, tree);
    ConstantInputEvent i(tree, factory);
    BOOST_TEST(i.getInputValue() == 1);
    BOOST_TEST(i.isDigital() == true);
}

BOOST_AUTO_TEST_SUITE_END();