#include "controller.h"

#include <string.h>

#include "gl.h"

// MARK: - Functions

/// Get the GLFW representation of the given controller button.
/// @param button The button to get the GLFW representation of.
/// @return The GLFW representation of the given button.
/// See `gamepad_buttons` in `glfw3.h` for further documentation.
int controller_button_to_glfw(enum controller_button_t button)
{
    switch (button)
    {
        case CONTROLLER_CROSS:      return GLFW_GAMEPAD_BUTTON_CROSS;
        case CONTROLLER_CIRCLE:     return GLFW_GAMEPAD_BUTTON_CIRCLE;
        case CONTROLLER_SQUARE:     return GLFW_GAMEPAD_BUTTON_SQUARE;
        case CONTROLLER_TRIANGLE:   return GLFW_GAMEPAD_BUTTON_TRIANGLE;
        case CONTROLLER_L1:         return GLFW_GAMEPAD_BUTTON_LEFT_BUMPER;
        case CONTROLLER_R1:         return GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER;
        case CONTROLLER_OPTIONS:    return GLFW_GAMEPAD_BUTTON_START;
        case CONTROLLER_L3:         return GLFW_GAMEPAD_BUTTON_LEFT_THUMB;
        case CONTROLLER_R3:         return GLFW_GAMEPAD_BUTTON_RIGHT_THUMB;
        case CONTROLLER_DPAD_UP:    return GLFW_GAMEPAD_BUTTON_DPAD_UP;
        case CONTROLLER_DPAD_DOWN:  return GLFW_GAMEPAD_BUTTON_DPAD_DOWN;
        case CONTROLLER_DPAD_LEFT:  return GLFW_GAMEPAD_BUTTON_DPAD_LEFT;
        case CONTROLLER_DPAD_RIGHT: return GLFW_GAMEPAD_BUTTON_DPAD_RIGHT;
    }
}

/// Get the GLFW representation of the given controller axis.
/// @param axis The axis to get the GLFW representation of.
/// @return The GLFW representation of the given axis.
/// See `gamepad_axes` in `glfw3.h` for further documentation.
int controller_axis_to_glfw(enum controller_axis_t axis)
{
    switch (axis)
    {
        case CONTROLLER_LEFT_X:  return GLFW_GAMEPAD_AXIS_LEFT_X;
        case CONTROLLER_LEFT_Y:  return GLFW_GAMEPAD_AXIS_LEFT_Y;
        case CONTROLLER_RIGHT_X: return GLFW_GAMEPAD_AXIS_RIGHT_X;
        case CONTROLLER_RIGHT_Y: return GLFW_GAMEPAD_AXIS_RIGHT_Y;
        case CONTROLLER_L2:      return GLFW_GAMEPAD_AXIS_LEFT_TRIGGER;
        case CONTROLLER_R2:      return GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER;
    }
}

void controller_init(struct controller_t *controller)
{
    // initialize the given controller
    controller->is_paired = false;
}

void controller_deinit(struct controller_t *controller)
{
}

bool controller_try_pair(struct controller_t *controller,
                         unsigned int num_pair_buttons,
                         const enum controller_button_t *pair_buttons)
{
    // try to pair with the first gamepad pressing the given pair combo
    for (int joystick = GLFW_JOYSTICK_1; joystick <= GLFW_JOYSTICK_LAST; joystick++)
    {
        // get the state of the current gamepad
        GLFWgamepadstate state;
        if (!glfwGetGamepadState(joystick, &state))
            // not a gamepad, skip
            continue;

        // get whether or not the given pair combo is currently being pressed
        bool is_pair_pressed = true;
        for (int i = 0; i < num_pair_buttons; i++)
        {
            int glfw_button = controller_button_to_glfw(pair_buttons[i]);
            bool is_button_pressed = state.buttons[glfw_button] == GLFW_PRESS;
            is_pair_pressed = is_pair_pressed && is_button_pressed;
        }

        // pair the given controller to the current gamepad if the pair combo is currently being pressed
        if (is_pair_pressed)
        {
            controller->is_paired = true;
            controller->gamepad = joystick;
            controller->gamepad_name = glfwGetGamepadName(joystick);
            return true;
        }
    }

    // if this point is reached then no gamepad is pressing the given pair combo
    return false;
}

const char *controller_get_name(struct controller_t *controller)
{
    if (controller->is_paired)
        return controller->gamepad_name;
    else
        return NULL;
}

void controller_update(struct controller_t *controller)
{
    // return early if the given controller is unpaired, as theres no gamepad to update from
    if (!controller->is_paired)
        return;

    // get the new state
    GLFWgamepadstate state;
    if (!glfwGetGamepadState(controller->gamepad, &state))
    {
        // the gamepad paired to the given controller is no longer available, unpair it
        controller->is_paired = false;
        return;
    }

    // update the current state of the given controller
    // buttons
    for (enum controller_button_t button = 0; button < CONTROLLER_NUM_BUTTONS; button++)
    {
        int glfw_button = controller_button_to_glfw(button);
        bool is_button_pressed = state.buttons[glfw_button] == GLFW_PRESS;
        controller->buttons[button] = is_button_pressed;
    }

    // axes
    for (enum controller_axis_t axis = 0; axis < CONTROLLER_NUM_AXES; axis++)
    {
        int glfw_axis = controller_axis_to_glfw(axis);
        float axis_value = state.axes[glfw_axis];
        controller->axes[axis] = axis_value;
    }
}

bool controller_get_button(struct controller_t *controller,
                           enum controller_button_t button)
{
    if (!controller->is_paired)
        return false;

    return controller->buttons[button];
}

float controller_get_axis(struct controller_t *controller,
                          enum controller_axis_t axis)
{
    if (!controller->is_paired)
        return 0;

    return controller->axes[axis];
}
