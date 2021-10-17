#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(test_suite_1);
BOOST_AUTO_TEST_CASE(test_one)
{
  int i = 1;
  BOOST_TEST(i);
  BOOST_TEST(i == 1);
}

BOOST_AUTO_TEST_CASE(test_two)
{
  int i = 2;
  BOOST_TEST(i);
  BOOST_TEST(i == 2);
}
BOOST_AUTO_TEST_SUITE_END();