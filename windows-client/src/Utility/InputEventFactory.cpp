
#include "InputEventFactory.h"
#include "InputEvent/DefaultInputEvent.h"
#include "InputEvent/InputEventCollection.h"
#include "InputEvent/InputEventInverter.h"
#include "InputEvent/InputEventSwitch.h"
#include "InputEvent/InputEventToggle.h"
#include "InputEvent/InputEventTurbo.h"
#include "InputEvent/SfJoystickAnalogInputEvent.h"
#include "InputEvent/SfJoystickDigitalInputEvent.h"
#include "InputEvent/SfKeyboardInputEvent.h"

InputEventFactory::InputEventFactory() {
#define AC_ADD_FACTORY(class)                                                  \
    factories[#class] = boost::bind(boost::factory<std::shared_ptr<class>>(),  \
                                    _1, boost::ref(*this));
    AC_ADD_FACTORY(DefaultInputEvent);
    AC_ADD_FACTORY(ActiveInputEvent);
    AC_ADD_FACTORY(InputEventCollection);
    AC_ADD_FACTORY(InputEventInverter);
    AC_ADD_FACTORY(InputEventSwitch);
    AC_ADD_FACTORY(InputEventToggle);
    AC_ADD_FACTORY(InputEventTurbo);
    AC_ADD_FACTORY(SfJoystickAnalogInputEvent);
    AC_ADD_FACTORY(SfJoystickDigitalInputEvent);
    AC_ADD_FACTORY(SfKeyboardInputEvent);
}