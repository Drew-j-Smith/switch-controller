
#include "InputEventFactory.h"
#include "InputEvent/InputEventTypes.h"

InputEventFactory::InputEventFactory() {
    std::vector<std::string> names = {AC_INPUT_EVENT_NAMES};
    addClass<AC_INPUT_EVENT_TYPES>(0, names);
}