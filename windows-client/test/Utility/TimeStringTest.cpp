
#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "Utility/TimeString.h"

BOOST_AUTO_TEST_SUITE(TimeStringTest);
    BOOST_AUTO_TEST_CASE(TimeStringTestCase)
    {
        std::string time = getTimeStr();
    }
BOOST_AUTO_TEST_SUITE_END();
