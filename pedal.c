#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <usb/usb_hid.h>

#define SWITCH_PIN &gpio_c0   // Change if using a different GPIO pin
#define DEBOUNCE_MS 50

typedef enum {
    SwitchClosed = 0,
    SwitchOpen = 1
} SwitchState;

int32_t usb_switch_arrow_app(void* p) {
    UNUSED(p);

    // Configure GPIO as input with pull-up (NC switch pulls to ground)
    furi_hal_gpio_init(SWITCH_PIN, GpioModeInput, GpioPullUp, GpioSpeedLow);

    // Enable USB HID keyboard mode
    usb_hid_start();
    usb_hid_set_keyboard(true);

    SwitchState last_state = SwitchClosed;

    while(true) {
        bool level = furi_hal_gpio_read(SWITCH_PIN); // true = open, false = closed
        SwitchState current_state = level ? SwitchOpen : SwitchClosed;

        // Detect "opened → closed" transition
        if(last_state == SwitchOpen && current_state == SwitchClosed) {
            // Debounce
            furi_delay_ms(DEBOUNCE_MS);

            // Send LEFT ARROW key
            usb_hid_kbd_press(HID_KEY_ARROW_LEFT);
            usb_hid_kbd_release(HID_KEY_ARROW_LEFT);
        }

        last_state = current_state;
        furi_delay_ms(10);
    }

    return 0;
}
