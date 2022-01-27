#include "ConstantInputEvent.h"
#include "InputEventCollection.h"
#include "InputEventInverter.h"
#include "InputEventSwitch.h"
#include "InputEventToggle.h"
#include "InputEventTurbo.h"
#include "SfJoystickAnalogInputEvent.h"
#include "SfJoystickDigitalInputEvent.h"
#include "SfKeyboardInputEvent.h"

#define AC_INPUT_EVENT_TYPES                                                   \
    ConstantInputEvent, InputEventCollection, InputEventInverter,              \
        InputEventToggle, InputEventSwitch, SfJoystickAnalogInputEvent,        \
        SfJoystickDigitalInputEvent, SfKeyboardInputEvent

#define AC_INPUT_EVENT_NAMES                                                   \
    "ConstantInputEvent", "InputEventCollection", "InputEventInverter",        \
        "InputEventToggle", "InputEventSwitch", "SfJoystickAnalogInputEvent",  \
        "SfJoystickDigitalInputEvent", "SfKeyboardInputEvent"
