#include QMK_KEYBOARD_H

#define DELTA_X_THRESHOLD 60
#define DELTA_Y_THRESHOLD 15

// safe range starts at `PLOOPY_SAFE_RANGE` instead.

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {{{ KC_NO }}};

static bool num_lock_state = false;

static int8_t delta_x = 0;
static int8_t delta_y = 0;

void keyboard_post_init_user(void) {
    num_lock_state  = host_keyboard_led_state().num_lock;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    static uint16_t roll_timer = 0;
    static int8_t roll_delta_x = 0;
    static int8_t roll_delta_y = 0;
    static bool scroll_lock_on = false;

    roll_delta_x += mouse_report.x;
    roll_delta_y += mouse_report.y;

    if (roll_delta_x > DELTA_X_THRESHOLD || delta_x < -DELTA_X_THRESHOLD ||
        roll_delta_y > DELTA_Y_THRESHOLD || delta_y < -DELTA_Y_THRESHOLD
    ) {
        roll_timer = timer_read();
        roll_delta_x = roll_delta_y = 0;
        if (!scroll_lock_on) {
            scroll_lock_on = true;
            tap_code(KC_SCROLL_LOCK);
        }
    }
    else if (scroll_lock_on && timer_elapsed(roll_timer) > 300) {
        scroll_lock_on = false;
        tap_code(KC_SCROLL_LOCK);
    }

    if (num_lock_state) {
        delta_x += mouse_report.x;
        delta_y += mouse_report.y;

        if (delta_x > DELTA_X_THRESHOLD) {
            mouse_report.h = 1;
            delta_x        = 0;
        } else if (delta_x < -DELTA_X_THRESHOLD) {
            mouse_report.h = -1;
            delta_x        = 0;
        }

        if (delta_y > DELTA_Y_THRESHOLD) {
            mouse_report.v = -1;
            delta_y        = 0;
        } else if (delta_y < -DELTA_Y_THRESHOLD) {
            mouse_report.v = 1;
            delta_y        = 0;
        }
        mouse_report.x = 0;
        mouse_report.y = 0;
    }
    return mouse_report;
}

bool led_update_user(led_t led_state) {
    num_lock_state = led_state.num_lock;
    return true;
}
