#include <boost/test/unit_test.hpp>

#include "InputEvent/InputEventGenerator.h"
#include "InputEvent/DefaultInputEvent.h"


BOOST_AUTO_TEST_SUITE(InputEventGeneratorTest);
    BOOST_AUTO_TEST_CASE(constructor)
    {
        InputEventGenerator i, j;
        BOOST_TEST((i == j));
    }

    BOOST_AUTO_TEST_SUITE(RegisterInputEvent);
        BOOST_AUTO_TEST_CASE(RegisterDefaultInputEvent)
        {
            InputEventGenerator i, j;
            i.registerInputEvent<DefaultInputEvent>();
            j.registerInputEvent<DefaultInputEvent>();
            BOOST_TEST((i == j));
        }
        BOOST_AUTO_TEST_CASE(RegisterActiveInputEvent)
        {
            InputEventGenerator i, j;
            i.registerInputEvent<ActiveInputEvent>();
            j.registerInputEvent<ActiveInputEvent>();
            BOOST_TEST((i == j));
        }
        BOOST_AUTO_TEST_CASE(RegisterActiveAndDefaultInputEvent)
        {
            InputEventGenerator i, j;
            i.registerInputEvent<ActiveInputEvent>();
            j.registerInputEvent<ActiveInputEvent>();
            i.registerInputEvent<DefaultInputEvent>();
            j.registerInputEvent<DefaultInputEvent>();
            BOOST_TEST((i == j));
        }
    BOOST_AUTO_TEST_SUITE_END();

    BOOST_AUTO_TEST_SUITE(TestEquals);
        BOOST_AUTO_TEST_CASE(NotEqualEmptyDefault)
        {
            InputEventGenerator i, j;
            j.registerInputEvent<DefaultInputEvent>();
            BOOST_TEST((!(i == j)));
        }
        BOOST_AUTO_TEST_CASE(NotEqualDefaultActive)
        {
            InputEventGenerator i, j;
            i.registerInputEvent<ActiveInputEvent>();
            j.registerInputEvent<DefaultInputEvent>();
            BOOST_TEST((!(i == j)));
        }
    BOOST_AUTO_TEST_SUITE_END();

BOOST_AUTO_TEST_SUITE_END();