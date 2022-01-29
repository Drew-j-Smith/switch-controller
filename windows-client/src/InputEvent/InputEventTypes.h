#include "ConstantInputEvent.h"
#include "InputEventCollection.h"
#include "InputEventToggle.h"
#include "SfJoystickInputEvent.h"
#include "SfKeyboardInputEvent.h"

#define AC_INPUT_EVENT_TYPES                                                   \
    ConstantInputEvent, InputEventCollection, InputEventToggle,                \
        SfJoystickInputEvent, SfKeyboardInputEvent

#define AC_INPUT_EVENT_NAMES                                                   \
    "ConstantInputEvent", "InputEventCollection", "InputEventToggle",          \
        "SfJoystickInputEvent", "SfKeyboardInputEvent"
