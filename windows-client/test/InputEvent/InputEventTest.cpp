#include "pch.h"

#include <boost/test/unit_test.hpp>

#include "InputEvent/InputEventTypes.h"

typedef std::tuple<AC_INPUT_EVENT_TYPES> testTypes;

BOOST_AUTO_TEST_SUITE(InputEventTest);
BOOST_AUTO_TEST_CASE_TEMPLATE(contructorTest, T, testTypes) {
    T i, j;
    BOOST_TEST((i == j));
}
BOOST_AUTO_TEST_SUITE(InputEventEqualityTest);
BOOST_AUTO_TEST_CASE_TEMPLATE(DefaultInputEventTestEquals, T, testTypes) {
    T i;
    ConstantInputEvent j;
    if (typeid(T) == typeid(ConstantInputEvent))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(DefaultInputEventTestNotEquals, T, testTypes) {
    T i;
    ConstantInputEvent j;
    if (typeid(T) == typeid(ConstantInputEvent))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventCollectionTestEquals, T, testTypes) {
    T i;
    InputEventCollection j;
    if (typeid(T) == typeid(InputEventCollection))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventCollectionTestNotEquals, T, testTypes) {
    T i;
    InputEventCollection j;
    if (typeid(T) == typeid(InputEventCollection))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventInverterTestEquals, T, testTypes) {
    T i;
    InputEventInverter j;
    if (typeid(T) == typeid(InputEventInverter))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventInverterTestNotEquals, T, testTypes) {
    T i;
    InputEventInverter j;
    if (typeid(T) == typeid(InputEventInverter))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventToggleTestEquals, T, testTypes) {
    T i;
    InputEventToggle j;
    if (typeid(T) == typeid(InputEventToggle))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventToggleTestNotEquals, T, testTypes) {
    T i;
    InputEventToggle j;
    if (typeid(T) == typeid(InputEventToggle))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventSwitchTestEquals, T, testTypes) {
    T i;
    InputEventSwitch j;
    if (typeid(T) == typeid(InputEventSwitch))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventSwitchTestNotEquals, T, testTypes) {
    T i;
    InputEventSwitch j;
    if (typeid(T) == typeid(InputEventSwitch))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickAnalogInputEventTestEquals, T,
                              testTypes) {
    T i;
    SfJoystickAnalogInputEvent j;
    if (typeid(T) == typeid(SfJoystickAnalogInputEvent))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickAnalogInputEventTestNotEquals, T,
                              testTypes) {
    T i;
    SfJoystickAnalogInputEvent j;
    if (typeid(T) == typeid(SfJoystickAnalogInputEvent))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickDigitalInputEventTestEquals, T,
                              testTypes) {
    T i;
    SfJoystickDigitalInputEvent j;
    if (typeid(T) == typeid(SfJoystickDigitalInputEvent))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickDigitalInputEventTestNotEquals, T,
                              testTypes) {
    T i;
    SfJoystickDigitalInputEvent j;
    if (typeid(T) == typeid(SfJoystickDigitalInputEvent))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SfKeyboardInputEventTestEquals, T, testTypes) {
    T i;
    SfKeyboardInputEvent j;
    if (typeid(T) == typeid(SfKeyboardInputEvent))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}

BOOST_AUTO_TEST_CASE_TEMPLATE(SfKeyboardInputEventTestNotEquals, T, testTypes) {
    T i;
    SfKeyboardInputEvent j;
    if (typeid(T) == typeid(SfKeyboardInputEvent))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}
BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();