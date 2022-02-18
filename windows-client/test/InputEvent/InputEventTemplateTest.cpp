#include <boost/test/unit_test.hpp>

#include "pch.h"

#include "InputEvent/InputEventTemplate.h"
#include <boost/property_tree/json_parser.hpp>

BOOST_AUTO_TEST_SUITE(InputEventTemplateTest);

BOOST_AUTO_TEST_CASE(TestCreateTree1) {
    boost::property_tree::ptree testTree, testTreeTemplate, expected;
    std::stringstream testTreeStr, testTreeTemplateStr, expectedStr;
    testTreeStr << R"({
        "test": "0"
    })";
    testTreeTemplateStr << R"({
        "type": "ConstantInputEvent",
        "isDigital": "1",
        "$test": {
            "replacement name": "inputValue",
            "description": "This is a test field",
            "type": "Integer"
        },
        "test field": {
            "$test": {
                "replacement name": "inputValue",
                "description": "This is a test field",
                "type": "Integer"
            }
        }
    })";
    expectedStr << R"({
        "type": "ConstantInputEvent",
        "isDigital": "1",
        "inputValue": "0",
        "test field": {
            "inputValue": "0"
        }
    })";
    boost::property_tree::read_json(testTreeStr, testTree);
    boost::property_tree::read_json(testTreeTemplateStr, testTreeTemplate);
    boost::property_tree::read_json(expectedStr, expected);
    auto actual = InputEventTemplate::createTree(testTreeTemplate, testTree);
    BOOST_TEST((actual == expected));
}
BOOST_AUTO_TEST_CASE(TestCreateTree2) {}
BOOST_AUTO_TEST_CASE(TestCreateTree3) {}
BOOST_AUTO_TEST_CASE(TestCreateTree4) {}

BOOST_AUTO_TEST_CASE(TestGetSchema1) {
    boost::property_tree::ptree testTreeTemplate;
    std::stringstream testTreeTemplateStr;
    testTreeTemplateStr << R"({
        "type": "ConstantInputEvent",
        "isDigital": "1",
        "$test": {
            "replacement name": "inputValue",
            "description": "This is a test field",
            "type": "Integer"
        },
        "test field": {
            "$test": {
                "replacement name": "inputValue",
                "description": "This is a test field",
                "type": "Integer"
            }
        }
    })";
    boost::property_tree::read_json(testTreeTemplateStr, testTreeTemplate);
    InputEventTemplate test(testTreeTemplate);
    auto schema = test.getSchema();

    BOOST_TEST(schema.size() == 1);
    BOOST_TEST(schema[0].name == "test");
    BOOST_TEST(schema[0].description == "This is a test field");
    BOOST_TEST(schema[0].type == InputEvent::SchemaItem::SchemaType::Integer);
}
BOOST_AUTO_TEST_CASE(TestGetSchema2) {}
BOOST_AUTO_TEST_CASE(TestGetSchema3) {}
BOOST_AUTO_TEST_CASE(TestGetSchema4) {}

BOOST_AUTO_TEST_CASE(TestConstructor1) {
    boost::property_tree::ptree testTree, testTreeTemplate;
    std::stringstream testTreeStr, testTreeTemplateStr;
    testTreeStr << R"({
        "test": "0"
    })";
    testTreeTemplateStr << R"({
        "type": "ConstantInputEvent",
        "isDigital": "1",
        "$test": {
            "replacement name": "inputValue",
            "description": "This is a test field",
            "type": "Integer"
        },
        "test field": {
            "$test": {
                "replacement name": "inputValue",
                "description": "This is a test field",
                "type": "Integer"
            }
        }
    })";
    boost::property_tree::read_json(testTreeStr, testTree);
    boost::property_tree::read_json(testTreeTemplateStr, testTreeTemplate);

    InputEventFactory factory({{"test", testTreeTemplate}});

    InputEventTemplate test(testTreeTemplate, testTree, factory);
    BOOST_TEST(test.isDigital() == true);
    BOOST_TEST(test.getInputValue() == 0);
}
BOOST_AUTO_TEST_CASE(TestConstructor2) {}
BOOST_AUTO_TEST_CASE(TestConstructor3) {}
BOOST_AUTO_TEST_CASE(TestConstructor4) {}

BOOST_AUTO_TEST_SUITE_END();