#pragma once

#include <stdbool.h>

///
/// Controllers pair to gamepads to read their inputs.
///
/// Controllers are meant to be thought of more as slots as opposed to direct gamepads.
/// For example, in a co-op experience, Player 1 and 2 would each have a single controller internally.
///
/// "Gamepad" refers to a joystick connected to the system, which is detected as having standard controls such as:
///  - Left and right triggers and analog sticks.
///  - Directional pad.
///  - Action buttons.
/// And various others.
///
/// On their own controllers do nothing, they must first be paired to a gamepad.
/// "Pairing" refers to linking a controller to a gamepad, which then lets the controller read state from it.
/// Pairing works by check for a "pair combo"; a combination of buttons which must be pressed simultaneously to pair a gamepad to a controller.
/// Something like the Controllers menu on the Nintendo Switch is an example of the experience that can be provided with this system.
///

// MARK: - Enumerations

/// The different buttons that can be present on a single controller.
///
/// Each case is named after the DualShock 4 input,
/// but they each match to the equivalent physical input on other supported gamepads.
enum controller_button_t
{
    /// Cross, also known as X, or A in XBox notation.
    CONTROLLER_CROSS = 0,

    /// Circle, or B in XBox notation.
    CONTROLLER_CIRCLE,

    /// Square, or X in XBox notation.
    CONTROLLER_SQUARE,

    /// Triangle, or Y in XBox notation.
    CONTROLLER_TRIANGLE,

    /// L1, or left bumper in XBox notation.
    CONTROLLER_L1,

    /// R1, or right bumper in XBox notation.
    CONTROLLER_R1,

    /// Options, or start in XBox notation.
    CONTROLLER_OPTIONS,

    /// L3, or left thumb in XBox notation.
    CONTROLLER_L3,

    /// R3, or right thumb in XBox notation.
    CONTROLLER_R3,

    /// Up on the directional pad.
    CONTROLLER_DPAD_UP,

    /// Down on the directional pad.
    CONTROLLER_DPAD_DOWN,

    /// Left on the directional pad.
    CONTROLLER_DPAD_LEFT,

    /// Right on the directional pad.
    CONTROLLER_DPAD_RIGHT,

    /// The total number of buttons that can be present on a single controller.
    CONTROLLER_NUM_BUTTONS,
};

/// The different axes that can be present on a single controller.
///
/// Each case is named after the DualShock 4 input,
/// but they each match to the equivalent physical input on other supported gamepads.
enum controller_axis_t
{
    /// X axis of the left analog stick.
    ///
    /// `-1` is left, `0` is center, and `1` is right.
    CONTROLLER_LEFT_X = 0,

    /// Y axis of the left analog stick.
    ///
    /// `-1` is top, `0` is center, and `1` is bottom.
    CONTROLLER_LEFT_Y,

    /// X axis of the right analog stick.
    ///
    /// `-1` is left, `0` is center, and `1` is right.
    CONTROLLER_RIGHT_X,

    /// Y axis of the right analog stick.
    ///
    /// `-1` is top, `0` is center, and `1` is bottom.
    CONTROLLER_RIGHT_Y,

    /// L2, or left trigger in XBox notation.
    ///
    /// `-1` is released, and `1` is pressed.
    CONTROLLER_L2,

    /// R2, or right trigger in XBox notation.
    ///
    /// `-1` is released, and `1` is pressed.
    CONTROLLER_R2,

    /// The total number of axes that can be present on a single controller.
    CONTROLLER_NUM_AXES,
};

// MARK: - Data Structures

/// A controller.
struct controller_t
{
    /// Whether or not this controller is currently paired to a gamepad.
    bool is_paired;

    /// The unique GLFW identifier of the gamepad that this controller is currently paired to, if any.
    int gamepad;

    /// The name of the gamepad that this controller is currently paired to, if any.
    ///
    /// The lifetime of this pointer is handled by GLFW, and is only available while the gamepad is connected.
    const char *gamepad_name;

    /// Whether each of this controller's buttons are currently pressed or released.
    bool buttons[CONTROLLER_NUM_BUTTONS];

    /// The current value of each of this controller's axes, normalized from `-1` to `1`.
    float axes[CONTROLLER_NUM_AXES];
};

// MARK: - Functions

/// Initialize the given controller.
/// @param controller The controller to initialize.
void controller_init(struct controller_t *controller);

/// Deinitialize the given controller, releasing all of its allocated resources.
/// @param controller The controller to deinitialize.
void controller_deinit(struct controller_t *controller);

/// Attempt to pair the given controller to the first gamepad currently pressing the given pair combo.
///
/// Note that multiple controllers can be paired to the same gamepad, as pairing is not exclusive.
/// If the given pair combo is empty then the first found gamepad is paired to.
/// If the given controller is already paired and there is a gamepad pressing the given pair combo then it is re-paired.
/// @param controller The controller to pair.
/// @param num_pair_buttons The total number of buttons within the pair combo.
/// @param pair_buttons All the buttons within the pair combo that must be pressed simultaneously.
/// @return Whether or not the given controller was paired.
bool controller_try_pair(struct controller_t *controller,
                         unsigned int num_pair_buttons,
                         const enum controller_button_t *pair_buttons);

/// Get the name of the gamepad that the given controller is currently paired to, if any.
///
/// The returned pointer is only available while the gamepad is connected.
/// @return The null-terminated human-readable name of the gamepad that the given controller is currently paired to.
/// If the given controller is not currently paired then `NULL` is returned instead.
const char *controller_get_name(struct controller_t *controller);

/// Update the current state of the given controller.
///
/// If the given controller is not paired then this function does nothing.
/// If the gamepad paired that the given controller is paired to is unavailable during this function then it is unpaired.
/// @param controller The controller to update.
void controller_update(struct controller_t *controller);

/// Get whether or not the given button is pressed on the given controller.
///
/// This value is not updated until the given controller is updated.
/// If the given controller is unpaired then the given button is assumed to be released.
/// @param controller The controller to get the given button's state from.
/// @param button The button to check.
/// @return Whether or not the given button is pressed on the given controller.
bool controller_get_button(struct controller_t *controller,
                           enum controller_button_t button);

/// Get the value of the given axis on the given controller.
///
/// This value is not updated until the given controller is updated.
/// If the given controller is unpaired then the given axis' value is assumed to be zero.
/// @param controller The controller to get the given axis' value from.
/// @param axis The axis to get the value of.
/// @return The current value of the given axis on the given controller, normalized from `-1` to `1`.
float controller_get_axis(struct controller_t *controller,
                          enum controller_axis_t axis);
