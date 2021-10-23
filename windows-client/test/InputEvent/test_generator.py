test_case = """
BOOST_AUTO_TEST_CASE_TEMPLATE({0}TestEquals, T, testTypes)
{{
    T i;
    {0} j;
    if (typeid(T) == typeid({0}))
        BOOST_TEST((i == j));
    else
        BOOST_TEST(!(i == j));
}}

BOOST_AUTO_TEST_CASE_TEMPLATE({0}TestNotEquals, T, testTypes)
{{
    T i;
    {0} j;
    if (typeid(T) == typeid({0}))
        BOOST_TEST(!(i != j));
    else
        BOOST_TEST((i != j));
}}
"""
types = [
"DefaultInputEvent",
"ActiveInputEvent",
"InputEventCollection",
"InputEventInverter",
"InputEventToggle",
"InputEventSwitch",
"SfJoystickAnalogInputEvent",
"SfJoystickDigitalInputEvent",
"SfKeyboardInputEvent",
]
for type in types:
    print(test_case.format(type))