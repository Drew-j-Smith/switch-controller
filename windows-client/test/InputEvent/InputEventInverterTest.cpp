#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "InputEvent/InputEventInverter.h"
#include "InputEvent/DefaultInputEvent.h"
#include "InputEvent/SfJoystickAnalogInputEvent.h"

BOOST_AUTO_TEST_SUITE(InputEventInverterTest);
    BOOST_AUTO_TEST_CASE(TestGetInputValueDefault)
    {
        InputEventInverter i;
        BOOST_TEST(i.getInputValue() == 1);
    }
    BOOST_AUTO_TEST_CASE(TestGetInputValueActive)
    {
        InputEventInverter i(std::make_shared<ActiveInputEvent>());
        BOOST_TEST(i.getInputValue() == 0);
    }
    BOOST_AUTO_TEST_CASE(TestIsDigital)
    {
        InputEventInverter i;
        BOOST_TEST(i.isDigital() == true);
    }
    BOOST_AUTO_TEST_CASE(TestMakeSharedDefault)
    {
        InputEventInverter i;
        boost::property_tree::ptree tree;
        tree.add_child("type", boost::property_tree::ptree("InputEventInverter"));
        tree.add_child("event", DefaultInputEvent().toPtree());
        std::map<std::string, std::shared_ptr<InputEvent>> map = {{"DefaultInputEvent", std::make_shared<DefaultInputEvent>()}};
        Factory<InputEvent> factory(map);
        std::shared_ptr<InputEvent> j = i.makeShared(tree, factory);
        BOOST_TEST((i == *j));
    }
    BOOST_AUTO_TEST_CASE(TestMakeSharedActive)
    {
        InputEventInverter i(std::make_shared<ActiveInputEvent>());
        boost::property_tree::ptree tree;
        tree.add_child("type", boost::property_tree::ptree("InputEventInverter"));
        tree.add_child("event", ActiveInputEvent().toPtree());
        std::map<std::string, std::shared_ptr<InputEvent>> map = {{"ActiveInputEvent", std::make_shared<ActiveInputEvent>()}};
        Factory<InputEvent> factory(map);
        std::shared_ptr<InputEvent> j = i.makeShared(tree, factory);
        BOOST_TEST((i == *j));
    }
    BOOST_AUTO_TEST_CASE(TestGetTypeName)
    {
        InputEventInverter i;
        BOOST_TEST(i.getTypeName() == "InputEventInverter");
    }
    BOOST_AUTO_TEST_CASE(TestToString)
    {
        InputEventInverter i;
        BOOST_TEST(i.toString() == "InputEventInverter: (DefaultInputEvent)");
    }
    BOOST_AUTO_TEST_CASE(TestToPtree)
    {
        InputEventInverter i;
        boost::property_tree::ptree j = i.toPtree();
        BOOST_TEST(j.get<std::string>("type") == "InputEventInverter");
        BOOST_TEST(j.get_child("event") == DefaultInputEvent().toPtree());
        BOOST_TEST(j.size() == 2);
    }
    BOOST_AUTO_TEST_CASE(TestEqualsTrue) {
        InputEventInverter i, j;
        BOOST_TEST((i == j));
    }
    BOOST_AUTO_TEST_CASE(TestEqualsFalse) {
        InputEventInverter i, j(std::make_shared<ActiveInputEvent>());
        BOOST_TEST(!(i == j));
    }
    BOOST_AUTO_TEST_CASE(TestNotEqualsTrue) {
        InputEventInverter i, j(std::make_shared<ActiveInputEvent>());
        BOOST_TEST((i != j));
    }
    BOOST_AUTO_TEST_CASE(TestNotEqualsFalse) {
        InputEventInverter i, j;
        BOOST_TEST(!(i != j));
    }
    BOOST_AUTO_TEST_CASE(TestInvalidInputEvent) {
        try {
            InputEventInverter i(std::make_shared<SfJoystickAnalogInputEvent>());
            BOOST_TEST(false, "Error was not thrown");
        }
        catch (std::invalid_argument& e) {
            BOOST_TEST(true, "Error was thrown");
            return;
        }
        
    }
BOOST_AUTO_TEST_SUITE_END();