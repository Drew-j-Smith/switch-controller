#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

/**
 * @file InputManager.h
 * @author Drew Smith
 * @brief This file is the header for the InputManager class which
 * takes inputs and converts them into a character array which can be sent to
 * the arduino
 * @date 2021-09-11
 *
 */
#include "pch.h"

#include "InputEvent/InputEvent.h"
#include "InputEventFactory.h"

/**
 * @brief This class takes inputs and converts them into a character array which
 * can be sent to the arduino
 *
 * @invariant The elements in inputs will not be null
 */
class InputManager {
private:
    /**
     * @brief used to determine how large the deadzone on control sticks will be
     * control stick range is -128 to 127
     * A value of 30 means that any value between -30 and 30 will register as 0
     *
     */
    const int JOYSTICK_DEADZONE = 30;

    /**
     * @brief An array of InputEvent that represent user input see
     * InputManager.cpp
     */
    std::shared_ptr<InputEvent> buttonEvents[14];
    std::shared_ptr<InputEvent> controlStickEvents[8];
    std::shared_ptr<InputEvent> dpadEvents[4];

public:
    /**
     * @brief Construct a new Input Manager object
     *
     * @param tree A dictionary of an input name to an InputEvent (see
     * InputEvent instances for a detailed desciption)
     * @param factory The factory used to construct InputEvents from the tree
     */
    InputManager(const boost::property_tree::ptree &tree,
                 InputEventFactory &factory);

    /**
     * @brief Gets the user's inputs in the form of a uint8_t array
     *
     */
    std::array<uint8_t, 8> getData() const;

    /**
     * @brief Get the Schema of the InputManager.
     *
     * @return std::vector<InputEvent::SchemaItem> the Schema of the
     * InputManager.
     */
    static std::vector<InputEvent::SchemaItem> getSchema();

private:
    /**
     * @brief loads an InputEvent into the input array
     *
     * @param it the iterator containing a name of the input and an InputEvent
     */
    void loadInputEvent(
        const std::pair<const std::string, boost::property_tree::ptree> &it,
        InputEventFactory &factory);

    /**
     * @brief Tests if a key is in a map and returns its index else -1
     *
     * @param map The map to be searched
     * @param key The key to be matched
     * @return int The found index or -1
     */
    int testInMap(const std::map<std::string, int> &map,
                  const std::string &key);

    /**
     * @brief Reports if a stick is in the deadzone
     *
     * @param stick The stick whose value is returned
     * @return 128 - JOYSTICK_DEADZONE < stickData < 128 + JOYSTICK_DEADZONE
     */
    bool isInDeadzone(int stick) const;

    /**
     * @brief Get the Control Stick Data at the given stick number
     *
     * @param stick The stick whose value is returned
     * @return unsigned char The value of the control stick
     */
    unsigned char getControlStickData(int stick) const;

    /**
     * @brief Gets the Dpad data from the inputs array
     *
     * @return unsigned char The Dpad value
     */
    unsigned char getDpadData() const;
    /**
     * @brief Get the Dpad data from the provided arguments
     *
     * @param up if up is pressed
     * @param right if right is pressed
     * @param down if down is pressed
     * @param left if left is pressed
     * @return unsigned char The Dpad value
     */
    unsigned char getDpadData(bool up, bool right, bool down, bool left) const;
};

#endif