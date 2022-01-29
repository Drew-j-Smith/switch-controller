#include "ConstantInputEvent.h"
#include "InputEventCollection.h"
#include "InputEventToggle.h"
#include "SfJoystickAnalogInputEvent.h"
#include "SfJoystickDigitalInputEvent.h"
#include "SfKeyboardInputEvent.h"

#define AC_INPUT_EVENT_TYPES                                                   \
    ConstantInputEvent, InputEventCollection, InputEventToggle,                \
        SfJoystickAnalogInputEvent, SfJoystickDigitalInputEvent,               \
        SfKeyboardInputEvent

#define AC_INPUT_EVENT_NAMES                                                   \
    "ConstantInputEvent", "InputEventCollection", "InputEventToggle",          \
        "SfJoystickAnalogInputEvent", "SfJoystickDigitalInputEvent",           \
        "SfKeyboardInputEvent"
