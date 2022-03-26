#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "InputEvent/ConcreteClasses/ConstantInputEvent.h"

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

BOOST_AUTO_TEST_SUITE_END();