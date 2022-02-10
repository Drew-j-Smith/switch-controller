#include "ConstantInputEvent.h"
#include "InputEventCollection.h"
#include "InputEventTemplate.h"
#include "InputEventToggle.h"
#include "SfJoystickInputEvent.h"
#include "SfKeyboardInputEvent.h"

struct InputEventSwitchStruct {
    static constexpr std::string_view stringView = R"({
    "type": "InputEventCollection",
    "operator": "or",
    "events": [
        {
            "type": "InputEventCollection",
            "operator": "and",
            "events": {
                "$event1": {
                    "replacement name": "",
                    "type": "Event",
                    "description": "The event to use when the switch event is true"
                },
                "$switchEvent": {
                    "replacement name": "",
                    "type": "Event",
                    "description": "The event used to decide which input event to use"
                }
            }
        },
        {
            "type": "InputEventCollection",
            "operator": "and",
            "events": {
                "$event2": {
                    "replacement name": "",
                    "type": "Event",
                    "description": "The event to use when the switch event is false"
                },
                "Inverted Switch event": {
                    "type": "InputEventCollection",
                    "operator": "not",
                    "events": {
                        "$switchEvent": {
                            "replacement name": "",
                            "type": "Event",
                            "description": "The event used to decide which input event to use"
                        }
                    }
                }
            }
        }
    ]
})";
};
using InputEventSwitch = InputEventTemplate<InputEventSwitchStruct>;

struct InputEventTurboStruct {
    static constexpr std::string_view stringView = R"({
    "type": "InputEventCollection",
    "operator": "and",
    "events": {
        "$event": {
            "replacement name": "",
            "type": "Event",
            "description": "The event to used eo enable turbo"
        }, 
        "": {
            "type": "InputEventToggle",
            "button cooldown": "15",
            "event": {
                "type": "ConstantInputEvent",
                "isDigital": 1,
                "inputValue": 1
            }
        }
    }
})";
};
using InputEventTurbo = InputEventTemplate<InputEventTurboStruct>;

#define AC_INPUT_EVENT_TYPES                                                   \
    ConstantInputEvent, InputEventCollection, InputEventToggle,                \
        SfJoystickInputEvent, SfKeyboardInputEvent, InputEventSwitch,          \
        InputEventTurbo

#define AC_INPUT_EVENT_NAMES                                                   \
    "ConstantInputEvent", "InputEventCollection", "InputEventToggle",          \
        "SfJoystickInputEvent", "SfKeyboardInputEvent", "InputEventSwitch",    \
        "InputEventTurbo"
