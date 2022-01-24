#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "InputEvent/ConstantInputEvent.h"
#include "InputEvent/InputEventInverter.h"
#include "InputEvent/SfJoystickAnalogInputEvent.h"

BOOST_AUTO_TEST_SUITE(InputEventInverterTest);
BOOST_AUTO_TEST_CASE(TestGetInputValueDefault) {
    InputEventInverter i;
    BOOST_TEST(i.getInputValue() == 1);
}
BOOST_AUTO_TEST_CASE(TestGetInputValueActive) {
    InputEventInverter i(std::make_shared<ConstantInputEvent>(1, true));
    BOOST_TEST(i.getInputValue() == 0);
}
BOOST_AUTO_TEST_CASE(TestIsDigital) {
    InputEventInverter i;
    BOOST_TEST(i.isDigital() == true);
}
BOOST_AUTO_TEST_CASE(TestGetTypeName) {
    InputEventInverter i;
    BOOST_TEST(i.getTypeName() == "InputEventInverter");
}
BOOST_AUTO_TEST_CASE(TestToString) {
    InputEventInverter i;
    BOOST_TEST(i.toString() == "InputEventInverter: (ConstantInputEvent)");
}
BOOST_AUTO_TEST_CASE(TestToPtree) {
    InputEventInverter i;
    boost::property_tree::ptree j = i.toPtree();
    BOOST_TEST(j.get<std::string>("type") == "InputEventInverter");
    BOOST_TEST(j.get_child("event") == ConstantInputEvent().toPtree());
    BOOST_TEST(j.size() == 2);
}
BOOST_AUTO_TEST_CASE(TestEqualsTrue) {
    InputEventInverter i, j;
    BOOST_TEST((i == j));
}
BOOST_AUTO_TEST_CASE(TestEqualsFalse) {
    InputEventInverter i, j(std::make_shared<InputEventInverter>());
    BOOST_TEST(!(i == j));
}
BOOST_AUTO_TEST_CASE(TestNotEqualsTrue) {
    InputEventInverter i, j(std::make_shared<InputEventInverter>());
    BOOST_TEST((i != j));
}
BOOST_AUTO_TEST_CASE(TestNotEqualsFalse) {
    InputEventInverter i, j;
    BOOST_TEST(!(i != j));
}
BOOST_AUTO_TEST_CASE(TestInvalidInputEvent) {
    try {
        InputEventInverter i(std::make_shared<ConstantInputEvent>(0, false));
        BOOST_TEST(false, "Error was not thrown");
    } catch ([[maybe_unused]] std::invalid_argument &e) {
        BOOST_TEST(true, "Error was thrown");
        return;
    }
}
BOOST_AUTO_TEST_SUITE_END();