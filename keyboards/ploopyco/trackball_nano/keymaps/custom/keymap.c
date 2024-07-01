#include QMK_KEYBOARD_H
#include <math.h>

#define DELTA_X_THRESHOLD 50
#define DELTA_Y_THRESHOLD 25

// safe range starts at `PLOOPY_SAFE_RANGE` instead.

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {{{ KC_NO }}};

static bool num_lock_state = false;

static int8_t delta_x = 0;
static int8_t delta_y = 0;

void keyboard_post_init_user(void) {
    num_lock_state  = host_keyboard_led_state().num_lock;
}

void handle_mouse_acceleration(report_mouse_t* mouse_report) {
    #define NB_DISTANCE_VALUES 100
    static float distance_values[NB_DISTANCE_VALUES] = {};
    static float total_distance = 0;
    static size_t distance_index = 0;

    // cast to u16 to prevent integer overflow when squaring the value.
    const int16_t mouse_x = mouse_report->x;
    const int16_t mouse_y = mouse_report->y;

    const float old_distance = distance_values[distance_index];
    const float new_distance = sqrt(mouse_x * mouse_x + mouse_y * mouse_y);

    total_distance -= old_distance;
    total_distance += distance_values[distance_index] = new_distance;

    distance_index = (distance_index + 1) % NB_DISTANCE_VALUES;

    const float gain = 2;
    const float accel = 1 + total_distance * gain / NB_DISTANCE_VALUES;

    mouse_report->x *= MIN(accel, 6.);
    mouse_report->y *= MIN(accel, 6.);
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    handle_mouse_acceleration(&mouse_report);

    static uint16_t roll_timer = 0;
    static bool scroll_lock_on = false;

    if (num_lock_state) {
        delta_x += mouse_report.x;
        delta_y += mouse_report.y;

        mouse_report.h =  delta_x / DELTA_X_THRESHOLD;
        mouse_report.v = -delta_y / DELTA_Y_THRESHOLD;

        delta_y %= DELTA_Y_THRESHOLD;
        delta_x %= DELTA_X_THRESHOLD;

        mouse_report.x = 0;
        mouse_report.y = 0;

        return mouse_report;
    }

    if (delta_x > DELTA_X_THRESHOLD || delta_x < -DELTA_X_THRESHOLD ||
        delta_y > DELTA_Y_THRESHOLD || delta_y < -DELTA_Y_THRESHOLD
    ) {
        roll_timer = timer_read();
        delta_x = delta_y = 0;
        if (!scroll_lock_on) {
            scroll_lock_on = true;
            tap_code(KC_SCROLL_LOCK);
        }
    }
    else if (scroll_lock_on && timer_elapsed(roll_timer) > 300) {
        scroll_lock_on = false;
        tap_code(KC_SCROLL_LOCK);
    }

    return mouse_report;
}

bool led_update_user(led_t led_state) {
    num_lock_state = led_state.num_lock;
    delta_x = delta_y = 0;
    return true;
}
