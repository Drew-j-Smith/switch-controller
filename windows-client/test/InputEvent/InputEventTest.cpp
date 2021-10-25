#include "pch.h"

#include <boost/test/unit_test.hpp>

#include "InputEvent/InputEvent.h"
#include "InputEvent/DefaultInputEvent.h"
#include "InputEvent/InputEventCollection.h"
#include "InputEvent/InputEventInverter.h"
#include "InputEvent/InputEventToggle.h"
#include "InputEvent/InputEventSwitch.h"
#include "InputEvent/SfJoystickAnalogInputEvent.h"
#include "InputEvent/SfJoystickDigitalInputEvent.h"
#include "InputEvent/SfKeyboardInputEvent.h"

typedef std::tuple<
DefaultInputEvent,
ActiveInputEvent,
InputEventCollection,
InputEventInverter,
InputEventToggle,
InputEventSwitch,
SfJoystickAnalogInputEvent,
SfJoystickDigitalInputEvent,
SfKeyboardInputEvent> testTypes;

BOOST_AUTO_TEST_SUITE(InputEventTest);
    BOOST_AUTO_TEST_CASE_TEMPLATE(contructorTest, T, testTypes)
    {
        T i, j;
        BOOST_TEST((i == j));
    }
    BOOST_AUTO_TEST_SUITE(InputEventEqualityTest);
        BOOST_AUTO_TEST_CASE_TEMPLATE(DefaultInputEventTestEquals, T, testTypes)
        {
            T i;
            DefaultInputEvent j;
            if (typeid(T) == typeid(DefaultInputEvent))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(DefaultInputEventTestNotEquals, T, testTypes)
        {
            T i;
            DefaultInputEvent j;
            if (typeid(T) == typeid(DefaultInputEvent))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(ActiveInputEventTestEquals, T, testTypes)
        {
            T i;
            ActiveInputEvent j;
            if (typeid(T) == typeid(ActiveInputEvent))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(ActiveInputEventTestNotEquals, T, testTypes)
        {
            T i;
            ActiveInputEvent j;
            if (typeid(T) == typeid(ActiveInputEvent))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventCollectionTestEquals, T, testTypes)
        {
            T i;
            InputEventCollection j;
            if (typeid(T) == typeid(InputEventCollection))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventCollectionTestNotEquals, T, testTypes)
        {
            T i;
            InputEventCollection j;
            if (typeid(T) == typeid(InputEventCollection))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventInverterTestEquals, T, testTypes)
        {
            T i;
            InputEventInverter j;
            if (typeid(T) == typeid(InputEventInverter))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventInverterTestNotEquals, T, testTypes)
        {
            T i;
            InputEventInverter j;
            if (typeid(T) == typeid(InputEventInverter))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventToggleTestEquals, T, testTypes)
        {
            T i;
            InputEventToggle j;
            if (typeid(T) == typeid(InputEventToggle))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventToggleTestNotEquals, T, testTypes)
        {
            T i;
            InputEventToggle j;
            if (typeid(T) == typeid(InputEventToggle))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventSwitchTestEquals, T, testTypes)
        {
            T i;
            InputEventSwitch j;
            if (typeid(T) == typeid(InputEventSwitch))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(InputEventSwitchTestNotEquals, T, testTypes)
        {
            T i;
            InputEventSwitch j;
            if (typeid(T) == typeid(InputEventSwitch))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickAnalogInputEventTestEquals, T, testTypes)
        {
            T i;
            SfJoystickAnalogInputEvent j;
            if (typeid(T) == typeid(SfJoystickAnalogInputEvent))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickAnalogInputEventTestNotEquals, T, testTypes)
        {
            T i;
            SfJoystickAnalogInputEvent j;
            if (typeid(T) == typeid(SfJoystickAnalogInputEvent))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickDigitalInputEventTestEquals, T, testTypes)
        {
            T i;
            SfJoystickDigitalInputEvent j;
            if (typeid(T) == typeid(SfJoystickDigitalInputEvent))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(SfJoystickDigitalInputEventTestNotEquals, T, testTypes)
        {
            T i;
            SfJoystickDigitalInputEvent j;
            if (typeid(T) == typeid(SfJoystickDigitalInputEvent))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }


        BOOST_AUTO_TEST_CASE_TEMPLATE(SfKeyboardInputEventTestEquals, T, testTypes)
        {
            T i;
            SfKeyboardInputEvent j;
            if (typeid(T) == typeid(SfKeyboardInputEvent))
                BOOST_TEST((i == j));
            else
                BOOST_TEST(!(i == j));
        }

        BOOST_AUTO_TEST_CASE_TEMPLATE(SfKeyboardInputEventTestNotEquals, T, testTypes)
        {
            T i;
            SfKeyboardInputEvent j;
            if (typeid(T) == typeid(SfKeyboardInputEvent))
                BOOST_TEST(!(i != j));
            else
                BOOST_TEST((i != j));
        }
    BOOST_AUTO_TEST_SUITE_END();
BOOST_AUTO_TEST_SUITE_END();